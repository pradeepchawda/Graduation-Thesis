//
//  merge.cc
//  980Merge
//
//  Created by Hensin on 2016-02-22.
//  Copyright (c) 2016 Hensin. All rights reserved.
//


#include "merge.h"
#include "msa.h"
#include <fstream>
#include <math.h>
#include "My_window.h"


//------------------------------------------------------------------------
//Get the information of a cluster from a file and store it
//------------------------------------------------------------------------
void Merge::read(Cluster& clt,char* file,float init_temp,float temp_scale,float term_temp,float cvg_r,int times)
{
    //implement the simulated annealing-based algorithm to a cluster to get the best solution
    MSA msa;
    Clo_Red_Graph fp;
    cout << file << endl;

    fp.read(file);
        fp.init_sqr();
    msa.sa_floorplanning(fp,init_temp,temp_scale,term_temp,cvg_r,times);
    
    //get the basic information of a cluster
    clt.setWidth(fp.getWidth());
    clt.setHeight(fp.getHeight());
    clt.setArea(fp.getArea());
    clt.setModules(fp.modules);
    clt.setModules_Info(fp.modules_info);
    clt.setCltmatrix(fp.getHv_Matrix());
    clt.setPackingSq(fp.getPackingSeq());
    clt.setX(0);
    clt.setY(0);
//    clt.setX_(fp.getWidth());
//    clt.setY_(fp.getHeight());
    
    Modules_Info MI = clt.getModules_Info();
    for(int i=0; i<MI.size(); i++){
        MI[i].rdx = MI[i].x;
        MI[i].rdy = MI[i].y;
    }
    clt.setModules_Info(MI);
    
    //get the final Horizontal Contour of a cluster after sa
    clt.setHcontour(fp.getH_contour());
    
    //get the final Vertical Contour of a cluster after sa
    clt.setVcontour(fp.getV_contour());
    
    //store the cluster into the vector
    clusters.push_back(clt);
    
    
    TotalArea += clt.getArea();
}


//------------------------------------------------------------------------
//Initialize the location relationships between clusters
//------------------------------------------------------------------------
void Merge::init_relation(){
    
    int i, j;
    VertexProperty vp;
    G_node n;
    G_edge e;
    Cluster clt;
    
    int N = int(clusters.size());
    clustersmatrix.init(N);
    
    for(i=0; i<N; i++){
        vp.index = i;
        
        //construct nodes for HCG
        n = add_vertex(vp, h_clo_G);
        h_nodes.push_back(n);
        
        //construct nodes for VCG
        n = add_vertex(vp, v_clo_G);
        v_nodes.push_back(n);
        
        //for packing sequence
        clt.label=i;
        clt.pt=i;
        packing_seq.push_back(clt);
    }
    
    int l=1;
    int len=(int)ceil(sqrt(N));
    bool inserted;
    for(i=0; i<N; i++){
        l=(i%len)+1;
        for(j=i+1; j<N; j++){
            if(l<len){
                boost::tie(e, inserted) = add_edge(h_nodes[i], h_nodes[j], h_clo_G);
                
                clustersmatrix.set_edge(i, j, e);
                clustersmatrix.set_edge_type(i, j, H);
            }else{
                boost::tie(e, inserted) = add_edge(v_nodes[i], v_nodes[j], v_clo_G);
                
                clustersmatrix.set_edge(i, j, e);
                clustersmatrix.set_edge_type(i, j, V);
            }
            l++; 
        }
    }
    
    //initialize the name and lc of each cluster that to be merged
    for(int i=0; i<clusters.size(); i++){
        clusters[i].setName(i+65);
        clusters[i].setLc(i);
    }
}


//------------------------------------------------------------------------
//Print the location relationships between clusters
//------------------------------------------------------------------------
void Merge::printRelationship(){
    int i, j;
    for(i=0; i <clusters.size(); i++){
        for(j=i+1; j< clusters.size(); j++){
            cout<<clustersmatrix.get_edge_type_c(i, j)<<" ";
        }
        cout<<endl;
    }
}


