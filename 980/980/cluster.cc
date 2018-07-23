//
//  cluster.cc
//  980Merge
//
//  Created by Hensin on 2016-02-25.
//  Copyright (c) 2016 Hensin. All rights reserved.
//

#include "cluster.h"
#include <fstream>


//------------------------------------------------------------------------
//Update the information of module, horizontal contour, and vertical contour,
//according to the coordinate of the cluster
//------------------------------------------------------------------------
void Cluster::update(){

    //update the module information
    for(int i=0; i<modules_Info.size(); i++){
        modules_Info[i].x = X + modules_Info[i].rdx;
        modules_Info[i].y = Y + modules_Info[i].rdy;
        modules_Info[i].rx = X + modules_Info[i].rdx + modules_Info[i].width;
        modules_Info[i].ry = Y +modules_Info[i].rdy + modules_Info[i].height;
    }

    //update the horizontal contour
    Modules_Info HC;
    for(int i=0; i<Hcontour.size(); i++){
        for(int j=0; j<modules_Info.size(); j++){
            if(Hcontour[i].name == modules_Info[j].name){
                HC.push_back(modules_Info[j]);
            }
        }
    }
    Hcontour = HC;
    
    //update the vertical contour
    Modules_Info VC;
    for(int i=0; i<Vcontour.size(); i++){
        for(int j=0; j<modules_Info.size(); j++){
            if(Vcontour[i].name == modules_Info[j].name){
                VC.push_back(modules_Info[j]);
            }
        }
    }
    Vcontour = VC;
    
    //update X_ and Y_
    X_ = X + Width;
    Y_ = Y + Height;
}





