#include<iostream>
#include<vector>
#include<string>
#include<fstream>
#include<sstream>
using namespace std;


const unsigned int PRIMARY_KEY = 1;   
const unsigned int NOT_NULL    = 2;   
const unsigned int UNIQUE      = 4;   


class Row{
public:
    vector<string> values;

    Row(vector<string> vals){
        values = vals;
    }
};

class Column{
public:
    string name;
    string type;
    unsigned int constraints;

    Column(string n , string t , unsigned int c){
        name = n;
        type = t;
        constraints = c;
    }
};


class Table{
public:
    string tablename;
    vector<Column> columns;
    vector<Row*> rows;

    Table(string name){
        tablename = name;
    }

    ~Table(){
        for(Row* r : rows){
            delete r;
        }
        rows.clear();
    }


    void insertRow(vector<string> vals){

        if(vals.size() != columns.size()){
            cout<<"Column count mismatch!\n";
            return;
        }

        for(int i=0; i<columns.size(); i++){

        
            if( (columns[i].constraints & NOT_NULL) && vals[i].empty() ){
                cout<<"Error: NOT NULL violation on "<<columns[i].name<<"\n";
                return;
            }

          
            if( (columns[i].constraints & PRIMARY_KEY) || 
                (columns[i].constraints & UNIQUE) ){

                for(Row* r : rows){
                    if(r->values[i] == vals[i]){
                        cout<<"Error: Duplicate value for "<<columns[i].name<<"\n";
                        return;
                    }
                }
            }
        }

        Row* newRow = new Row(vals);
        rows.push_back(newRow);

        cout<<"Record inserted successfully.\n";
    }

   
    void selectAll(){

        for(Column c : columns)
            cout<<c.name<<"\t";
        cout<<"\n";

        for(Row* r : rows){
            for(string val : r->values)
                cout<<val<<"\t";
            cout<<"\n";
        }
    }

  
    void saveToFile(string filename){

        fstream file(filename , ios::out);

        if(!file){
            cout<<"File open error!\n";
            return;
        }

        file<<"TABLE "<<tablename<<"\n";

        for(Column c : columns){
            file<<c.name<<" "<<c.type<<" "<<c.constraints<<"\n";
        }

        file<<"DATA\n";

        for(Row* r : rows){
            for(string val : r->values)
                file<<val<<" ";
            file<<"\n";
        }

        file.close();
        cout<<"Table saved successfully.\n";
    }

    void loadFromFile(string filename){

        fstream file(filename , ios::in);

        if(!file){
            cout<<"File not found!\n";
            return;
        }

        for(Row* r : rows)
            delete r;

        rows.clear();
        columns.clear();

        string line;

        getline(file , line);
        tablename = line.substr(6);

        while(getline(file , line)){

            if(line == "DATA")
                break;

            stringstream ss(line);
            string name , type;
            unsigned int cons;

            ss >> name >> type >> cons;

            columns.push_back(Column(name , type , cons));
        }

    
        while(getline(file , line)){

            if(line.empty())
                continue;

            stringstream ss(line);
            vector<string> vals;
            string val;

            while(ss >> val)
                vals.push_back(val);

            Row* r = new Row(vals);
            rows.push_back(r);
        }

        file.close();
        cout<<"Table loaded successfully.\n";
    }
};

int main(){

    Table* mytable = nullptr;
    char buffer[256];

    while(true){

        cout<<"\nCommand (CREATE/INSERT/SELECT/SAVE/LOAD/EXIT): ";
        cin.getline(buffer , 256);
        string cmd(buffer);

        
        if(cmd.substr(0,6) == "CREATE"){

            string tname;
            cout<<"Enter table name: ";
            cin>>tname;

            if(mytable)
                delete mytable;

            mytable = new Table(tname);

            int colcount;
            cout<<"Number of columns: ";
            cin>>colcount;

            for(int i=0; i<colcount; i++){

                string name , type;
                unsigned int cons;

                cout<<"Column "<<i+1<<" name: ";
                cin>>name;

                cout<<"Type (int/string): ";
                cin>>type;

                cout<<"Constraints (1=PK 2=NOT_NULL 4=UNIQUE): ";
                cin>>cons;

                mytable->columns.push_back(Column(name , type , cons));
            }

            cin.ignore();
            cout<<"Table created successfully.\n";
        }

  
        else if(cmd.substr(0,6) == "INSERT"){

            if(!mytable){
                cout<<"Create table first!\n";
                continue;
            }

            vector<string> vals;

            for(Column c : mytable->columns){

                string val;
                cout<<"Enter value for "<<c.name<<": ";
                cin>>val;
                vals.push_back(val);
            }

            cin.ignore();
            mytable->insertRow(vals);
        }

        
        else if(cmd.substr(0,6) == "SELECT"){

            if(!mytable){
                cout<<"Create table first!\n";
                continue;
            }

            mytable->selectAll();
        }

   
        else if(cmd.substr(0,4) == "SAVE"){

            if(!mytable){
                cout<<"Create table first!\n";
                continue;
            }

            string fname;
            cout<<"Enter filename: ";
            cin>>fname;
            cin.ignore();

            mytable->saveToFile(fname);
        }

 
        else if(cmd.substr(0,4) == "LOAD"){

            string fname;
            cout<<"Enter filename: ";
            cin>>fname;
            cin.ignore();

            if(mytable)
                delete mytable;

            mytable = new Table("");
            mytable->loadFromFile(fname);
        }

        else if(cmd.substr(0,4) == "EXIT"){
            break;
        }

        else{
            cout<<"Unknown command!\n";
        }
    }

    if(mytable)
        delete mytable;

    return 0;
}