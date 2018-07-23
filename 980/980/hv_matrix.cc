#include "hv_matrix.h"
//--------------------------------------------------------
void error_message(char* m)
{
   cout<<m<<endl;
   exit(0);
}

//----------------------------------------------------------------

Hv_Matrix::Hv_Matrix(const Hv_Matrix& m)
{
   triangle_matrix=m.triangle_matrix;
}

Hv_Matrix& Hv_Matrix::operator=(const Hv_Matrix& m)
{
   triangle_matrix=m.triangle_matrix;

   return *this;
}

G_edge& Hv_Matrix::get_edge(int i, int j)
{
   if(i < j)
      return triangle_matrix[i][j-i-1].ed;
   else if(j < i)
      return triangle_matrix[j][i-j-1].ed;       
   else
   {
      cout<<"Error!!!get_edge in hv_matrix.cc"<<endl;
      exit(0);
   }
}

Edge_Type Hv_Matrix::get_edge_type(int i, int j) 
{
   if(i<j)
      return triangle_matrix[i][j-i-1].direction;
   else
      return triangle_matrix[j][i-j-1].direction;       
}

char Hv_Matrix::get_edge_type_c(int i, int j) 
{
   if(i<j){
       if(triangle_matrix[i][j-i-1].direction==H||
          triangle_matrix[i][j-i-1].direction==h)
          return 'H';
       else
          return 'V';
   }else{
       if(triangle_matrix[j][i-j-1].direction==H||
          triangle_matrix[j][i-j-1].direction==h)
          return 'H';
       else
         return 'V'; 
   }      
}

void Hv_Matrix::rev_edge_type(int i, int j, char d)
{
   if(d=='H')
   {
      if(i<j)
        triangle_matrix[i-1][j-i-1].direction=H;
      else
        triangle_matrix[j-1][i-j-1].direction=h;
   }
   else
   {
      if(i<j)
        triangle_matrix[i-1][j-i-1].direction=V;
      else
        triangle_matrix[j-1][i-j-1].direction=v;
   }
}

void Hv_Matrix::update_matrix(int i, int j, char x, 
                              Graph& G, G_edge& e)
{
   G_edge old_e;
   if(i < j)
   { 
      if(x=='H')
      {   
          if(triangle_matrix[i][j-i-1].direction!=H){
            //delete old edge in the graph
            old_e=triangle_matrix[i][j-i-1].ed;
            remove_edge(old_e, G);
 
//            update matrix
            triangle_matrix[i][j-i-1].direction=H;
            triangle_matrix[i][j-i-1].ed=e;
          }
          else{
              cout<<"update matrix error of h edge <"<<i<<","
              <<j<<">"<<endl;
            exit(0);
          }
      }
      else if(x=='V')
      {   
          if(triangle_matrix[i][j-i-1].direction!=V)
          {
            //delete old edge in the graph
            old_e=triangle_matrix[i][j-i-1].ed;
            remove_edge(old_e, G);
//            G.del_edge(old_e);
 
            //update matrix
            triangle_matrix[i][j-i-1].direction=V;
            triangle_matrix[i][j-i-1].ed=e;
          }
          else{
            cout<<"update matrix error of v edge <"<<i<<"," 
                <<j<<">"<<endl;
            exit(0);
          }
      }
      else
        // error_message("update matrix error (not H or V)!!!");        
	cout<<"update matrix error (not H orV)!!!"<<endl;  
 }
   else
   {
      if(x=='H')
      {
          if(triangle_matrix[j][i-j-1].direction!=h)
          {
            //delete old edge in the graph
            old_e=triangle_matrix[j][i-j-1].ed;
            remove_edge(old_e, G);
//            G.del_edge(old_e);
 
            //update matrix
            triangle_matrix[j][i-j-1].ed=e;
            triangle_matrix[j][i-j-1].direction=h;
          }
          else{
            cout<<"update matrix error of h edge <"<<j<<"," 
                <<i<<">"<<endl;
            exit(0);
          }
      }
      else if(x=='V')
      {
          if(triangle_matrix[j][i-j-1].direction!=v)
          {
            //delete old edge in the graph
            old_e=triangle_matrix[j][i-j-1].ed;
            remove_edge(old_e, G);
//            G.del_edge(old_e);
 
            //update matrix  
            triangle_matrix[j][i-j-1].direction=v; 
            triangle_matrix[j][i-j-1].ed=e;
          }
          else{
            cout<<"update matrix error of v edge <"<<j<<"," 
                <<i<<">"<<endl;
            exit(0);
          }
      }
      else
        // error_message("update matrix error (not H or V)!!!");        
	    cout<<"update matrix error of v edge <"<<endl;
   }
}

