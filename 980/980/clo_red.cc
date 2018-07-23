#include <math.h>
#include <time.h>
#include "clo_red.h"


//-----------------------------------------------------------------
// Initialization CGS
//-----------------------------------------------------------------
void Clo_Red_Graph::init_sqr()
{
    int i, j;
    VertexProperty vp;
//    EdgeProperty ep;
    G_node n;
    G_edge e;
    triangle_matrix.init(modules_N);
    Snode sn;
 
    for(i=0; i <modules_N; i++){
        vp.index = i;
        
      //construct nodes for HCG
        n = add_vertex(vp, h_clo_G);
        h_nodes.push_back(n);

      //construct nodes for VCG
        n = add_vertex(vp, v_clo_G);
        v_nodes.push_back(n);

      //for packing sequence
        sn.label=i;
        sn.pt=i;
        packing_seq.push_back(sn);
    }
   
    int l=1;
    int len=(int)ceil(sqrt(modules_N));
    bool inserted;
    for(i=0; i <modules_N; i++){
        l=(i%len)+1;
        for(j=i+1; j< modules_N; j++){
            if(l<len){
                boost::tie(e, inserted) = add_edge(h_nodes[i], h_nodes[j], h_clo_G);
                
               // h_clo_G[e].flag = (i+1)*j;
                
                triangle_matrix.set_edge(i, j, e);
                triangle_matrix.set_edge_type(i, j, H);
            }else{
                boost::tie(e, inserted) = add_edge(v_nodes[i], v_nodes[j], v_clo_G);
                
                //v_clo_G[e].flag = (i+1)*j;
              
                triangle_matrix.set_edge(i, j, e);
                triangle_matrix.set_edge_type(i, j, V);
            }
            l++;
       }
    }
}


//------------------------------------------------------------------------
//Print the edge types between two modules
//------------------------------------------------------------------------
void Clo_Red_Graph::printEdgeType(){
    int i, j;
    for(i=0; i <modules_N; i++){
        for(j=i+1; j< modules_N; j++){
            cout<<triangle_matrix.get_edge_type_c(i, j)<<" ";
        }
        cout<<endl;
    }
}


//------------------------------------------------------------------------
//Get the packing sequence
//------------------------------------------------------------------------
vector<int> Clo_Red_Graph::getPackingSeq(){
    vector<int> packingSeq;
    for(int i=0; i<packing_seq.size(); i++){
        int sqe = packing_seq[i].label;
        packingSeq.push_back(sqe);
    }
    return packingSeq;
}


//-----------------------------------------------------------------------
//Implementing contour packing
//-----------------------------------------------------------------------
void Clo_Red_Graph::contour_packing(){
    
    RBT Ts, Tv;
    Snode sn;
    
    Ts.insertHead('s');
    Tv.insertHead('t');

    //Construct the red-black tree according to the packing sequence
    for(int i=0; i<packing_seq.size(); i++){
        sn = packing_seq[i];
        Ts.insert_(&(modules_info[sn.label]), &triangle_matrix);
        Tv.insert_(&(modules_info[sn.label]), &triangle_matrix);
    }

    //Get the horizontal contour in terms of module
    Ts.inorder();
    Modules_Info Hcontour = Ts.getContour();
    Modules_Info hc;
    for(int i=0; i<Hcontour.size(); i++){
        for(int j=0; j<modules_info.size(); j++){
            if(Hcontour[i].name==modules_info[j].name){
                hc.push_back(modules_info[j]);
            }
        }
    }
    H_contour = hc;
    
    //Get the vertical contour in terms of module
    Tv.inorder();
    Modules_Info Vcontour = Tv.getContour();
    Modules_Info vc;
    for(int i=0; i<Vcontour.size(); i++){
        for(int j=0; j<modules_info.size(); j++){
            if(Vcontour[i].name==modules_info[j].name){
                vc.push_back(modules_info[j]);
            }
        }
    }
    V_contour = vc;
    
    //Get the width, height, area after packing
    Width = H_contour.back().rx;
    Height = V_contour.back().ry;
    Area=Width*Height;
    Cost=Area;
}


//int Clo_Red_Graph::show_graph(){
//    My_window win(Point(150,0),1000,700,modules_info,"The Result of Packing Modules");
//    return gui_main();
//}


void Clo_Red_Graph::compute_wire_length(){
    int length=0;
    
    // int num_of_pins=0;
    // compute absolute position
    
    int mx, my, width, height;
    for(int i=0; i < modules_N; i++){
        mx= modules_info[i].x;
        my= modules_info[i].y;
        width=modules[i].width;
        height=modules[i].height;
        
        for(int j=0; j < modules[i].pins.size(); j++){
            Pin &p = modules[i].pins[j];
            int p_x=0, p_y=0;
            
            switch(modules_info[i].degree){
                case 0:{
                    p_x=p.x;
                    p_y=p.y;
                    break;
                }
                case 1:{
                    p_x=p.y;
                    p_y=width-p.x;
                    break;
                }
                case 2:{
                    p_x=width-p.x;
                    p_y=height-p.y;
                    break;
                }
                case 3:{
                    p_x=height-p.y;
                    p_y=p.x;
                    break;
                }
            }
            
            p.ax=p_x+mx;
            p.ay=p_y+my;
        }
    }
    
    float x_ratio=(float)Width/modules[modules_N].width;
    float y_ratio=(float)Height/modules[modules_N].height;
    for(int j=0; j < modules[modules_N].pins.size(); j++){
        Pin &p = modules[modules_N].pins[j];
        p.ax=int(p.x*x_ratio);
        p.ay=int(p.y*y_ratio);
    }
    
    for(int i=0; i < network.size(); i++){
        assert(network[i].size() > 0);
        int max_x= INT_MIN, max_y= INT_MIN;
        int min_x= INT_MAX, min_y= INT_MAX;
        
        for(int j=0; j < network[i].size(); j++){
            Pin& p = *network[i][j];
            max_x= max(max_x, p.ax);
            max_y= max(max_y, p.ay);
            min_x= min(min_x, p.ax);
            min_y= min(min_y, p.ay);
        }
        length += (max_x-min_x)+(max_y-min_y);
    }
    Wire=length;
    Cost=Wire;
}