//------------------------------------------------------------------------
//Add dummy modules to all the clusters that need to be merged
//------------------------------------------------------------------------
void Merge::addDummyMd(){
    for(int i=0; i<clusters.size(); i++){
        
        Modules_Info modules_info;
        Modules_Info hc;
        Modules_Info vc;
        Module_Info dummy1, dummy2;
        dummy1.name = 'y';
        dummy2.name = 'x';
        
        //move the cluster to coordinate (dx,dy)
        clusters[i].setX(clusters[i].getDx());
        clusters[i].setY(clusters[i].getDy());
        
        //reset the rdx and rdy for all the modules in each cluster
        Modules_Info mis = clusters[i].getModules_Info();
        for(int j=0; j<mis.size(); j++){
            mis[j].rdx += clusters[i].getDx();
            mis[j].rdy += clusters[i].getDy();
        }
        clusters[i].setModules_Info(mis);
        
        //update the information of modules in the orignal cluster
        clusters[i].update();
        
        //build two dummy modules and initialize the information
        dummy1.x = 0;
        dummy1.y = 0;
        dummy1.rx = clusters[i].getX_();
        dummy1.ry = clusters[i].getDy();
        dummy1.rdx = 0;
        dummy1.rdy = 0;
        dummy1.width = clusters[i].getX_();
        dummy1.height = clusters[i].getDy();
        dummy2.x = 0;
        dummy2.y = clusters[i].getDy();
        dummy2.rx = clusters[i].getDx();
        dummy2.ry = clusters[i].getY_();
        dummy2.rdx = 0;
        dummy2.rdy = clusters[i].getDy();
        dummy2.width = clusters[i].getDx();
        dummy2.height = clusters[i].getY_()-clusters[i].getDy();
        
        //add the dummy information to the cluster
        modules_info.push_back(dummy1);
        modules_info.push_back(dummy2);
        Modules_Info mis2 = clusters[i].getModules_Info();
        for(int j=0; j<mis2.size(); j++){
            modules_info.push_back(mis2[j]);
        }
        clusters[i].setModules_Info(modules_info);
        
        //update the Horizontal Contour
        hc.push_back(dummy2);
        Modules_Info hcs = clusters[i].getHcontour();
        for(int k=0; k<hcs.size(); k++){
            hc.push_back(hcs[k]);
        }
        clusters[i].setHcontour(hc);
        
        //update the Vertical Contour
        vc.push_back(dummy1);
        Modules_Info vcs = clusters[i].getVcontour();
        for(int l=0; l<vcs.size(); l++){
            vc.push_back(vcs[l]);
        }
        clusters[i].setVcontour(vc);
        
        //update the coordinate, width, and height
        clusters[i].setX(0);
        clusters[i].setY(0);
        clusters[i].setWidth(clusters[i].getWidth()+clusters[i].getDx());
        clusters[i].setHeight(clusters[i].getHeight()+clusters[i].getDy());
        
    }
}


//------------------------------------------------------------------------
//Remove the dummy modules in all the merged clusters
//------------------------------------------------------------------------
void Merge::removeDummyMd(){
    
    for(int i=0; i<clusters.size(); i++){
        
        Modules_Info modules_info;
        Modules_Info hc;
        Modules_Info vc;
        Module_Info dummy1, dummy2;
        
        //remove the dummy information for each cluster
        Modules_Info mis = clusters[i].getModules_Info();
        for(int j=0; j<mis.size(); j++){
            if(j==0)
                dummy1 = mis[j];
            else if(j==1)
                dummy2 = mis[j];
            else
                modules_info.push_back(mis[j]);
        }
        clusters[i].setModules_Info(modules_info);
        
        //reset the rdx and rdy for all the modules in a cluster
        Modules_Info mis1 = clusters[i].getModules_Info();
        for(int j=0; j<mis1.size(); j++){
            mis1[j].rdx -= clusters[i].getDx();
            mis1[j].rdy -= clusters[i].getDy();
        }
        clusters[i].setModules_Info(mis1);
        
        //reset the X and Y for each cluster
        clusters[i].setX(clusters[i].getX()+clusters[i].getDx());
        clusters[i].setY(clusters[i].getY()+clusters[i].getDy());
        
        //remove the dummy from Horization Contour
        Modules_Info hcs = clusters[i].getHcontour();
        for(int k=0; k<hcs.size(); k++){
            if(k != 0){
                hc.push_back(hcs[k]);
            }
        }
        clusters[i].setHcontour(hc);
        
        //remove the dummy from Vertical Contour
        Modules_Info vcs = clusters[i].getVcontour();
        for(int l=0; l<vcs.size(); l++){
            if(l != 0)
                vc.push_back(vcs[l]);
        }
        clusters[i].setVcontour(vc);
        
        //update the width, and height
        clusters[i].setWidth(clusters[i].getWidth()-clusters[i].getDx());
        clusters[i].setHeight(clusters[i].getHeight()-clusters[i].getDy());
        
        clusters[i].update();
    }
    
    //update the information of cluster in clustersHcontour and clustersVcontour
    update_clustersHcontour(clustersHcontour);
    update_clustersVcontour(clustersVcontour);
}


