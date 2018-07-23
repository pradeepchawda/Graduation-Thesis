//
//  cluster.h
//  980Merge
//
//  Created by Hensin on 2016-02-25.
//  Copyright (c) 2016 Hensin. All rights reserved.
//

#ifndef ___80Merge__cluster__
#define ___80Merge__cluster__

#include "fplan.h"
#include "hv_matrix.h"

class Cluster{
    
private:
    int X, Y;     //left-bottom corner of the cluster
    int X_, Y_;   //right-top corner of the cluster
    int dx, dy;   //the gap between this cluster and the cluster left to it and the cluster under it
    char name;    //the name of this cluster
    int lc;       //sign the location in a vector of clusters
    int Width, Height, Area;        //the width, height, area of this cluster
    Modules modules;                //the modules that this cluster contains
    Modules_Info modules_Info;      //the information of the modules in this cluster
    Modules_Info Hcontour;          //the horizontal contour of this cluster in terms of module
    Modules_Info Vcontour;          //the vertical contour of this cluster in terms of module
    vector<int> final_packingSq;    //the packing sequence after SA
    Hv_Matrix cltmatrix;            //stores the geometric relationships between the modules after SA
    
public:
    Cluster() {};
    ~Cluster() {};
    
    int label;
    int pt;
    
    int getX() {return X;};
    int getY() {return Y;};
    int getX_() {return X_;};
    int getY_() {return Y_;};
    int getDx() {return dx;};
    int getDy() {return dy;};
    char getName() {return name;};
    int getLc() {return lc;};
    int getWidth() {return Width;};
    int getHeight() {return Height;};
    int getArea() {return Area;};
    Modules_Info getHcontour() {return Hcontour;};
    Modules_Info getVcontour() {return Vcontour;};
    Modules_Info getModules_Info() {return modules_Info;};
    Modules getModules() {return modules;};
    Hv_Matrix getCltmatrix() {return cltmatrix;};
    vector<int> getPackingSq() {return final_packingSq;};
    
    void setX(int x) {X = x;};
    void setY(int y) {Y = y;};
    void setX_(int x) {X_ = x;};
    void setY_(int y) {Y_ = y;};
    void setDxy(int x, int y) {dx = x; dy = y;};
    void setName(char n) {name = n;};
    void setLc(int l) {lc = l;};
    void setWidth(int w) {Width = w;};
    void setHeight(int h) {Height = h;};
    void setArea(int a) {Area = a;};
    void setHcontour(vector<Module_Info> h) {Hcontour = h;};
    void setVcontour(vector<Module_Info> v) {Vcontour = v;};
    void setModules_Info(Modules_Info m) {modules_Info = m;};
    void setModules(Modules m) {modules = m;};
    void setCltmatrix(Hv_Matrix m) {cltmatrix = m;};
    void setPackingSq(vector<int> s) {final_packingSq = s;};
    
    void update();
    void show_cluster();
    
};

#endif /* defined(___80Merge__cluster__) */