void Clo_Red_Graph::compute_cost(float alpha, float beta,
                                 int A, int W){
    
    if(alpha!=0.0&&beta!=0.0)
        Cost=alpha*(double)Area/A+beta*(double)Wire/W;
    else if(alpha==0.0&&beta!=0.0)
        Cost=Wire;
    else if(alpha!=0.0&&beta==0.0)
        Cost=Area;
}


//---------------------------------------------------------------------------
//   Simulated Annealing Permutation Operations
//---------------------------------------------------------------------------

//***************************************************************
// perturbation
//***************************************************************
void Clo_Red_Graph::perturbate1()
{
    int success;
    float rotate_rate=0.7;
    
    do{
        int c;
        success=1;
        if(rotate_rate> rand_01()){
            c=rand()%3;
            switch(c){
                case 0:{   //move edges from h closure graph to v closure graph
                    int c2=rand()%2;
                    if(c2){
                        //cout<<"move edges from h graph to v graph"<<endl;
                        success=move_edge(h_clo_G, v_clo_G, 'V');
                        break;
                    }else{
                        //cout<<"move edges from v graph to h graph"<<endl;
                        success=move_edge(v_clo_G, h_clo_G, 'H');
                        break;
                    }
                    
                        }
                case 1:{   // reverse direction of an edge in h closure graph
                    int c3=rand()%2;
                    if(c3){
                        //cout<<"reverse edge in h graph"<<endl;
                        success=reverse_edge(h_clo_G, v_clo_G, 'H');
                        break;
                    }
                    else{   // reverse direction of an edge in v closure graph
                        //cout<<"reverse edge in v graph"<<endl;
                        success=reverse_edge(v_clo_G, h_clo_G, 'V');
                        break;
                    }}
                case 2:{   //move edges from h closure graph to v closure graph
                    int c4=rand()%2;
                    if(c4){
                        //cout<<"move edges from h graph to v graph and reverse"<<endl;
                        success=move_edge_reverse(h_clo_G, v_clo_G, 'V');
                        break;
                    }else{
                        //cout<<"move edges from v graph to h graph and reverse"<<endl;
                        success=move_edge_reverse(v_clo_G, h_clo_G, 'H');
                        break;
                    }}
            }
        }else{
            c=rand()%2;
            switch (c) {
                case 0:
                    //cout<<"interchange two nodes"<<endl;
                    //interchange();
                    break;
                case 1:{
                    int c1=rand()%2;
                    rotate(c1);
                    break;
                }
            }
        }
    }while(!success);
}


void Clo_Red_Graph::perturbate2()
{
    int success;
    do{
        int c=rand()%5;
        success=1;
        switch (c) {
            case 0:{
                int c1=rand()%2;
                rotate(c1);
                break;
            }
            case 1:
                //cout<<"interchange two nodes"<<endl;
                //interchange();
                break;
            case 2:{   //move edges from h closure graph to v closure graph
                int c2=rand()%2;
                if(c2){
                    //cout<<"move edges from h graph to v graph"<<endl;
                    success=move_edge(h_clo_G, v_clo_G, 'V');
                    break;
                }else{
                    //cout<<"move edges from v graph to h graph"<<endl;
                    success=move_edge(v_clo_G, h_clo_G, 'H');
                    break;
                }}
            case 3:{   // reverse direction of an edge in h closure graph
                int c3=rand()%2;
                if(c3){
                    //cout<<"reverse edge in h graph"<<endl;
                    success=reverse_edge(h_clo_G, v_clo_G, 'H');
                    break;
                }
                else{   // reverse direction of an edge in v closure graph
                    //cout<<"reverse edge in v graph"<<endl;
                    success=reverse_edge(v_clo_G, h_clo_G, 'V');
                    break;
                }}
            case 4:{   //move edges from h closure graph to v closure graph
                int c4=rand()%2;
                if(c4){
                    //cout<<"move edges from h graph to v graph and reverse"<<endl;
                    success=move_edge_reverse(h_clo_G, v_clo_G, 'V');
                    break;
                }else{
                    //cout<<"move edges from v graph to h graph and reverse"<<endl;
                    success=move_edge_reverse(v_clo_G, h_clo_G, 'H');
                    break;
                }}
        }
    }while(!success);
}