//------------------------------------------------------------------------
//Update the information in the attribute of clustersHcontour
//------------------------------------------------------------------------
void Merge::update_clustersHcontour(vector<Cluster> H_contour){
    vector<Cluster> Hc;
    for(int i =0; i<H_contour.size();i++){
        for(int j=0; j<clusters.size(); j++){
            if(clusters[j].getName()==H_contour[i].getName()){
                Hc.push_back(clusters[j]);
            }
        }
    }
    clustersHcontour = Hc;
}


//------------------------------------------------------------------------
//Update the information in the attribute of clustersVcontour
//------------------------------------------------------------------------
void Merge::update_clustersVcontour(vector<Cluster> V_contour){
    vector<Cluster> Vc;
    for(int i =0; i<V_contour.size();i++){
        for(int j=0; j<clusters.size(); j++){
            if(clusters[j].getName()==V_contour[i].getName()){
                Vc.push_back(clusters[j]);
            }
        }
    }
    clustersVcontour = Vc;
}


//------------------------------------------------------------------------
//Merge clusters by employing red-black tree
//------------------------------------------------------------------------
void Merge::mergeClusters(){
    
    addDummyMd();
    
    //Construct two red-black trees and initialize them
    RBT Ts, Tv;
    Ts.insertHead('s');
    Tv.insertHead('t');
    
    //Merge clusters
//  cout<<"Clusters' Packing Sequence: ";
    for(int i=0; i<packing_seq.size(); i++){
        Cluster clt = packing_seq[i];
//      cout<<clusters[clt.label].getName()<<" ";
        Ts.insert_cluster(&(clusters[clt.label]), &clustersmatrix);
        Tv.insert_cluster(&(clusters[clt.label]), &clustersmatrix);
    }
    
    //update the information of modules in all the clusters after merging
    update();
    
    //Get the horizontal contour in terms of cluster
    Ts.inorder_clusters();
    vector<Cluster> H_contour = Ts.get_clt_contour();
    update_clustersHcontour(H_contour);
    
    //Get the vertical contour in terms of cluster
    Tv.inorder_clusters();
    vector<Cluster> V_contour = Tv.get_clt_contour();
    update_clustersVcontour(V_contour);
    
    //Get the width, height, area after packing
    Width = clustersHcontour.back().getX_();
    Height = clustersVcontour.back().getY_();
    Area = Width*Height;
    Cost = Area;
    
    removeDummyMd();
}


//------------------------------------------------------------------------
//Get the horizontal and vertical contours in terms of module
//------------------------------------------------------------------------
void Merge::ct_modulesContour(){
    
    //get merged Horizontal Contour in terms of module
    Modules_Info mhc;
    for(int i=0; i<clustersHcontour.size(); i++){
        if(i==0){
            Modules_Info mi = clustersHcontour[i].getHcontour();
            for(int j=0; j<mi.size(); j++){
                mhc.push_back(mi[j]);
            }
        }
        else{
            Modules_Info pmi = clustersHcontour[i-1].getHcontour();
            int pmiMAX = pmi.back().rx;
            
            Modules_Info cmi = clustersHcontour[i].getHcontour();
            for(int k=0; k<cmi.size(); k++){
                if(cmi[k].rx>pmiMAX){
                    mhc.push_back(cmi[k]);
                }
            }
        }
    }
    modulesHcontour = mhc;
    
    //get merged Vertical Contour in terms of module
    Modules_Info mvc;
    for(int i=0; i<clustersVcontour.size(); i++){
        
        if(i==0){
            Modules_Info mi = clustersVcontour[i].getVcontour();
            for(int j=0; j<mi.size(); j++){
                mvc.push_back(mi[j]);
            }
        }
        else{
            Modules_Info pmi = clustersVcontour[i-1].getVcontour();
            int pmiMAX = pmi.back().ry;
            
            Modules_Info cmi = clustersVcontour[i].getVcontour();
            for(int k=0; k<cmi.size(); k++){
                if(cmi[k].ry>pmiMAX){
                    mvc.push_back(cmi[k]);
                }
            }
        }
    }
    modulesVcontour = mvc;
}


