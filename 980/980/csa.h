//
//  csa.h
//  980MergeSA
//
//  Created by Hensin on 2016-03-15.
//  Copyright (c) 2016 Hensin. All rights reserved.
//

#ifndef ___80MergeSA__csa__
#define ___80MergeSA__csa__

#include "merge.h"

class CSA{
public:
    CSA() {
        init_avg=200;
        hill_climb_stage = 4;
        avg_ratio = 200;
    };
    ~CSA() {};
    
    void set_hill_climb_stage(int stage){ hill_climb_stage=stage;};
    void set_avg_ratio(float ratio) {avg_ratio = ratio;};
    int get_hill_climb_stage(){ return hill_climb_stage;};
    float get_avg_ratio(){return avg_ratio;};
    
    double seconds();
    void sa_floorplanning(Merge& merge,float P,float r,float Term_T,float conv_rate,int k);
    
private:
    float init_avg;
    int hill_climb_stage;
    float avg_ratio;
    double mean(vector<double> &chain);
    double std_var(vector<double> &chain);
    
    float random_floorplanning(Merge& merge, int times);
};

#endif /* defined(___80MergeSA__csa__) */
