//
//  sa.h
//  980Merge
//
//  Created by Hensin on 2016-02-23.
//  Copyright (c) 2016 Hensin. All rights reserved.
//

#ifndef ___80Merge__sa__
#define ___80Merge__sa__

#include "clo_red.h"

class MSA{
    public:
        MSA() {
            init_avg=200;
            hill_climb_stage = 4;
            avg_ratio = 200;
        };
        ~MSA() {};
        
        void set_hill_climb_stage(int stage){ hill_climb_stage=stage;};
        void set_avg_ratio(float ratio) {avg_ratio = ratio;};
        int get_hill_climb_stage(){ return hill_climb_stage;};
        float get_avg_ratio(){return avg_ratio;};
    
        double seconds();
        void sa_floorplanning(Clo_Red_Graph& fp,float P,float r,float Term_T,float conv_rate,int k);

    private:
        float init_avg;
        int hill_climb_stage;
        float avg_ratio;
        double mean(vector<double> &chain);
        double std_var(vector<double> &chain);
    
        float random_floorplanning(Clo_Red_Graph& fp, int times);
};

#endif 