void Clo_Red_Graph::perturbate3()
{
    int success;
    float rotate_rate=0.3;
    
    do{
        int c;
        success=1;
        if(rotate_rate> rand_01()){
            c=rand()%3;
            switch(c){
                case 0:{   //move edges from h closure graph to v closure graph
                    int c2=rand()%2;
                    if(c2){
                        //cout<<"move edges from h graph to v graph"<<endl;
                        success=move_edge(h_clo_G, v_clo_G, 'V');
                        break;
                    }else{
                        //cout<<"move edges from v graph to h graph"<<endl;
                        success=move_edge(v_clo_G, h_clo_G, 'H');
                        break;
                    }}
                case 1:{   // reverse direction of an edge in h closure graph
                    int c3=rand()%2;
                    if(c3){
                        //cout<<"reverse edge in h graph"<<endl;
                        success=reverse_edge(h_clo_G, v_clo_G, 'H');
                        break;
                    }
                    else{   // reverse direction of an edge in v closure graph
                        //cout<<"reverse edge in v graph"<<endl;
                        success=reverse_edge(v_clo_G, h_clo_G, 'V');
                        break;
                    }}
                case 2:{   //move edges from h closure graph to v closure graph
                    int c4=rand()%2;
                    if(c4){
                        //cout<<"move edges from h graph to v graph and reverse"<<endl;
                        success=move_edge_reverse(h_clo_G, v_clo_G, 'V');
                        break;
                    }else{
                        //cout<<"move edges from v graph to h graph and reverse"<<endl;
                        success=move_edge_reverse(v_clo_G, h_clo_G, 'H');
                        break;
                    }}
            }
        }else{
            c=rand()%2;
            switch (c) {
                case 0:
                    //cout<<"interchange two nodes"<<endl;
                    //interchange();
                    break;
                case 1:{
                    int c1=rand()%2;
                    rotate(c1);
                    break;
                }
            }
        }
    }while(!success);
}

//-----------------------------------------------------------------
// Get a random edge of Graph g
//-----------------------------------------------------------------
G_edge Clo_Red_Graph::choose_edge(Graph g){
    
    int n, c;
    n = int(num_edges(g));
    c = rand()%n;
    typename graph_traits<Graph>::edge_iterator e_i, e_end;
    boost::tie(e_i, e_end)= edges(g);
    for(int i=0; i<c; i++){
        e_i++;
    }
    
    return *e_i;
};

//------------------------------------------------------------------
// List all the edges and vertices in a graph
//------------------------------------------------------------------
void Clo_Red_Graph::list_Graph(Graph& g){
    typedef graph_traits<Graph>::vertex_iterator vertex_iter;
    std::pair<vertex_iter, vertex_iter> vp;
    for( vp=vertices(g); vp.first != vp.second; ++vp.first){
        
        int in_edges_num = int(in_degree(*vp.first, g));
        if(in_edges_num != 0){
            cout<<"in_edges for "<< g[*vp.first].index<<": ";
            typename graph_traits<Graph>::in_edge_iterator in_i, in_end;
            for(boost::tie(in_i, in_end)=in_edges(*vp.first, g); in_i != in_end; ++in_i){
                cout<<*in_i<<" ";
            }
            cout<<endl;
        }else{
            cout<<"in_edges for "<< g[*vp.first].index<<": Null"<<endl;
        }
        
        int out_edges_num = int(out_degree(*vp.first, g));
        if(out_edges_num != 0){
            cout<<"out_edges for "<< g[*vp.first].index<<": ";
            typename graph_traits<Graph>::out_edge_iterator out_i, out_end;
            for(boost::tie(out_i, out_end)=out_edges(*vp.first, g); out_i != out_end; ++out_i){
                cout<<*out_i<<" ";
            }
            cout<<endl;
        }else{
            cout<<"out_edges for "<< g[*vp.first].index<<": Null"<<endl;
        }
    }
}


//***************************************************************
// Pick_Red_Edge is used to pick a reduction edge
//***************************************************************
int Clo_Red_Graph::Pick_Red_Edge(char c, G_edge& pick_e,
                                 int& s_label, int& t_label,
                                 Graph& G)
{
    if(num_edges(G)!=0){
        G_edge e = choose_edge(G);
        int index= G[source(e,G)].index;
        Snode init_sn, sn;
        char cur_c;
        int k=0;
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label==index){
                k = i;
                init_sn = packing_seq[i];
            }
        }
        vector<Snode> modules1, modules2;
        
        k++;
        for(int i=k; i<packing_seq.size(); i++){
            sn=packing_seq[i];
            cur_c=triangle_matrix.get_edge_type_c(init_sn.pt, sn.pt);
            
            //the edge (init_module, module) appears in the same graph
            if(c==cur_c){
                modules1.push_back(sn);
            }
        }

        Snode first_sn, s_sn;
        s_sn=modules1.front();
        for(int j=1; j<modules1.size(); j++){
            modules2.push_back(modules1[j]);
        }
        
        first_sn=s_sn;
        int n = int(modules2.size());
        int l=0;
        for(int i=0; i<n; i++){
            sn = modules2[l];
            cur_c=triangle_matrix.get_edge_type_c(s_sn.pt, sn.pt);
            
            //the edge (s_module, module) appears in the same graph
            if(cur_c==c){
                vector<Snode>::iterator it = modules2.begin() + l;
                modules2.erase(it);
            }else{
                s_sn = sn;
                l++;
            }
        }
        
        modules2.push_back(first_sn);

        int size=int(modules2.size());
        if(size>=1){
            index=rand()%size;
            sn=modules2[index];
            s_label=init_sn.label;
            t_label=sn.label;
            pick_e=triangle_matrix.get_edge(init_sn.pt, sn.pt);
            return 1;
        }else
            return 0;
    }else
        return 0;
}