//------------------------------------------------------------------------
//Print the horizontal and vertical contours in terms of cluster
//------------------------------------------------------------------------
void Merge::print_clustersHVC(){
    cout<<"Horizontal Contour in terms of cluster: ";
    for(int i=0; i<clustersHcontour.size(); i++){
        cout<<clustersHcontour[i].getName()<<" ";
    }
    cout<<endl;
    cout<<"Vertical Contour in terms of cluster: ";
    for(int i=0; i<clustersVcontour.size(); i++){
        cout<<clustersVcontour[i].getName()<<" ";
    }
    cout<<endl;
}


void Merge::update(){
    for(int i=0; i<clusters.size(); i++){
        clusters[i].update();
    }
}


//------------------------------------------------------------------------
//Print the horizontal and vertical contours in terms of module
//------------------------------------------------------------------------
void Merge::print_modulesHVC(){
    cout<<"Horizontal Contour in terms of module: ";
    for(int i=0; i<modulesHcontour.size(); i++){
        cout<<modulesHcontour[i].name<<" ";
    }
    cout<<endl;
    cout<<"Vertical Contour in terms of module: ";
    for(int i=0; i<modulesVcontour.size(); i++){
        cout<<modulesVcontour[i].name<<" ";
    }
    cout<<endl;
}

//------------------------------------------------------------------------
//Show the result of clusters after merging in terms of module
//------------------------------------------------------------------------
int Merge::draw_mergedModules(){
    My_window win(Point(150,0),1000,700,clusters,"Cluster");
    return gui_main();
}

//===================================================================
// perturbation
//===================================================================
void Merge::perturbate1()
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
                    //rotate one module
                    rotate();
                    break;
                }
            }
        }
    }while(!success);
}

void Merge::perturbate2()
{
    int success;
    
    do{
        int c=rand()%5;
        success=1;
        switch (c) {
            case 0:{
                //rotate one module
                rotate();
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

void Merge::perturbate3()
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
                    //rotate one module
                    rotate();
                    break;
                }
            }
        }
    }while(!success);
}


//===========================================================================
// Perturbation Operations
//===========================================================================
//-------------------------------------------------------------------
// Get a random edge of Graph g
//-------------------------------------------------------------------
G_edge Merge::C_choose_edge(Graph g){
    
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
void Merge::C_list_Graph(Graph& g){
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

//---------------------------------------------------------------
// Pick_Red_Edge is used to pick a reduction edge
//---------------------------------------------------------------
int Merge::Pick_Red_Edge(char c, G_edge& pick_e,
                         int& s_label, int& t_label,
                         Graph& G)
{
    if(num_edges(G)!=0){
        G_edge e = C_choose_edge(G);
        int index= G[source(e,G)].index;
        Cluster init_sn, sn;
        char cur_c;
        int k=0;
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label==index){
                k = i;
                init_sn = packing_seq[i];
            }
        }
        vector<Cluster> modules1, modules2;
        
        k++;
        for(int i=k; i<packing_seq.size(); i++){
            sn=packing_seq[i];
            cur_c=clustersmatrix.get_edge_type_c(init_sn.pt, sn.pt);
            
            //the edge (init_module, module) appears in the same graph
            if(c==cur_c){
                modules1.push_back(sn);
            }
        }
        
        Cluster first_sn, s_sn;
        s_sn=modules1[0];
        for(int j=1; j<modules1.size(); j++){
            modules2.push_back(modules1[j]);
        }
        
        first_sn=s_sn;
        int n = int(modules2.size());
        int l=0;
        for(int i=0; i<n; i++){
            sn = modules2[l];
            cur_c=clustersmatrix.get_edge_type_c(s_sn.pt, sn.pt);
            
            //the edge (s_module, module) appears in the same graph
            if(cur_c==c){
                vector<Cluster>::iterator it = modules2.begin() + l;
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
            pick_e=clustersmatrix.get_edge(init_sn.pt, sn.pt);
            return 1;
        }else
            return 0;
    }else
        return 0;
}


//---------------------------------------------------------------
// Tuning graphs while add edges
//---------------------------------------------------------------
void Merge::Tune_Closure_Graphs(Graph& CG_to,
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
            
            if(clustersmatrix.get_edge_type_c(s_pt, t_pt)!=d){
                bool inserted;
                boost::tie(e, inserted) = add_edge(sn, edge_target_node, CG_to);
                clustersmatrix.update_matrix(s_pt, t_pt, d, CG_from, e);
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
            
            if(clustersmatrix.get_edge_type_c(s_pt, t_pt)!=d){
                bool inserted;
                boost::tie(e, inserted) = add_edge(edge_source_node, tn, CG_to);
                clustersmatrix.update_matrix(s_pt, t_pt, d, CG_from, e);
                
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
                    
                    if(clustersmatrix.get_edge_type_c(s_pt, t_pt)!=d){
                        bool inserted;
                        boost::tie(e, inserted) = add_edge(sn, tn, CG_to);
                        clustersmatrix.update_matrix(s_pt, t_pt, d, CG_from, e);
                    }
                }
            }
        }
    }
}

