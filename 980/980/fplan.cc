#include "fplan.h"
#include <fstream>
#include <cstdio>
#include <cstring>
#include <climits>
#include <iostream>

//---------------------------------------------------------------------------
char line[100],t1[40],t2[40];
ifstream fs;

char* tail(char *str){
    str[strlen(str)-1]=0;
    return str;
}

/*
 void error(char *msg,char *msg2){
 printf(msg,msg2);  cout << endl;
 throw 1;
 }*/

void FloorPlan::read(char* file){
    
    fs.open(file);
    if(!fs)
        //    error("unable to open file: %s",file);
        cout<<"unable to open file: %s"<<endl;
    
    bool final=false;
    Module dummy_mod;
    for(int i=0; !fs.eof(); i++){
        // modules
        modules.push_back(dummy_mod);	// new module
        Module &mod = modules.back();
        mod.id = i;
        mod.pins.clear();
        
        fs >> t1 >> t2;
        //cout << t1 << endl;
        tail(t2);			// remove ";"
        strcpy(mod.name,t2);
        
        fs >> t1 >> t2;
        if(!strcmp(t2,"PARENT;"))
            final= true;
        
        // dimension
        read_dimension(mod);
        read_IO_list(mod,final);
        
        // network
        if(final){
            read_network();
            break;
        }
    }
    create_network();
    
    root_module = modules.back();
    modules_N = int(modules.size())-1;  // exclude the parent module
    modules_info.resize(modules_N);
    //modules.resize(modules_N);
    
    TotalArea = 0;
    for(int i=0; i < modules_N; i++){
        TotalArea += modules[i].area;
        modules_info[i].width=modules[i].width;
        modules_info[i].height=modules[i].height;
        modules_info[i].degree=0;
        modules_info[i].name = i+97;
        modules_info[i].lc=i;
        cout<<modules_info[i].name<<" "<<modules_info[i].width<<" "<<modules_info[i].height<<endl;
    }
    
    fs.close();
}

void FloorPlan::read_dimension(Module &mod){
    fs >> t1;
    int min_x=INT_MAX,min_y=INT_MAX,max_x=INT_MIN,max_y=INT_MIN;
    int tx,ty;
    for(int i=0; i < 4;i++){
        fs >> tx >> ty;
        min_x=min(min_x,tx); max_x=max(max_x,tx);
        min_y=min(min_y,ty); max_y=max(max_y,ty);
    }
    
    mod.x      = min_x;
    mod.y      = min_y;
    mod.width  = max_x - min_x;
    mod.height = max_y - min_y;
    mod.area   = mod.width * mod.height;
    fs >> t1 >> t2;
}

void FloorPlan::read_IO_list(Module &mod,bool parent=false){
    // IO list
    while(!fs.eof()){
        Pin p;
        fs.getline(line,100);
        if(strlen(line)==0) continue;
        sscanf(line,"%s %*s %d %d",t1,&p.x,&p.y);
        
        if(!strcmp(t1,"ENDIOLIST;"))
            break;
        
        if(parent){ // IO pad is network
            // make unique net id
            net_table.insert(make_pair(string(t1),net_table.size()));
            p.net = net_table[t1];
        }
        
        p.mod = mod.id;
        p.x -= mod.x;  p.y -= mod.y;	// shift to origin
        //if(mod.id==9){
        //  cout<<"p.x="<<p.x<<endl;
        //  cout<<"p.y="<<p.y<<endl;
        //}
        mod.pins.push_back(p);
    }
    fs.getline(line,100);
}

void FloorPlan::read_network(){
    while(!fs.eof()){
        bool end=false;
        int n=0;
        fs >> t1 >> t2;
        if(!strcmp(t1,"ENDNETWORK;"))
            break;
        // determine which module interconnection by name
        int m_id;
        for(m_id=0; m_id < modules.size(); m_id++)
            if(!strcmp(modules[m_id].name,t2))
                break;
        if(m_id== modules.size())
            //	error("can't find suitable module name!");
            cout<<"can't find suitable module name!"<<endl;
        while(!fs.eof()){
            fs >> t1;
            if(t1[strlen(t1)-1]==';'){
                tail(t1);
                end=true;
            }
            
            // make unique net id
            net_table.insert(make_pair(string(t1),net_table.size()));
            modules[m_id].pins[n++].net = net_table[t1];
            if(end) break;
        }
    }
}

