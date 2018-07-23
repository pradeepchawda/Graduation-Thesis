//---------------------------------------------------------------------------
#ifndef hv_matrixH
#define hv_matrixH
//---------------------------------------------------------------------------
#include <vector>
#include <iostream>
#include "foundation.h"
//---------------------------------------------------------------------------
using namespace std;


// 'H' ('V') denote there exists a directed edge 
// from node i to node j in horizontal (vertical) constraint graph
// 'h' ('v') denote there exists a directed edge 
// from node j to node i in horizontal (vertical) constraint graph
enum Edge_Type { H, h, V, v};

struct Element{
  G_edge ed;
  Edge_Type direction;  
};

typedef vector<vector<Element> > Matrix;

class Hv_Matrix{
   public:
     Hv_Matrix();
     Hv_Matrix(const Hv_Matrix&);
     Hv_Matrix& operator=(const Hv_Matrix&);
     ~Hv_Matrix(); 
         
     void init(int);
    
     void update_matrix(int, int, char, Graph&, G_edge&);
     
     void set_edge(int, int, G_edge&);
     void set_edge_type(int, int, Edge_Type);
     void set_edge_type(int, int, char);
     G_edge& get_edge(int, int);
     Edge_Type get_edge_type(int, int);
     char get_edge_type_c(int, int);
     void rev_edge_type(int, int, char);
     int equal_type(int, int, char);     
     void copy_edge_types(Hv_Matrix&);     
     friend ostream& operator<<(ostream&, const Hv_Matrix&); 
   protected:
           
   private:
     Matrix triangle_matrix;
};

//---------------------------------------------------------------------------
#endif
