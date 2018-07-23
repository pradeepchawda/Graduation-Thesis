//
//  rbt.h
//  980ProjectA
//
//  Created by Hensin on 2015-11-25.
//  Copyright (c) 2015 Hensin. All rights reserved.
//

#ifndef _80ProjectA_RBT_h
#define _80ProjectA_RBT_h


#include <stack>
#include "fplan.h"
#include "hv_matrix.h"
#include "cluster.h"


using namespace std;
#define INFI 1000000000
#define BLACK 1
#define RED 0
#define TT 1    //: specifying the redBlackTree with t-edge
#define SS 0    //: specifying the redBlackTree with s-edge


//Tree node used to store module information
class Node {
public:
    int lc;
    char value;  //node name
    bool color;  //red of black of each node indicate their position
    Node *leftTree, *rightTree, *parent;  //Used to store subtrees and parent node
    int width, height;  //Module's width and height
    int X, Y, X_, Y_;  //X,Y means left-bottom corner, X_,Y_ means right-top corner
    
    //Initiate the new node
    Node() {
        color = RED;
        leftTree = NULL;
        rightTree = NULL;
        parent = NULL;
        value = '0';
        width = 0; height = 0;
        X = 0; Y = 0; X_ = 0; Y_ = 0;
    }
    
    ~Node() {};
    
    //Get grandparent node of current tree node
    Node* grandparent() {
        if (parent == NULL) {
            return NULL;
        }
        return parent->parent;
    };
    
    //Search same level node of parent node
    Node* uncle() {
        if (grandparent() == NULL) {
            return NULL;
        }
        if (parent == grandparent()->rightTree)
            return grandparent()->leftTree;
        else
            return grandparent()->rightTree;
    };
    
    //Search same level node of current tree node
    Node* sibling() {//brother tree
        if (parent->leftTree == this)
            return parent->rightTree;
        else
            return parent->leftTree;
    }
};


class RBT {
private:
    void rotate_right(Node *p);
    void rotate_left(Node *p);
    void inorder(Node *p);
    void traverseTree(Node *p, char data, stack<Node> *nodes);
    void traverseTree_Delete(Node *p, int x_, int y_);
    Node* getSmallestChild(Node *p);
    Node* getLargestChild(Node *p);
    void delete_child_(Node *p, char data);
    bool delete_child(Node *p, char data);
    void delete_one_child_(Node *p);    
    void delete_one_child(Node *p);
    void delete_case(Node *p);
    void insert(Node *p, char data);
    int getRelation(Node *p, Module_Info *m, Hv_Matrix *matrix);
    void insert_rb_(Node *p, Module_Info *m, Hv_Matrix *matrix);
    void insert_case(Node *p);
    void DeleteTree(Node *p);
//***************Added by Zhenxin**************
    int getRelationc(Node *p, Cluster *c, Hv_Matrix *matrix);
    void insert_cluster_(Node *p, Cluster *c, Hv_Matrix *matrix);
    void search_last(char data);
    void search(Node *p);
    void inorder_clusters_(Node *p);
//*********************************************

public:
    RBT() {
        NIL = new Node();
        NIL->color = BLACK;
        root = NULL;
        fixed = false;
    }
    
    ~RBT() {
        if (root)
            DeleteTree(root);
        delete NIL;
    }
    
    void inorder();
    void insertHead(char x);
    void insert_(module *m, Hv_Matrix *matrix);
    void traverseTree(char data, stack<Node> *nodes);
    void delete_(char data);
    bool delete_value(char data);
    //****************Added by Zhenxin******************
    void insert_cluster(Cluster *c, Hv_Matrix *matrix);
    void inorder_clusters();
    vector<Module_Info> getContour();
    vector<Cluster> get_clt_contour();
    //**************************************************
    
private:
    //*****************Added by Zhenxin*************************
    vector<Module_Info> contour;
    vector<Cluster> clustersContour;
    vector<Node> order;
    Node last;
    //**********************************************************
    //Used to indicates root node and leave node as definition of RBT
    Node *root, *NIL;
    //Used to check whether hte tree is 's' tree or 't' tree
    bool hv;
    //Used to check whether certain node arrived when recurs the tree
    bool fixed;
};

#endif