//***************************************************************
//Tuning graphs while add edges
//***************************************************************
void Clo_Red_Graph::Tune_Closure_Graphs(Graph& CG_to,
                                        Graph& CG_from,
                                        G_node edge_source_node,
                                        G_node edge_target_node, char d)
{
    int edge_source_label=CG_to[edge_source_node].index;
    int edge_target_label=CG_to[edge_target_node].index;

    //incremental update edges
    G_edge se, te, e;
    G_node sn, tn;
    vector<G_edge> ses, tes;
    int s_label, t_label;
    int s_pt=0, t_pt=0;
    
    int m=0, n=0;
    for(int i=0; i<packing_seq.size(); i++){
        if(packing_seq[i].label == edge_source_label)
            m = i;
        if(packing_seq[i].label == edge_target_label)
            n = i;
    }
    t_pt=packing_seq[n].pt;
    int in_edges_num = int(in_degree(edge_source_node, CG_to));
    if(in_edges_num != 0){
        typename graph_traits<Graph>::in_edge_iterator in_i, in_end;
        for(boost::tie(in_i, in_end)=in_edges(edge_source_node, CG_to); in_i != in_end; ++in_i){
            se = *in_i;
            sn = source(se, CG_to);
            s_label = CG_to[sn].index;
            for(int i=0; i<packing_seq.size(); i++){
                if(packing_seq[i].label == s_label)
                    s_pt=packing_seq[i].pt;
            }
            
            if(triangle_matrix.get_edge_type_c(s_pt, t_pt)!=d){
                bool inserted;
                boost::tie(e, inserted) = add_edge(sn, edge_target_node, CG_to);
                triangle_matrix.update_matrix(s_pt, t_pt, d, CG_from, e);
            }
        }
    }

    s_pt=packing_seq[m].pt;
    int out_edges_num = int(out_degree(edge_target_node, CG_to));
    if(out_edges_num != 0){
        typename graph_traits<Graph>::out_edge_iterator out_i, out_end;
        for(boost::tie(out_i, out_end)=out_edges(edge_target_node, CG_to); out_i != out_end; ++out_i){
            te = *out_i;
            tn = target(te, CG_to);
            t_label = CG_to[tn].index;
            
            for(int i=0; i<packing_seq.size(); i++){
                if(packing_seq[i].label == t_label)
                    t_pt=packing_seq[i].pt;
            }
            
            if(triangle_matrix.get_edge_type_c(s_pt, t_pt)!=d){
                bool inserted;
                boost::tie(e, inserted) = add_edge(edge_source_node, tn, CG_to);
                triangle_matrix.update_matrix(s_pt, t_pt, d, CG_from, e);

            }
        }
    }

    in_edges_num = int(in_degree(edge_source_node, CG_to));
    if(in_edges_num != 0){
        typename graph_traits<Graph>::in_edge_iterator in_i, in_end;
        for(boost::tie(in_i, in_end)=in_edges(edge_source_node, CG_to); in_i != in_end; ++in_i){
            se = *in_i;
            sn = source(se, CG_to);
            s_label = CG_to[sn].index;
            
            for(int i=0; i<packing_seq.size(); i++){
                if(packing_seq[i].label == s_label)
                    s_pt=packing_seq[i].pt;
            }
        
            out_edges_num = int(out_degree(edge_target_node, CG_to));
            if(out_edges_num != 0){
                typename graph_traits<Graph>::out_edge_iterator out_i, out_end;
                for(boost::tie(out_i, out_end)=out_edges(edge_target_node, CG_to); out_i != out_end; ++out_i){
                    te = *out_i;
                    tn = target(te, CG_to);
                    t_label = CG_to[tn].index;
                    
                    for(int i=0; i<packing_seq.size(); i++){
                        if(packing_seq[i].label == t_label)
                            t_pt=packing_seq[i].pt;
                    }
                    
                    if(triangle_matrix.get_edge_type_c(s_pt, t_pt)!=d){
                        bool inserted;
                        boost::tie(e, inserted) = add_edge(sn, tn, CG_to);
                        triangle_matrix.update_matrix(s_pt, t_pt, d, CG_from, e);
                    }
                }
            }        
        }
    }
}


//***************************************************************
////operator 1::move an edge from one graph to another
//***************************************************************
int Clo_Red_Graph::move_edge(Graph& CG_from, Graph& CG_to, char d)
{
    G_edge move_edge, e;
    G_node source_node=0, target_node=0;
    int source_label, target_label;
    int success=1;
    
    if(d=='H')
    {
        success=Pick_Red_Edge('V', move_edge, source_label, target_label, CG_from);
    }
    else{
        success=Pick_Red_Edge('H', move_edge, source_label, target_label, CG_from);
    }
    
    if(success){
        if(d=='H'){
            for(int i=0; i<h_nodes.size(); i++){
                if(h_clo_G[h_nodes[i]].index == source_label)
                    source_node = h_nodes[i];
                if(h_clo_G[h_nodes[i]].index == target_label)
                    target_node = h_nodes[i];
            }
//            source_node=h_nodes[source_label];
//            target_node=h_nodes[target_label];
        }else{
            for(int i=0; i<v_nodes.size(); i++){
                if(v_clo_G[v_nodes[i]].index == source_label)
                    source_node = v_nodes[i];
                if(v_clo_G[v_nodes[i]].index == target_label)
                    target_node = v_nodes[i];
            }
//            source_node=v_nodes[source_label];
//            target_node=v_nodes[target_label];
        }
        
        //add the new edge to the graph
        bool inserted;
        boost::tie(e, inserted) = add_edge(source_node, target_node, CG_to);
        
        int s_pt=0, t_pt=0;
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label == source_label)
                s_pt = packing_seq[i].pt;
            if(packing_seq[i].label == target_label)
                t_pt = packing_seq[i].pt;
        }