//---------------------------------------------------------------
// operator 1::move an edge from one graph to another
//---------------------------------------------------------------
int Merge::move_edge(Graph& CG_from, Graph& CG_to, char d)
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
        
        clustersmatrix.update_matrix(s_pt, t_pt, d, CG_from, e);
        
        Tune_Closure_Graphs(CG_to, CG_from, source_node, target_node, d);
        return 1;
    }
    else
        return 0;
}

//---------------------------------------------------------------
// operator 2::reverse edge direction in one graph
//---------------------------------------------------------------
int Merge::reverse_edge(Graph& CG_invert, Graph& CG, char d)
{
    G_edge invert_edge, e;
    G_node source_node=0, target_node=0;
    int source_label, target_label;
    int success=1;
    Cluster tmp;
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
        clustersmatrix.update_matrix(t_pt, s_pt, d, CG_invert, e);
        
        //update packing sequence
        int m=0, n=0;
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label == source_label)
                m = i;
            if(packing_seq[i].label == target_label)
                n = i;
        }
        
        vector<Cluster>::iterator sloc = packing_seq.begin() + m;
        vector<Cluster>::iterator tloc = packing_seq.begin() + n;
        
        tmp = packing_seq[m];
        packing_seq.erase(sloc);
        packing_seq.insert(tloc, tmp);
        
        vector<Cluster>::iterator start = sloc;
        vector<Cluster>::iterator terminate = tloc-1;
        while(start != terminate){
            d_new = clustersmatrix.get_edge_type_c(source_label, (*sloc).pt);
            
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

//---------------------------------------------------------------
// operator 3::rotate a module
//---------------------------------------------------------------
void Merge::rotate(){
    
    int node_label=rand()%(clusters.size());
    
    //rotate all the modules in the cluster
    Modules_Info mis = clusters[node_label].getModules_Info();
    
    for(int i=0; i<mis.size(); i++){
        int back_up = mis[i].width;
        mis[i].width = mis[i].height;
        mis[i].height = back_up;
    }
    
    //change the relationship between all the modules
    Hv_Matrix matrix = clusters[node_label].getCltmatrix();
    for(int i=0; i<mis.size(); i++){
        for(int j=i+1; j<mis.size(); j++){
            if(matrix.get_edge_type_c(i, j) == 'H')
                matrix.set_edge_type(i,j,V);
            else
                matrix.set_edge_type(i,j,H);
        }
    }
    
    //packing the modules according to the packing sequence and relationship
    vector<int> packingSq = clusters[node_label].getPackingSq();
    RBT Ts, Tv;
    Ts.insertHead('s');
    Tv.insertHead('t');
    for(int i=0; i<packingSq.size(); i++){
        int seq = packingSq[i];
        Ts.insert_(&(mis[seq]), &matrix);
        Tv.insert_(&(mis[seq]), &matrix);
    }
    
    //update the module information in the cluster
    //get the rdx and rdy for all the modules
    for(int i=0; i<mis.size(); i++){
        mis[i].rdx = mis[i].x;
        mis[i].rdy = mis[i].y;
    }
    //update the coordinates for all the modules
    for(int i=0; i<mis.size(); i++){
        mis[i].x = mis[i].rdx + clusters[node_label].getX();
        mis[i].y = mis[i].rdy + clusters[node_label].getY();
        mis[i].rx = mis[i].x + mis[i].width;
        mis[i].ry = mis[i].y + mis[i].height;
    }
    clusters[node_label].setModules_Info(mis);
    
    //update the Horizontal contour for the cluster
    Ts.inorder();
    Modules_Info Hcontour = Ts.getContour();
    Modules_Info hc;
    for(int i=0; i<Hcontour.size(); i++){
        for(int j=0; j<mis.size(); j++){
            if(Hcontour[i].name==mis[j].name){
                hc.push_back(mis[j]);
            }
        }
    }
    clusters[node_label].setHcontour(hc);
    
    //update the vertical contour for the cluster
    Tv.inorder();
    Modules_Info Vcontour = Tv.getContour();
    Modules_Info vc;
    for(int i=0; i<Vcontour.size(); i++){
        for(int j=0; j<mis.size(); j++){
            if(Vcontour[i].name==mis[j].name){
                vc.push_back(mis[j]);
            }
        }
    }
    clusters[node_label].setVcontour(vc);
        
    //update the Width, Height for the cluster
    int width = hc.back().rx-hc.front().x;
    clusters[node_label].setWidth(width);
    int height = vc.back().ry-vc.front().y;
    clusters[node_label].setHeight(height);
    
}

//---------------------------------------------------------------
// operator 4::interchang two modules
//---------------------------------------------------------------
void Merge::interchange()
{
    int node_label1, node_label2;
    
    node_label1=rand()%(clusters.size());
    do{
        node_label2=rand()%(clusters.size());
    }while(node_label1==node_label2);
    
    //update packing sequence
    packing_seq[node_label1].label=node_label2;
    packing_seq[node_label2].label=node_label1;
    
    //exchange the nodes in h_nodes
    h_clo_G[h_nodes[node_label1]].index = node_label2;
    h_clo_G[h_nodes[node_label2]].index = node_label1;
    v_clo_G[v_nodes[node_label1]].index = node_label2;
    v_clo_G[v_nodes[node_label2]].index = node_label1;
    
    G_node n;
    n=h_nodes[node_label1];
    h_nodes[node_label1]=h_nodes[node_label2];
    h_nodes[node_label2]=n;
    
    n=v_nodes[node_label1];
    v_nodes[node_label1]=v_nodes[node_label2];
    v_nodes[node_label2]=n;
    
    
    //*********************Added by Zhenxin*********************
    char tmp;
    tmp=clusters[node_label1].getLc();
    clusters[node_label1].setLc(clusters[node_label2].getLc());
    clusters[node_label2].setLc(tmp);
    //**********************************************************
}

//---------------------------------------------------------------
// operator 5::move an edge from one graph to another and reverse
//---------------------------------------------------------------
int Merge::move_edge_reverse(Graph& CG_from, Graph& CG_to, char d)
{
    G_edge move_edge;
    G_edge e;
    G_node source_node=0, target_node=0;
    int source_label, target_label;
    int success=1;
    Cluster tmp;
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
        clustersmatrix.update_matrix(t_pt, s_pt, d, CG_from, e);
        
        //update packing sequence
        int m=0, n=0;
        for(int i=0; i<packing_seq.size(); i++){
            if(packing_seq[i].label == source_label)
                m = i;
            if(packing_seq[i].label == target_label)
                n = i;
        }
        vector<Cluster>::iterator sloc = packing_seq.begin() + m;
        vector<Cluster>::iterator tloc = packing_seq.begin() + n;
        
        tmp = packing_seq[m];
        packing_seq.erase(sloc);
        packing_seq.insert(tloc, tmp);
        
        vector<Cluster>::iterator start = sloc;
        vector<Cluster>::iterator terminate = tloc-1;
        while(start != terminate){
            d_new = clustersmatrix.get_edge_type_c(source_label, (*sloc).pt);
            
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


//===============================================================================
// Simulated Annealing Temporal Solution
//===============================================================================
//---------------------------------------------------------------
//Reconstruct a TCG-S from another TCG-S
//---------------------------------------------------------------
void Merge::reconstruct(vector<Cluster>& packing_seq_from,
                        vector<Cluster>& packing_seq,
                        Graph& G_h,  Graph& G_v,
                        vector<G_node>& v_h, vector<G_node>& v_v,
                        Hv_Matrix& triangle_matrix_from, Hv_Matrix& triangle_matrix){

    //list_item litem;
    Cluster sn;
    packing_seq.clear();
    G_h.clear();
    G_v.clear();
    G_node n;
    VertexProperty vp;
    
    for(int i=0; i<packing_seq_from.size(); i++){
        Cluster sn = packing_seq_from[i];
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

//---------------------------------------------------------------
//store the information into the solution
//---------------------------------------------------------------
void Merge::get_solution(Solution_C &sol){
    clustersmatrix.copy_edge_types(sol.cltsmatrix);
    sol.H_ccontour=clustersHcontour;
    sol.V_ccontour=clustersVcontour;
    sol.packing_seq=packing_seq;
    sol.Area= Area;
    sol.Width=Width;
    sol.Height=Height;
    sol.Wire=Wire;
    sol.Cost=Cost;
    sol.clts=clusters;
}

//---------------------------------------------------------------
// Recover the information from the solution
//---------------------------------------------------------------
void Merge::recover(Solution_C &sol){
    reconstruct(sol.packing_seq, packing_seq,
                h_clo_G, v_clo_G, h_nodes, v_nodes,
                sol.cltsmatrix, clustersmatrix);
    clustersHcontour=sol.H_ccontour;
    clustersVcontour=sol.V_ccontour;
    packing_seq=sol.packing_seq;
    Area=sol.Area;
    Wire=sol.Wire;
    Width=sol.Width;
    Height=sol.Height;
    Cost=sol.Cost;
    clusters=sol.clts;
}

//---------------------------------------------------------------
// Test the operation of rotation
//---------------------------------------------------------------
void Merge::rotate_test(){
    
    int node_label=1;//rand()%(clusters.size());
    
    //rotate all the modules in the cluster
    Modules_Info mis = clusters[node_label].getModules_Info();
    
    for(int i=0; i<mis.size(); i++){
        int back_up = mis[i].width;
        mis[i].width = mis[i].height;
        mis[i].height = back_up;
    }
    
    //change the relationship between all the modules
    Hv_Matrix matrix = clusters[node_label].getCltmatrix();
    for(int i=0; i<mis.size(); i++){
        for(int j=i+1; j<mis.size(); j++){
            if(matrix.get_edge_type_c(i, j) == 'H')
                matrix.set_edge_type(i,j,V);
            else
                matrix.set_edge_type(i,j,H);
        }
    }
    
    //packing the modules according to the packing sequence and relationship
    vector<int> packingSq = clusters[node_label].getPackingSq();
    cout<<"Modules' Packing Sequence's Label: ";
    for(int i=0; i<packingSq.size(); i++){
        cout<<packingSq[i]<<" ";
    }
    cout<<endl;
    
    RBT Ts, Tv;
    Ts.insertHead('s');
    Tv.insertHead('t');
    
    cout<<"Modules' Packing Sequence: ";
    for(int i=0; i<packingSq.size(); i++){
        int seq = packingSq[i];
        cout<<mis[seq].name<<" ";
        Ts.insert_(&(mis[seq]), &matrix);
        Tv.insert_(&(mis[seq]), &matrix);
    }
    
    //update the module information in the cluster
    //get the rdx and rdy for all the modules
    for(int i=0; i<mis.size(); i++){
        mis[i].rdx = mis[i].x;
        mis[i].rdy = mis[i].y;
    }
    //update the coordinates for all the modules
    for(int i=0; i<mis.size(); i++){
        mis[i].x = mis[i].rdx + clusters[node_label].getX();
        mis[i].y = mis[i].rdy + clusters[node_label].getY();
        mis[i].rx = mis[i].x + mis[i].width;
        mis[i].ry = mis[i].y + mis[i].height;
    }
    clusters[node_label].setModules_Info(mis);
    
    //update the Horizontal contour for the cluster
    Ts.inorder();
    Modules_Info Hcontour = Ts.getContour();
    Modules_Info hc;
    for(int i=0; i<Hcontour.size(); i++){
        for(int j=0; j<mis.size(); j++){
            if(Hcontour[i].name==mis[j].name){
                hc.push_back(mis[j]);
            }
        }
    }
    clusters[node_label].setHcontour(hc);
    
    //update the vertical contour for the cluster
    Tv.inorder();
    Modules_Info Vcontour = Tv.getContour();
    Modules_Info vc;
    for(int i=0; i<Vcontour.size(); i++){
        for(int j=0; j<mis.size(); j++){
            if(Vcontour[i].name==mis[j].name){
                vc.push_back(mis[j]);
            }
        }
    }
    clusters[node_label].setVcontour(vc);
    
    
    //update the Width, Height for the cluster
    int width = hc.back().rx-hc.front().x;
    clusters[node_label].setWidth(width);
    int height = vc.back().ry-vc.front().y;
    clusters[node_label].setHeight(height);
    
}




