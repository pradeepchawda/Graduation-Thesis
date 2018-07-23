//
//  merge_test.cc
//  980Merge
//
//  Created by Hensin on 2016-02-24.
//  Copyright (c) 2016 Hensin. All rights reserved.
//

#include "csa.h"

int main(int argc,char **argv)
{
    cout << "Result of Merging clusters:" << endl;
    char filename1[80] = {0};
    char filename2[80] = {0};
    char filename3[80] = {0};
    char filename4[80] = {0};
    char filename5[80] = {0};
    char filename6[80] = {0};
    char filename7[80] = {0};
    char filename8[80] = {0};
    char filename9[80] = {0};
    char filename10[80] = {0};
    int times=10;
    float init_temp=0.9,term_temp=0.3,temp_scale=0.9,cvg_r=0.95;
    
    if(argc<=2){
        cout<<"Please input at least two filenames"<<endl;
        return 0;
    }else{
        int argi=1;
        if(argi < argc) strcpy(filename1,argv[argi++]);
        if(argi < argc) strcpy(filename2,argv[argi++]);
        if(argi < argc) strcpy(filename3,argv[argi++]);
        if(argi < argc) strcpy(filename4,argv[argi++]);
        if(argi < argc) strcpy(filename5,argv[argi++]);
        if(argi < argc) strcpy(filename6,argv[argi++]);
        if(argi < argc) strcpy(filename7,argv[argi++]);
        if(argi < argc) strcpy(filename8,argv[argi++]);
        if(argi < argc) strcpy(filename9,argv[argi++]);
        if(argi < argc) strcpy(filename10,argv[argi++]);
        if(argi < argc) init_temp=atof(argv[argi++]);
        if(argi < argc) temp_scale=atof(argv[argi++]);
        if(argi < argc) term_temp=atof(argv[argi++]);
        if(argi < argc) cvg_r=atof(argv[argi++]);
        if(argi < argc) times=atoi(argv[argi++]);
    }

    
    Cluster cluster1,cluster2,cluster3,cluster4,cluster5,cluster6,cluster7,cluster8,cluster9,cluster10;
    cluster1.setDxy(100, 200);
    cluster2.setDxy(200, 200);
    cluster3.setDxy(200, 100);
    cluster4.setDxy(100, 300);
    cluster5.setDxy(300, 200);
    cluster6.setDxy(200, 200);
    cluster7.setDxy(300, 200);
    cluster8.setDxy(200, 100);
    cluster9.setDxy(100, 300);
    cluster10.setDxy(300,200);
 
    
    Merge merge;
    merge.read(cluster1,filename1,init_temp,temp_scale,term_temp,cvg_r,times);
    merge.read(cluster2,filename2,init_temp,temp_scale,term_temp,cvg_r,times);
    if(filename3[0] != 0)
        merge.read(cluster3,filename3,init_temp,temp_scale,term_temp,cvg_r,times);
    if(filename4[0] != 0)
        merge.read(cluster4,filename4,init_temp,temp_scale,term_temp,cvg_r,times);
    if(filename5[0] != 0)
        merge.read(cluster5,filename5,init_temp,temp_scale,term_temp,cvg_r,times);
    if(filename6[0] != 0)
        merge.read(cluster6,filename6,init_temp,temp_scale,term_temp,cvg_r,times);
    if(filename7[0] != 0)
        merge.read(cluster7,filename7,init_temp,temp_scale,term_temp,cvg_r,times);
    if(filename8[0] != 0)
        merge.read(cluster8,filename8,init_temp,temp_scale,term_temp,cvg_r,times);
    if(filename9[0] != 0)
        merge.read(cluster9,filename9,init_temp,temp_scale,term_temp,cvg_r,times);
    if(filename10[0] != 0)
        merge.read(cluster10,filename10,init_temp,temp_scale,term_temp,cvg_r,times);
    
    merge.init_relation();
    CSA cas;
    cas.sa_floorplanning(merge,init_temp,temp_scale,term_temp,cvg_r,times);
    merge.ct_modulesContour();
    
    merge.printRelationship();
    merge.print_clustersHVC();
    merge.print_modulesHVC();
    merge.draw_mergedModules();
   
    return 1;
}