//        int s_pt=packing_seq[source_label].pt;
//        int t_pt=packing_seq[target_label].pt;

        triangle_matrix.update_matrix(s_pt, t_pt, d, CG_from, e);
        
        Tune_Closure_Graphs(CG_to, CG_from, source_node, target_node, d);
        return 1;
    }
    else
        return 0;
}


//***************************************************************
////operator 2::reverse edge direction in one graph
//***************************************************************
int Clo_Red_Graph::reverse_edge(Graph& CG_invert, Graph& CG, char d)
{
    G_edge invert_edge, e;
    G_node source_node=0, target_node=0;
    int source_label, target_label;
    int success=1;
    Snode tmp;
    char d_new;
    
    success=Pick_Red_Edge(d, invert_edge, source_label, target_label, CG_invert);
    
    if(success){
        //delete the edge from G and add a new edge to G
//        int s_pt=packing_seq[source_label].pt;
//        int t_pt=packing_seq[target_label].pt;
        int s_pt=0, t_pt=0;
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label == source_label)
                s_pt = packing_seq[i].pt;
        }
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label == target_label)
                t_pt = packing_seq[i].pt;
        }
        
        if(d=='H'){
            for(int i=0; i<h_nodes.size(); i++){
                if(h_clo_G[h_nodes[i]].index == source_label){
                    source_node = h_nodes[i];
                }
                if(h_clo_G[h_nodes[i]].index == target_label){
                    target_node = h_nodes[i];
                }
            }
//            source_node=h_nodes[source_label];
//            target_node=h_nodes[target_label];
        }else{
            for(int i=0; i<v_nodes.size(); i++){
                if(v_clo_G[v_nodes[i]].index == source_label){
                    source_node = v_nodes[i];
                }
                if(v_clo_G[v_nodes[i]].index == target_label){
                    target_node = v_nodes[i];
                }
            }
//            source_node=v_nodes[source_label];
//            target_node=v_nodes[target_label];
        }
        
        bool inserted;
        boost::tie(e, inserted) = add_edge(target_node, source_node, CG_invert);
        triangle_matrix.update_matrix(t_pt, s_pt, d, CG_invert, e);
        
        //update packing sequence
        int m=0, n=0;
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label == source_label)
                m = i;
            if(packing_seq[i].label == target_label)
                n = i;
        }
        
        vector<Snode>::iterator sloc = packing_seq.begin() + m;
        vector<Snode>::iterator tloc = packing_seq.begin() + n;
    
        tmp = packing_seq[m];
        packing_seq.erase(sloc);
        packing_seq.insert(tloc, tmp);
        
        vector<Snode>::iterator start = sloc;
        vector<Snode>::iterator terminate = tloc-1;
        while(start != terminate){
            d_new = triangle_matrix.get_edge_type_c(source_label, (*sloc).pt);
            
            if(d_new == d){
                tmp = *sloc;
                packing_seq.erase(sloc);
                packing_seq.insert(tloc, tmp);
            }else
                sloc++;
            
            start++;
        }
        
        //tuning graphs
        Tune_Closure_Graphs(CG_invert, CG, target_node, source_node, d);
        return 1;
    }
    else
        return 0;
}


//***************************************************************
//operator 3::rotate a module
//***************************************************************
void Clo_Red_Graph::rotate(int clockwised){
    int node_label=rand()%modules_N;
    
    //clockwised==0, rotate positive 90 degree
    //clockwised==1, rotate negative 90 degree
    int back_up=modules_info[node_label].width;
    modules_info[node_label].width=modules_info[node_label].height;
    modules_info[node_label].height=back_up;
    
    if(clockwised==0){
        modules_info[node_label].degree++;
        
        if(modules_info[node_label].degree==4)
            modules_info[node_label].degree=0;
    }else{
        modules_info[node_label].degree--;
        
        if(modules_info[node_label].degree==-1)
            modules_info[node_label].degree=3;
    }
}


//***************************************************************
//operator 4::interchange two modules
//***************************************************************
void Clo_Red_Graph::interchange()
{
    int node_label1, node_label2;
//    int tmpindex;
    Snode tmpsnode;
    
    node_label1=rand()%modules_N;
    do{
        node_label2=rand()%modules_N;
    }while(node_label1==node_label2);
    
    //update packing sequence
    int k=0, l=0;
    for(int i=0; i<packing_seq.size(); i++){
        if(packing_seq[i].label == node_label1)
            k = i;
        if(packing_seq[i].label == node_label2)
            l = i;
    }
    tmpsnode = packing_seq[k];
    packing_seq[k] = packing_seq[l];
    packing_seq[l] = tmpsnode;
    
    //exchange the nodes in h_nodes
//    int m=0, n=0;
//    for(int i=0; i<h_nodes.size(); i++){
//        if(h_clo_G[h_nodes[i]].index == node_label1)
//            m = i;
//        if(h_clo_G[h_nodes[i]].index == node_label2)
//            n = i;
//    }
//    h_clo_G[h_nodes[m]].index = node_label2;
//    h_clo_G[h_nodes[n]].index = node_label1;
//    
//    int u=0, v=0;
//    for(int i=0; i<v_nodes.size(); i++){
//        if(v_clo_G[v_nodes[i]].index == node_label1)
//            u = i;
//        if(v_clo_G[v_nodes[i]].index == node_label2)
//            v = i;
//    }
//    v_clo_G[v_nodes[u]].index = node_label2;
//    v_clo_G[v_nodes[v]].index = node_label1;
    
//    h_clo_G[h_nodes[node_label1]].index = node_label2;
//    h_clo_G[h_nodes[node_label2]].index = node_label1;
//    v_clo_G[v_nodes[node_label1]].index = node_label2;
//    v_clo_G[v_nodes[node_label2]].index = node_label1;
    
//    G_node t;
//    t=h_nodes[node_label1];
//    h_nodes[node_label1]=h_nodes[node_label2];
//    h_nodes[node_label2]=t;
//    
//    t=v_nodes[node_label1];
//    v_nodes[node_label1]=v_nodes[node_label2];
//    v_nodes[node_label2]=t;
    
    //*********************Added by Zhenxin*********************
    char tmp;
    tmp=modules_info[node_label1].lc;
    modules_info[node_label1].lc=modules_info[node_label2].lc;
    modules_info[node_label2].lc=tmp;
    //**********************************************************
}


