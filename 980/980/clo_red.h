//---------------------------------------------------------------------------
#ifndef clo_redH
#define clo_redH
//---------------------------------------------------------------------------
#define DEBUG_UT 1

#include <list>
#include "hv_matrix.h"
#include "fplan.h"
#include "rbt.h"
//--------------------------------------------------------------------------

class Snode{
public:
    int label;
    int pt;
    
    Snode() {};
    ~Snode() {};
    
    Snode(const Snode& sn){
        label=sn.label;
        pt=sn.pt;
    }
    
    Snode& operator=(const Snode& sn){
        label=sn.label;
        pt=sn.pt;
        return *this;
    }
    
    void assign_label(int l){
        label=l;
    }
    
    friend istream& operator>>(istream& is, Snode& sn){
        is>>sn.label>>sn.pt;
        return is;
    }
    
    friend ostream& operator<<(ostream& os, const Snode& sn){
        os<<"label="<<sn.label<<", pt="<<sn.pt;
        return os;
    }
};


struct Solution{
    Hv_Matrix triangle_matrix;
    Modules_Info modules_info;
    vector<Snode> packing_seq;
    //*********Added by Zhenxin*********
    vector<Module_Info> H_contour;
    vector<Module_Info> V_contour;
    //**********************************
    int Area;
    int Wire;
    int Width;
    int Height;
    float Cost;
};
      
class Clo_Red_Graph: public FloorPlan{
  public:
    Clo_Red_Graph() {};
    ~Clo_Red_Graph() {};
    
    void init_sqr();
    void compute_cost(float, float, int, int);
    void compute_wire_length();
    void contour_packing();
//    int show_graph();
    
    //simulated Annealing
    void perturbate0();
    void perturbate1();
    void perturbate2();
    void perturbate3();
    float get_cost();
    void get_solution(Solution&);
    void recover(Solution &sol);
    
    void list_Graph(Graph& g);
    Hv_Matrix getHv_Matrix(){ return triangle_matrix; };
    void printEdgeType();
    vector<int> getPackingSeq();
    vector<Module_Info> getH_contour() {return H_contour;};
    vector<Module_Info> getV_contour() {return V_contour;};
   
    //debug tools
    void Test();
    void list_information();
    
  private:
    
    //simulated operators
    G_edge choose_edge(Graph g);
    
    /////////////////////add by fuchen///////////////////////////
    int reverse_edge_delete(Graph&, Graph&, char);
    int reverse_edge_keep(Graph&, Graph&, char);
    int move_edge_keep(Graph&, Graph&, char);
    int move_edge_delete(Graph&, Graph&, char);
    int remove_edge(Graph&, Graph&, char);
    /////////////////////////////////////////////////
    
    
    int reverse_edge(Graph&, Graph&, char);
    int move_edge(Graph&, Graph&, char);
    int move_edge_reverse(Graph&, Graph&, char);
    void rotate(int);
    void interchange();
    int Pick_Red_Edge(char, G_edge&, int&, int&, Graph&);
    void Tune_Closure_Graphs(Graph&, Graph&, G_node, G_node, char);
    
    void reconstruct(vector<Snode>&, vector<Snode>&,
                     Graph&,  Graph&,
                     vector<G_node>&, vector<G_node>&,
                     Hv_Matrix&, Hv_Matrix&);
    
    Graph h_clo_G;
    Graph v_clo_G;
    vector<G_node> h_nodes;
    vector<G_node> v_nodes;
    vector<Snode> packing_seq;
    
    //vector<list<Snode>::iterator> packing_litems;
    
    Hv_Matrix triangle_matrix;
    vector<Module_Info> H_contour;
    vector<Module_Info> V_contour;
};
//float rand_01();
//---------------------------------------------------------------------------
#endif
