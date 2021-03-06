//
//  sa.cc
//  980Merge
//
//  Created by Hensin on 2016-02-23.
//  Copyright (c) 2016 Hensin. All rights reserved.
//

#include "msa.h"


double MSA::mean(vector<double> &chain){
    double sum=0;
    for(int i=0; i < chain.size();i++)
        sum+= chain[i];
    return sum/chain.size();
}

double MSA::std_var(vector<double> &chain){
    double m = mean(chain);
    double sum=0;
    double var;
    int N= chain.size();
    
    for(int i=0; i < N;i++)
        sum += (chain[i]-m)*(chain[i]-m);
    
    var = sqrt(sum/(N-1));
    
    return var;
}

/*
 Simulated Annealing B*Tree Floorplanning
 P: probability to accept in avg. delta cost (0~1)
 r: temperature decreasing rate (0~1)
 t: terminating temperature
 conv_rate: converge condition which is maximal fail rate (0~1)
 k: factor of the number of permutation in one temperature
 */
void MSA::sa_floorplanning(Clo_Red_Graph& fp, float P,float r,float Term_T, float conv_rate,int k)
{
    Solution Best,E;
    int MT,uphill,reject;
    float pre_cost,cost,d_cost,reject_rate;
    int N = k * fp.size();
    float T, Actual_T=1;
    float avg=init_avg;
    float max_reject_rate = 1;
    ofstream fs("/tmp/debug");
    double rf= random_floorplanning(fp,k);
    double estimate_avg =  rf / avg_ratio;
    //cout << "Random Average Delta Cost = " << rf << endl;
    //cout << "Estimate Average Delta Cost = " << estimate_avg << endl;
    
    if(hill_climb_stage==0)
        avg = estimate_avg;
    
    T = avg / log(P);
    
    // get inital solution
    fp.get_solution(Best);
    pre_cost = Best.Cost;
    E = Best;
    
    fs.precision(4);
    
    int good_num=0,bad_num=0;
    float total_cost=0;
    int count=0;
    
    do{
        count++;
        MT=uphill=reject=0;
        total_cost = 0;
        
        fs   << "Iteration " << count << ", T=" << Actual_T << endl;
        cout << "Iteration " << count << ", T=" << Actual_T << endl;
        
        vector<double> chain;
        for(; uphill < N && MT < 2*N; MT++){
            
            if(Actual_T==1)
                fp.perturbate1();
            else if(Actual_T > 0.8)
                fp.perturbate2();
            else
                fp.perturbate3();
            
            //fp.Test();
            fp.contour_packing();
            //fp.compute_wire_length();
            //fp.compute_cost(alpha, beta, Best);
            cost = fp.get_cost();
            d_cost = cost - pre_cost;
            float p = exp(d_cost/T);
            
            chain.push_back(cost);
            
            if(d_cost <=0 || rand_01() < p ){
                pre_cost = cost;
                total_cost += d_cost;
                
                if(d_cost != 0)
                    fs << "    total= " << total_cost << endl;
                
                if(d_cost > 0){
                    uphill++;
                    fs << " bad= " << d_cost << ": \t\t" << p <<  endl;
                    bad_num++;
                }
                
                if(d_cost < 0){
                    fs << " good= " << d_cost << endl;
                    good_num++;
                }
                fp.get_solution(E);
                
                // keep best solution
                if(cost < Best.Cost){
                    fp.get_solution(Best);
                    cout << "   ==>  Cost= " <<  Best.Cost << endl;
                    assert(Best.Cost >= fp.getTotalArea());
                }
            }
            else{
                reject++;
                fp.recover(E);
            }
        }//for_MT
        T = r*T;
        //cout<<"decreasing T="<<T<<endl;
        //double sv = std_var(chain);
        //float r_t = exp(lamda*T/sv);
        //T = r_t*T;
        
        // After Hill-Climbing, start to use normal SA
        if(count == hill_climb_stage){
            T = estimate_avg/log(P);
            Actual_T = exp(estimate_avg/T);
        }
        if(count > hill_climb_stage){
            Actual_T = exp(estimate_avg/T);
            max_reject_rate = conv_rate;
        }
        
        reject_rate = float(reject)/MT;
        
        fs    << " reject=" << reject_rate << endl;
    }while(reject_rate < max_reject_rate && Actual_T > Term_T);
    
    if(reject_rate >= conv_rate)
        cout << "Convergent!\n";
    
    if(Actual_T < Term_T)
        cout << "Cooling Enough!\n";
    
    fp.recover(Best);
}


float MSA::random_floorplanning(Clo_Red_Graph& fp, int times){
    Solution Best;
    fp.contour_packing();
    Best.Area=fp.getArea();
    //show_graph(fp.getWidth(), fp.getHeight(), fp.modules_info);
    fp.get_solution(Best);
    double total_cost=0, pre_cost, cost;
    int t = 0;
    int N = times;
    pre_cost = (double)fp.get_cost();
    
    for(int i=0; i < N; i++){
        fp.perturbate1();
        //fp.Test();
        fp.contour_packing();
        cost = fp.get_cost();
        if(cost-pre_cost > 0){
            total_cost += (cost-pre_cost);
            t++;
            pre_cost = cost;
        }
        
        if(cost <= Best.Cost){
            fp.get_solution(Best);
        }
    }
    fp.recover(Best);
    return (total_cost/t);
}


double MSA::seconds()
{
#ifdef BSD       // BSD system instead of SYSV
    rusage time;
    getrusage(RUSAGE_SELF,&time);
    return (double)(1.0*time.ru_utime.tv_sec+0.000001*time.ru_utime.tv_usec);
#else
    return double(clock())/CLOCKS_PER_SEC;
#endif
}