//***************************************************************
////operator 5::move an edge from one graph to another and reverse
//***************************************************************
int Clo_Red_Graph::move_edge_reverse(Graph& CG_from, Graph& CG_to, char d)
{
    G_edge move_edge;
    G_edge e;
    G_node source_node=0, target_node=0;
    int source_label, target_label;
    int success=1;
    Snode tmp;
    char d_new;
    
    //list_information();
    if(d=='H')
    {
        success=Pick_Red_Edge('V', move_edge, source_label, target_label, CG_from);
    }
    else
    {
        success=Pick_Red_Edge('H', move_edge, source_label, target_label, CG_from);
    }
    
    if(success){
//        int s_pt=packing_seq[source_label].pt;
//        int t_pt=packing_seq[target_label].pt;
        int s_pt=0, t_pt=0;
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label == source_label)
                s_pt = packing_seq[i].pt;
            if(packing_seq[i].label == target_label)
                t_pt = packing_seq[i].pt;
        }
        
        if(d=='H'){
            for(int i=0; i<h_nodes.size(); i++){
                if(h_clo_G[h_nodes[i]].index == source_label)
                    source_node = h_nodes[i];
                if(h_clo_G[h_nodes[i]].index == target_label)
                    target_node = h_nodes[i];
            }
//            source_node=h_nodes[source_label];
//            target_node=h_nodes[target_label];
        }else{
            for(int i=0; i<v_nodes.size(); i++){
                if(v_clo_G[v_nodes[i]].index == source_label)
                    source_node = v_nodes[i];
                if(v_clo_G[v_nodes[i]].index == target_label)
                    target_node = v_nodes[i];
            }
//            source_node=v_nodes[source_label];
//            target_node=v_nodes[target_label];
        }
        
        bool inserted;
        boost::tie(e, inserted) = add_edge(target_node, source_node, CG_to);
        triangle_matrix.update_matrix(t_pt, s_pt, d, CG_from, e);
        
        //update packing sequence
        int m=0, n=0;
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label == source_label)
                m = i;
            if(packing_seq[i].label == target_label)
                n = i;
        }
        vector<Snode>::iterator sloc = packing_seq.begin() + m;
        vector<Snode>::iterator tloc = packing_seq.begin() + n;
        
        tmp = packing_seq[m];
        packing_seq.erase(sloc);
        packing_seq.insert(tloc, tmp);
        
        vector<Snode>::iterator start = sloc;
        vector<Snode>::iterator terminate = tloc-1;
        while(start != terminate){
            d_new = triangle_matrix.get_edge_type_c(source_label, (*sloc).pt);
            
            if(d_new == d){
                tmp = *sloc;
                packing_seq.erase(sloc);
                packing_seq.insert(tloc, tmp);
            }else
                sloc++;
            start++;
        }
        
        Tune_Closure_Graphs(CG_to, CG_from, target_node, source_node, d);
        return 1;
    }
    else
        return 0;
}

