//
//  module_test.cc
//  980MergeSA2
//
//  Created by Hensin on 2016-04-03.
//  Copyright (c) 2016 Hensin. All rights reserved.
//

//
//  merge_test.cc
//  980Merge
//
//  Created by Hensin on 2016-02-24.
//  Copyright (c) 2016 Hensin. All rights reserved.
//

#include "msa.h"

int main(int argc,char **argv)
{
    cout << "Result of Merging clusters:" << endl;
    char filename[80];
    int times=10;
    float init_temp=0.9,term_temp=0.3,temp_scale=0.9,cvg_r=0.95;
    
    
    if(argc<=1){
        cout<<"Please input at least two filenames"<<endl;
        return 0;
    }else{
        int argi=1;
        if(argi < argc) strcpy(filename,argv[argi++]);
        if(argi < argc) init_temp=atof(argv[argi++]);
        if(argi < argc) temp_scale=atof(argv[argi++]);
        if(argi < argc) term_temp=atof(argv[argi++]);
        if(argi < argc) cvg_r=atof(argv[argi++]);
        if(argi < argc) times=atoi(argv[argi++]);
    }
    
    MSA msa;
    Clo_Red_Graph fp;
    fp.read(filename);
    fp.init_sqr();
    fp.show_modules();
//    fp.contour_packing();
//    fp.perturbate5();
//    fp.contour_packing();
//    fp.perturbate6();
//    fp.contour_packing();
   msa.sa_floorplanning(fp,init_temp,temp_scale,term_temp,cvg_r,times);
    int area=fp.getArea();
    cout<<"Best Area="<< area<<endl;
    cout<<endl;
    fp.show_graph();
    
    
    return 1;
}