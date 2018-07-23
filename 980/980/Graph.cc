//
//  Graph.cc
//  980MergeSA2
//
//  Created by Hensin on 2016-03-30.
//  Copyright (c) 2016 Hensin. All rights reserved.
//
#include "Graph.h"
#include<map>

namespace Graph_lib {

void Shape::draw_lines()
{
	if (color().visibility() && 1<points.size())	// draw sole pixel?
		for (unsigned int i=1; i<points.size(); ++i)
			fl_line(points[i-1].x,points[i-1].y,points[i].x,points[i].y);
}

void Shape::draw()
{
	Fl_Color oldc = fl_color();
	// there is no good portable way of retrieving the current style
	fl_color(lcolor.as_int());
	fl_line_style(ls.style(),ls.width());
	draw_lines();
	fl_color(oldc);	// reset color (to pevious) and style (to default)
	fl_line_style(0);
}


// does two lines (p1,p2) and (p3,p4) intersect?
// if se return the distance of the intersect point as distances from p1
inline pair<double,double> line_intersect(Point p1, Point p2, Point p3, Point p4, bool& parallel) 
{
    double x1 = p1.x;
    double x2 = p2.x;
	double x3 = p3.x;
	double x4 = p4.x;
	double y1 = p1.y;
	double y2 = p2.y;
	double y3 = p3.y;
	double y4 = p4.y;

	double denom = ((y4 - y3)*(x2-x1) - (x4-x3)*(y2-y1));
	if (denom == 0){
		parallel= true;
		return pair<double,double>(0,0);
	}
	parallel = false;
	return pair<double,double>( ((x4-x3)*(y1-y3) - (y4-y3)*(x1-x3))/denom,
								((x2-x1)*(y1-y3) - (y2-y1)*(x1-x3))/denom);
}


//intersection between two line segments
//Returns true if the two segments intersect,
//in which case intersection is set to the point of intersection
bool line_segment_intersect(Point p1, Point p2, Point p3, Point p4, Point& intersection){
   bool parallel;
   pair<double,double> u = line_intersect(p1,p2,p3,p4,parallel);
   if (parallel || u.first < 0 || u.first > 1 || u.second < 0 || u.second > 1) return false;
   intersection.x = p1.x + u.first*(p2.x - p1.x);
   intersection.y = p1.y + u.first*(p2.y - p1.y);
   return true;
}


void Shape::move(int dx, int dy)
{
	for (unsigned int i = 0; i<points.size(); ++i) {
		points[i].x+=dx;
		points[i].y+=dy;
	}
}

void CLMO::draw_lines()
{
    for(int j=0; j<clusters.size(); j++){
        
        //draw the clusters
        float x, y , rx, ry;
        x=clusters[j].getX();
        y=clusters[j].getY();
        rx=clusters[j].getX_();
        ry=clusters[j].getY_();
        
        //draw the modoules in a cluster
        Modules_Info modules_Info = clusters[j].getModules_Info();
        for(int i=0; i<modules_Info.size(); i++){
            float x1, y1, x2, y2;
            x1=modules_Info[i].x;
            y1=modules_Info[i].y;
            x2=modules_Info[i].rx;
            y2=modules_Info[i].ry;
            
            fl_color(FL_BLACK);
            fl_line(x1/s, 700-y1/s, x1/s, 700-y2/s);
            fl_line(x1/s, 700-y1/s, x2/s, 700-y1/s);
            fl_line(x1/s, 700-y2/s, x2/s, 700-y2/s);
            fl_line(x2/s, 700-y1/s, x2/s, 700-y2/s);
            
            if(modules_Info[i].name == 'a')
                fl_draw("a" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'b')
                fl_draw("b" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'c')
                fl_draw("c" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'd')
                fl_draw("d" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'e')
                fl_draw("e" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'f')
                fl_draw("f" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'g')
                fl_draw("g" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'h')
                fl_draw("h" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'i')
                fl_draw("i" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'j')
                fl_draw("j" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'k')
                fl_draw("k" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'l')
                fl_draw("l" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'm')
                fl_draw("m" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
            if(modules_Info[i].name == 'n')
                fl_draw("n" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        }
        
        fl_color(FL_RED);
        fl_line(x/s, 700-y/s, x/s, 700-ry/s);
        fl_line(x/s, 700-y/s, rx/s, 700-y/s);
        fl_line(x/s, 700-ry/s, rx/s, 700-ry/s);
        fl_line(rx/s, 700-y/s, rx/s, 700-ry/s);
        
        if(clusters[j].getName() == 'A')
            fl_draw("A" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'B')
            fl_draw("B" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'C')
            fl_draw("C" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'D')
            fl_draw("D" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'E')
            fl_draw("E" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'F')
            fl_draw("F" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'G')
            fl_draw("G" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'H')
            fl_draw("H" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'I')
            fl_draw("I" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'J')
            fl_draw("J" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'K')
            fl_draw("K" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'L')
            fl_draw("L" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'M')
            fl_draw("M" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
        if(clusters[j].getName() == 'N')
            fl_draw("N" ,int(x/s+(rx/s-x/s)/2), 700-int(y/s+(ry/s-y/s)/2));
    }
}
    

void MO::draw_lines(){
    
    for(int i=0; i<modulesInfo.size(); i++){
        float x1, y1, x2, y2;
        x1=modulesInfo[i].x;
        y1=modulesInfo[i].y;
        x2=modulesInfo[i].rx;
        y2=modulesInfo[i].ry;

        fl_color(FL_BLACK);
        fl_line(x1/s, 700-y1/s, x1/s, 700-y2/s);
        fl_line(x1/s, 700-y1/s, x2/s, 700-y1/s);
        fl_line(x1/s, 700-y2/s, x2/s, 700-y2/s);
        fl_line(x2/s, 700-y1/s, x2/s, 700-y2/s);
        if(modulesInfo[i].name == 'a')
            fl_draw("a" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'b')
            fl_draw("b" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'c')
            fl_draw("c" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'd')
            fl_draw("d" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'e')
            fl_draw("e" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'f')
            fl_draw("f" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'g')
            fl_draw("g" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'h')
            fl_draw("h" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'i')
            fl_draw("i" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'j')
            fl_draw("j" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'k')
            fl_draw("k" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'l')
            fl_draw("l" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'm')
            fl_draw("m" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
        if(modulesInfo[i].name == 'n')
            fl_draw("n" ,int(x1/s+(x2/s-x1/s)/2), 700-int(y1/s+(y2/s-y1/s)/2));
    }
}

} // Graph