/////////////////////////////////////////////////////
/////////////////////add by fuchen///////////////////
int Clo_Red_Graph::reverse_edge_keep(Graph& CG_invert, Graph& CG, char d)
{
    //same with TCG reverse
    G_edge invert_edge, e;
    G_node source_node=0, target_node=0;
    int source_label, target_label;
    int success=1;
    Snode tmp;
    char d_new;
    
    success=Pick_Red_Edge(d, invert_edge, source_label, target_label, CG_invert);
    
    if(success){
        //delete the edge from G and add a new edge to G
        //        int s_pt=packing_seq[source_label].pt;
        //        int t_pt=packing_seq[target_label].pt;
        int s_pt=0, t_pt=0;
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label == source_label)
                s_pt = packing_seq[i].pt;
        }
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label == target_label)
                t_pt = packing_seq[i].pt;
        }
        
        if(d=='H'){
            for(int i=0; i<h_nodes.size(); i++){
                if(h_clo_G[h_nodes[i]].index == source_label){
                    source_node = h_nodes[i];
                }
                if(h_clo_G[h_nodes[i]].index == target_label){
                    target_node = h_nodes[i];
                }
            }
            //            source_node=h_nodes[source_label];
            //            target_node=h_nodes[target_label];
        }else{
            for(int i=0; i<v_nodes.size(); i++){
                if(v_clo_G[v_nodes[i]].index == source_label){
                    source_node = v_nodes[i];
                }
                if(v_clo_G[v_nodes[i]].index == target_label){
                    target_node = v_nodes[i];
                }
            }
            //            source_node=v_nodes[source_label];
            //            target_node=v_nodes[target_label];
        }
        
        bool inserted;
        boost::tie(e, inserted) = add_edge(target_node, source_node, CG_invert);
        triangle_matrix.update_matrix(t_pt, s_pt, d, CG_invert, e);
        
        //update packing sequence
        int m=0, n=0;
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label == source_label)
                m = i;
            if(packing_seq[i].label == target_label)
                n = i;
        }
        
        vector<Snode>::iterator sloc = packing_seq.begin() + m;
        vector<Snode>::iterator tloc = packing_seq.begin() + n;
        
        tmp = packing_seq[m];
        packing_seq.erase(sloc);
        packing_seq.insert(tloc, tmp);
        
        vector<Snode>::iterator start = sloc;
        vector<Snode>::iterator terminate = tloc-1;
        while(start != terminate){
            d_new = triangle_matrix.get_edge_type_c(source_label, (*sloc).pt);
            
            if(d_new == d){
                tmp = *sloc;
                packing_seq.erase(sloc);
                packing_seq.insert(tloc, tmp);
            }else
                sloc++;
            
            start++;
        }
        
        //tuning graphs
        Tune_Closure_Graphs(CG_invert, CG, target_node, source_node, d);
        return 1;
    }
    else
        return 0;
}
//
//int Clo_Red_Graph::reverse_edge_delete(Graph& CG_invert, Graph& CG, char d)
//{
//    //not funished
//    G_edge invert_edge, e;
//    G_node source_node=0, target_node=0;
//    int source_label, target_label;
//    int success=1;
//    Snode tmp;
//    char d_new;
//
//    success=Pick_Red_Edge(d, invert_edge, source_label, target_label, CG_invert);
//
//    if(success){
//        //delete the edge from G and add a new edge to G
//        //        int s_pt=packing_seq[source_label].pt;
//        //        int t_pt=packing_seq[target_label].pt;
//        int s_pt=0, t_pt=0;
//        for(int i=0; i<packing_seq.size(); i++){
//            if(packing_seq[i].label == source_label)
//                s_pt = packing_seq[i].pt;
//        }
//        for(int i=0; i<packing_seq.size(); i++){
//            if(packing_seq[i].label == target_label)
//                t_pt = packing_seq[i].pt;
//        }
//
//        if(d=='H'){
//            for(int i=0; i<h_nodes.size(); i++){
//                if(h_clo_G[h_nodes[i]].index == source_label){
//                    source_node = h_nodes[i];
//                }
//                if(h_clo_G[h_nodes[i]].index == target_label){
//                    target_node = h_nodes[i];
//                }
//            }
//            //            source_node=h_nodes[source_label];
//            //            target_node=h_nodes[target_label];
//        }else{
//            for(int i=0; i<v_nodes.size(); i++){
//                if(v_clo_G[v_nodes[i]].index == source_label){
//                    source_node = v_nodes[i];
//                }
//                if(v_clo_G[v_nodes[i]].index == target_label){
//                    target_node = v_nodes[i];
//                }
//            }
//            //            source_node=v_nodes[source_label];
//            //            target_node=v_nodes[target_label];
//        }
//
//        bool inserted;
//        boost::tie(e, inserted) = add_edge(target_node, source_node, CG_invert);
//        triangle_matrix.update_matrix(t_pt, s_pt, d, CG_invert, e);
//
//        //update packing sequence

//
//        vector<Snode>::iterator sloc = packing_seq.begin() + m;
//        vector<Snode>::iterator tloc = packing_seq.begin() + n;
//
//        tmp = packing_seq[m];
//        packing_seq.erase(sloc);
//        packing_seq.insert(tloc, tmp);
//

//    }
//    else
//        return 0;
//}

int Clo_Red_Graph::remove_edge(Graph& CG_from, Graph& CG_to, char d)
{
    G_edge remove_edge, e;
    G_node source_node=0, target_node=0;
    int source_label, target_label;
    int success=1;
    
    if(d=='H')
    {
        success=Pick_Red_Edge('V', remove_edge, source_label, target_label, CG_from);
    }
    else{
        success=Pick_Red_Edge('H', remove_edge, source_label, target_label, CG_from);
    }
    
    if(success){
        if(d=='H'){
            for(int i=0; i<h_nodes.size(); i++){
                if(h_clo_G[h_nodes[i]].index == source_label)
                    source_node = h_nodes[i];
                if(h_clo_G[h_nodes[i]].index == target_label)
                    target_node = h_nodes[i];
            }

        }else{
            for(int i=0; i<v_nodes.size(); i++){
                if(v_clo_G[v_nodes[i]].index == source_label)
                    source_node = v_nodes[i];
                if(v_clo_G[v_nodes[i]].index == target_label)
                    target_node = v_nodes[i];
            }

        }

        return 1;
    }
    else
        return 0;
}

int Clo_Red_Graph::move_edge_keep(Graph& CG_from, Graph& CG_to, char d)
{
    //same with TCG move
    G_edge move_edge, e;
    G_node source_node=0, target_node=0;
    int source_label, target_label;
    int success=1;
    
    if(d=='H')
    {
        success=Pick_Red_Edge('V', move_edge, source_label, target_label, CG_from);
    }
    else{
        success=Pick_Red_Edge('H', move_edge, source_label, target_label, CG_from);
    }
    
    if(success){
        if(d=='H'){
            for(int i=0; i<h_nodes.size(); i++){
                if(h_clo_G[h_nodes[i]].index == source_label)
                    source_node = h_nodes[i];
                if(h_clo_G[h_nodes[i]].index == target_label)
                    target_node = h_nodes[i];
            }
            //            source_node=h_nodes[source_label];
            //            target_node=h_nodes[target_label];
        }else{
            for(int i=0; i<v_nodes.size(); i++){
                if(v_clo_G[v_nodes[i]].index == source_label)
                    source_node = v_nodes[i];
                if(v_clo_G[v_nodes[i]].index == target_label)
                    target_node = v_nodes[i];
            }
            //            source_node=v_nodes[source_label];
            //            target_node=v_nodes[target_label];
        }
        
        //add the new edge to the graph
        bool inserted;
        boost::tie(e, inserted) = add_edge(source_node, target_node, CG_to);
        
        int s_pt=0, t_pt=0;
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label == source_label)
                s_pt = packing_seq[i].pt;
            if(packing_seq[i].label == target_label)
                t_pt = packing_seq[i].pt;
        }
        
        //        int s_pt=packing_seq[source_label].pt;
        //        int t_pt=packing_seq[target_label].pt;
        
        triangle_matrix.update_matrix(s_pt, t_pt, d, CG_from, e);
        
        Tune_Closure_Graphs(CG_to, CG_from, source_node, target_node, d);
        return 1;
    }
    else
        return 0;
}

