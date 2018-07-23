//
//  My_window.h
//  980MergeSA2
//
//  Created by Hensin on 2016-03-30.
//  Copyright (c) 2016 Hensin. All rights reserved.
//
#ifndef ___80Merge__My_window__
#define ___80Merge__My_window__

#include "GUI.h"	// for Simple_window only (doesn't really belong in Window.h)
#include "Graph.h"

using namespace Graph_lib;

//Build a class of My_window to include the drawing of cluster and two buttons
class My_window : Graph_lib::Window {
    
public:
    My_window(Graph_lib::Point xy, int w, int h, vector<Cluster> clts, const string& title );
    
    CLMO clmo;
    
private:
    Button button_zoomIn;
    Button button_zoomOut;
	
    void zoom_in();
    void zoom_out();
};

#endif