void FloorPlan::create_network(){
    network.resize(net_table.size());
    
    for(int i=0; i < modules.size(); i++){
        for(int j=0; j < modules[i].pins.size(); j++){
            Pin &p = modules[i].pins[j];
            network[p.net].push_back(&p);
        }
    }
    
}
/*
 void FloorPlan::getWireLength(){
 int length=0;
 
 // compute absolute position
 for(int i=0; i < modules_info.size(); i++){
 int mx= modules_info[i].x, my= modules_info[i].y;
 int width=modules[i].width;
 int height=modules[i].height;
 
 for(int j=0; j < modules[i].pins.size(); j++){
 Pin &p = modules[i].pins[j];
 int p_x, p_y;
 
 switch(modules_info[i].degree){
 case 0:{
 p_x=p.x;
 p_y=p.y;
 break;
 }
 case 1:{
 p_x=p.y;
 p_y=width-p.x;
 break;
 }
 case 2:{
 p_x=width-p.x;
 p_y=height-p.y;
 break;
 }
 case 3:{
 p_x=height-p.y;
 p_y=p.x;
 break;
 }
 }
 
 p.ax=p_x+mx;
 p.ay=p_y+my;
 }
 }
 
 for(int i=0; i < network.size(); i++){
 assert(network[i].size() > 0);
 int max_x= INT_MIN, max_y= INT_MIN;
 int min_x= INT_MAX, min_y= INT_MAX;
 
 for(int j=0; j < network[i].size(); j++){
 Pin& p = *network[i][j];
 max_x= max(max_x, p.ax);
 max_y= max(max_y, p.ay);
 min_x= min(min_x, p.ax);
 min_y= min(min_y, p.ay);
 }
 length += (max_x-min_x)+(max_y-min_y);
 }
 Wire=length;
 //  return length;
 }*/

string query_map(map<string,int> M,int value){
    for(map<string,int>::iterator p=M.begin(); p != M.end(); p++){
        if(p->second == value)
            return p->first;
    }
    return "";
}

void FloorPlan::show_modules()
{
    int total_area=0;
    for(int i=0; i < modules.size();i++){
        if(i!=modules.size()-1)
            total_area+=modules[i].area;
        cout << "  Module: " << modules[i].name << endl;
        cout << "  Width = " << modules[i].width;
        cout << "  Height= " << modules[i].height << endl;
        cout << "  Area  = " << modules[i].area << endl;
        cout << modules[i].pins.size() << " Pins:\n";
        //    for(int j=0; j < modules[i].pins.size(); j++){
        //      cout << query_map(net_table,modules[i].pins[j].net) << " ";
        //      cout << modules[i].pins[j].x << " " << modules[i].pins[j].y << endl;
        //    }
    }
    cout<<"Area="<<total_area<<endl;
}

void FloorPlan::show_modules_pins()
{
    for(int i=0; i < modules.size();i++){
        cout << "  Module: " << modules[i].name << endl;
        cout << "  Width = " << modules[i].width;
        cout << "  Height= " << modules[i].height << endl;
        cout << "  Area  = " << modules[i].area << endl;
        cout << "  <X, Y> = <" <<modules_info[i].x << ", "
        << modules_info[i].y<<">"<<endl;
        cout << "  Degree = "<< modules_info[i].degree*90<<endl;
        cout << modules[i].pins.size() << " Pins:\n";
        //    for(int j=0; j < modules[i].pins.size(); j++){
        //      cout << query_map(net_table,modules[i].pins[j].net) << " ";
        //      cout << modules[i].pins[j].x << " " << modules[i].pins[j].y << endl;
        //      cout << "after rotate=";
        //      cout << modules[i].pins[j].ax<< " " << modules[i].pins[j].ay<<endl;
        //    }
    }
    cout<<"Area="<<Area<<endl;
}


//void error(char *msg,char *msg2=""){