//int Clo_Red_Graph::move_edge_delete(Graph& CG_from, Graph& CG_to, char d)
//{
//    G_edge move_edge, e;
//    G_node source_node=0, target_node=0;
//    int source_label, target_label;
//    int success=1;
//
//    if(d=='H')
//    {
//        success=Pick_Red_Edge('V', move_edge, source_label, target_label, CG_from);
//    }
//    else{
//        success=Pick_Red_Edge('H', move_edge, source_label, target_label, CG_from);
//    }
//
//    if(success){
//        if(d=='H'){
//            for(int i=0; i<h_nodes.size(); i++){
//                if(h_clo_G[h_nodes[i]].index == source_label)
//                    source_node = h_nodes[i];
//                if(h_clo_G[h_nodes[i]].index == target_label)
//                    target_node = h_nodes[i];
//            }
//            //            source_node=h_nodes[source_label];
//            //            target_node=h_nodes[target_label];
//        }else{
//            for(int i=0; i<v_nodes.size(); i++){
//                if(v_clo_G[v_nodes[i]].index == source_label)
//                    source_node = v_nodes[i];
//                if(v_clo_G[v_nodes[i]].index == target_label)
//                    target_node = v_nodes[i];
//            }
//            //            source_node=v_nodes[source_label];
//            //            target_node=v_nodes[target_label];
//        }
//
//        //add the new edge to the graph
//        //not finished
//    }
//    else
//        return 0;
//}






///////////////////////////////////////////////////////

//---------------------------------------------------------------
//   Simulated Annealing Temporal Solution
//---------------------------------------------------------------
float Clo_Red_Graph::get_cost(){
    return Cost;
}


void Clo_Red_Graph::reconstruct(vector<Snode>& packing_seq_from,
                                vector<Snode>& packing_seq,
                                Graph& G_h,  Graph& G_v,
                                vector<G_node>& v_h, vector<G_node>& v_v,
                                Hv_Matrix& triangle_matrix_from, Hv_Matrix& triangle_matrix){
    //list_item litem;
    Snode sn;
    packing_seq.clear();
    G_h.clear();
    G_v.clear();
    G_node n;
    VertexProperty vp;
    
    for(int i=0; i<packing_seq_from.size(); i++){
        Snode sn = packing_seq_from[i];
        packing_seq.push_back(sn);
        
        vp.index = sn.label;
        n = add_vertex(vp, G_h);
        v_h[sn.label] = n;
        
        n = add_vertex(vp, G_v);
        v_v[sn.label] = n;
    }
    
    int s_label, t_label;
    int s_pt, t_pt;
    char d;
    G_edge e;
    
    for(int i=0; i<packing_seq.size()-1; i++){
        s_label=packing_seq[i].label;
        s_pt=packing_seq[i].pt;
        for(int j=i+1; j<packing_seq.size(); j++){
            t_label=packing_seq[j].label;
            t_pt=packing_seq[j].pt;
            d=triangle_matrix_from.get_edge_type_c(s_pt, t_pt);
            
            if(d=='H'){
                bool inserted;
                boost::tie(e, inserted) = add_edge(v_h[s_label], v_h[t_label], G_h);
                triangle_matrix.set_edge(s_pt, t_pt, e);
                triangle_matrix.set_edge_type(s_pt, t_pt, d);
            }else{
                bool inserted;
                boost::tie(e, inserted) = add_edge(v_v[s_label], v_v[t_label], G_v);
                triangle_matrix.set_edge(s_pt, t_pt, e);
                triangle_matrix.set_edge_type(s_pt, t_pt, d);
            }
        }
    }
}
    

void Clo_Red_Graph::get_solution(Solution &sol){
    triangle_matrix.copy_edge_types(sol.triangle_matrix);
    //***********Added by Zhenxin************
    sol.H_contour=H_contour;
    sol.V_contour=V_contour;
    //***************************************
    sol.packing_seq=packing_seq;
    sol.Area= Area;
    sol.Width=Width;
    sol.Height=Height;
    sol.Wire= Wire;
    sol.Cost = Cost;
    sol.modules_info=modules_info;
}


void Clo_Red_Graph::recover(Solution &sol){
    reconstruct(sol.packing_seq, packing_seq,
                h_clo_G, v_clo_G, h_nodes, v_nodes,
                sol.triangle_matrix, triangle_matrix);
    //*********Added by Zhenxin*************
    H_contour=sol.H_contour;
    V_contour=sol.V_contour;
    //**************************************
    Area=sol.Area;
    Wire=sol.Wire;
    Width=sol.Width;
    Height=sol.Height;
    Cost=sol.Cost;
    modules_info=sol.modules_info;
}


float rand_01(){
    return float(rand()%10000)/10000;
}
