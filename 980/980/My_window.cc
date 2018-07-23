//
//  My_window.cc
//  980MergeSA2
//
//  Created by Hensin on 2016-03-30.
//  Copyright (c) 2016 Hensin. All rights reserved.
//

#include "My_window.h"

My_window::My_window(Graph_lib::Point xy, int w, int h, vector<Cluster> clts, const string& title )
     :Graph_lib::Window(xy,w,h,title),
     clmo(clts, 50),
     button_zoomIn{Point(x_max()-70,0), 70, 20, "Zoom In", [](Address, Address pw){reference_to<My_window>(pw).zoom_in();}},
     button_zoomOut{Point(0,0), 70, 20, "Zoom Out", [](Address, Address pw){reference_to<My_window>(pw).zoom_out();}}
     {
         attach(button_zoomIn);
         attach(button_zoomOut);
         attach(clmo);
     };

void My_window::zoom_in(){
    if(clmo.s != 0)
        clmo.s = clmo.s - 10;
    else
        clmo.s =1;
    redraw();
}

void My_window::zoom_out(){
    clmo.s = clmo.s + 10;
    redraw();
}