void Hv_Matrix::set_edge(int i, int j, G_edge& e)
{
   if(i < j) {
     triangle_matrix[i][j-i-1].ed= e;
   } 
   else if(j < i)
     triangle_matrix[j][i-j-1].ed= e;
   else
   {
      cout<<"Error!!!set_edge in hv_matrix.cc."<<endl;
      exit(0);
   }  
}

void Hv_Matrix::set_edge_type(int i, int j, Edge_Type et)
{
    if(i < j)
      triangle_matrix[i][j-i-1].direction= et;
    else if(j < i)
      triangle_matrix[j][i-j-1].direction= et;
    else 
    {
      cout<<"Error!!!set_edge_type in hv_matrix.cc"<<endl;
      exit(0);
    }
}

void Hv_Matrix::set_edge_type(int i, int j, char d)
{
    if(i < j)
    {
      if(d=='H')
        triangle_matrix[i][j-i-1].direction=H;
      else
        triangle_matrix[i][j-i-1].direction=V;
    }
    else if(j < i)
    {
      if(d=='H')
        triangle_matrix[j][i-j-1].direction=h;
      else
        triangle_matrix[j][i-j-1].direction=v;
    }
    else 
    {
      cout<<"Error!!!set_edge_type in hv_matrix.cc"<<endl;
      exit(0);
    }
}

void Hv_Matrix::init(int module_size)
{
   int i;
  
   triangle_matrix.resize(module_size);

   for(i=0; i<module_size; i++)
           triangle_matrix[i].resize(module_size-i-1);
}

int Hv_Matrix::equal_type(int source_label, int target_label, char d)
{
 
   if(d=='H'){
     if(source_label<target_label){
       if(triangle_matrix[source_label][target_label-source_label-1].direction
          ==H){
         //cout<<"direction="
         // <<triangle_matrix[source_label][target_label-source_label-1].direction
         // <<endl;
        return 1;
       }else
        return 0; 
     }else{
       if(triangle_matrix[target_label][source_label-target_label-1].direction
         ==h){
         //cout<<"direction="
         // <<triangle_matrix[target_label][source_label-target_label-1].direction
         // <<endl;
        return 1;
       }else
        return 0; 
     }
   }else{
     if(source_label<target_label){
       if(triangle_matrix[source_label][target_label-source_label-1].direction
          ==V){
         //cout<<"direction="
         // <<triangle_matrix[source_label][target_label-source_label-1].direction
         // <<endl;
        return 1;
       }else
        return 0; 
     }else{
       if(triangle_matrix[target_label][source_label-target_label-1].direction
         ==v){
         //cout<<"direction="
         // <<triangle_matrix[target_label][source_label-target_label-1].direction
         // <<endl;
        return 1;
       }else
        return 0; 
     }
   }
}

Hv_Matrix::Hv_Matrix()
{
}

Hv_Matrix::~Hv_Matrix()
{
   triangle_matrix.clear();
}

ostream& operator<<(ostream& os, const Hv_Matrix& m)
{
  int i, j;
  int size=m.triangle_matrix.size();

  for(i=0; i< size; i++)
  {
     for(j=i+1; j < size; j++)
       os << "<" << i << ", " << j << ">="
          << m.triangle_matrix[i][j-i-1].direction<<endl; 
  }  
  return os;
}

void Hv_Matrix::copy_edge_types(Hv_Matrix& m){
   int size=triangle_matrix.size();
   int i, j;
   m.triangle_matrix.resize(size);

   for(i=0; i<size; i++)
      m.triangle_matrix[i].resize(size-i-1);


   for(i=0; i< size; i++)
     for(j=i+1; j < size; j++)
        m.triangle_matrix[i][j-i-1].direction=
          triangle_matrix[i][j-i-1].direction;
}
