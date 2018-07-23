//
//  merge.h
//  980Merge
//
//  Created by Hensin on 2016-02-22.
//  Copyright (c) 2016 Hensin. All rights reserved.
//

#include "cluster.h"
#include "foundation.h"

//stores the solution that temporally achieves the minimum area
struct Solution_C{
    Hv_Matrix cltsmatrix;
    vector<Cluster> clts;
    vector<Cluster> packing_seq;
    vector<Cluster> H_ccontour;
    vector<Cluster> V_ccontour;
    int Area;
    int Wire;
    int Width;
    int Height;
    float Cost;
};

class Merge{
public:
    Merge() {};
    ~Merge() {};
    
    void read(Cluster& clt,char* file,float init_temp,float temp_scale,float term_temp,float cvg_r,int times);
    void init_relation();
    void mergeClusters();
    void ct_modulesContour();
    void printRelationship();
    void print_clustersHVC();
    void print_modulesHVC();
    void rotate_test();
    int draw_mergedModules();
    
    Modules_Info getMergedHcontour() {return modulesHcontour;};
    Modules_Info getMergedVcontour() {return modulesVcontour;};
    vector<Cluster> getMergedclusters() {return clusters;};
    Hv_Matrix getClustersmatrix() {return clustersmatrix;};
    
    int size() {return clusters.size();};
    int getArea() {return Area;};
    int getWidth() {return Width;};
    int getHeight() {return Height;};
    double getCost() {return Cost;};
    int getTotalArea() {return TotalArea;};
    
    //simulated Annealing
    void perturbate0();
    void perturbate1();
    void perturbate2();
    void perturbate3();
    void get_solution(Solution_C&);
    void recover(Solution_C& sol);
    
    
private:    
    int Area, Width, Height, Wire, TotalArea;
    double Cost;
    
    Hv_Matrix clustersmatrix;             //stores the geometric relationship between clusters
    vector<Cluster> clusters;             //stores the information of clusters
    vector<Cluster> clustersHcontour;     //stores the horizontal contour in terms of cluster
    vector<Cluster> clustersVcontour;     //stores the vertical contour in terms of cluster
    Modules_Info modulesHcontour;         //stores the horizontal contour in terms of module
    Modules_Info modulesVcontour;         //stores the vertical contour in terms of module
    
    void update();
    void update_clustersHcontour(vector<Cluster> H_contour);
    void update_clustersVcontour(vector<Cluster> V_contour);
    void addDummyMd();
    void removeDummyMd();
    
    Graph h_clo_G;                        //the graph which represents Ch for clusters
    Graph v_clo_G;                        //the graph which represents Cv for clusters
    vector<G_node> h_nodes;               //stores the information of nodes in graph Ch
    vector<G_node> v_nodes;               //stores the information of nodes in graph Cv
    vector<Cluster> packing_seq;          //stores the packing sequence for clusters
    
    //Perturbation operations
    G_edge C_choose_edge(Graph g);
    void C_list_Graph(Graph& g);
    int reverse_edge(Graph&, Graph&, char);
    int move_edge(Graph&, Graph&, char);
    int move_edge_reverse(Graph&, Graph&, char);
    void rotate();
    void interchange();
    //Additional operations
    int Pick_Red_Edge(char, G_edge&, int&, int&, Graph&);
    void Tune_Closure_Graphs(Graph&, Graph&, G_node, G_node, char);
    void reconstruct(vector<Cluster>&, vector<Cluster>&,
                     Graph&,  Graph&,
                     vector<G_node>&, vector<G_node>&,
                     Hv_Matrix&, Hv_Matrix&);
};
