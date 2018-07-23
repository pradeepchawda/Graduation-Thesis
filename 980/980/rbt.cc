//
//  rbt.cc
//  980ProjectA
//
//  Created by Hensin on 2015-11-24.
//  Copyright (c) 2015 Hensin. All rights reserved.
//

#include "rbt.h"

//--------------------------------------------------
// rotate tree to right and use node p as shaft
//--------------------------------------------------
void RBT::rotate_right(Node *p) {
    Node *gp = p->grandparent();
    Node *fa = p->parent;
    Node *y = p->rightTree;
    
    fa->leftTree = y;
    
    if (y != NIL)
        y->parent = fa;
    p->rightTree = fa;
    fa->parent = p;
    
    if (root == fa)
        root = p;
    p->parent = gp;
    
    if (gp != NULL) {
        if (gp->leftTree == fa)
            gp->leftTree = p;
        else
            gp->rightTree = p;
    }    
}

//--------------------------------------------------
// rotate tree to left and use node p as shaft
//--------------------------------------------------
void RBT::rotate_left(Node *p) {
    if (p->parent == NULL) {
        root = p;
        return;
    }
    Node *gp = p->grandparent();
    Node *fa = p->parent;
    Node *y = p->leftTree;
    
    fa->rightTree = y;
    
    if (y != NIL)
        y->parent = fa;
    p->leftTree = fa;
    fa->parent = p;
    
    if (root == fa)
        root = p;
    p->parent = gp;
    
    if (gp != NULL) {
        if (gp->leftTree == fa)
            gp->leftTree = p;
        else
            gp->rightTree = p;
    }
}

//--------------------------------------------------
// find smallest child of current node p
//--------------------------------------------------
Node* RBT::getSmallestChild(Node *p) {
    if (p->leftTree == NIL)
        return p;
    return getSmallestChild(p->leftTree);
}

//--------------------------------------------------
// find largest child of current node p
//--------------------------------------------------
Node* RBT::getLargestChild(Node *p)
{
    if (p->rightTree == NIL)
        return p;
    return getSmallestChild(p->rightTree);
}

//----------------------------------------------------------------------
// traverse all nodes of current tree to get contour in terms of module
//----------------------------------------------------------------------
void RBT::inorder(Node *p) {
    if (p == NIL)
    {
        return;
    }
    if (p->leftTree)
        inorder(p->leftTree);
    
    //*******************Added by Zhenxin*************
    Module_Info m;
    m.name = p->value;
    contour.push_back(m);
    //************************************************
    
    if (p->rightTree)
        inorder(p->rightTree);
}

//----------------------------------------------------------------------
// traverse all nodes of current tree to get contour in terms of cluster
//----------------------------------------------------------------------
void RBT::inorder_clusters_(Node *p){
    if (p == NIL){
        return;
    }
    
    if (p->leftTree)
        inorder_clusters_(p->leftTree);
    
    Cluster m;
    m.setName(p->value);
    clustersContour.push_back(m);
    
    if (p->rightTree)
        inorder_clusters_(p->rightTree);
}

//*******************Added by Zhenxin****************
//-----------------------------------------------------
// get the order of the tree, and store it in a vector
//-----------------------------------------------------
void RBT::search(Node *p){
    if (p == NIL)
    {
        return;
    }
    if (p->leftTree)
        search(p->leftTree);

    order.push_back(*p);
    
    if (p->rightTree)
        search(p->rightTree);
}

//******************Added by Zhenxin*******************
//-----------------------------------------------------
// return a copy of the private attribute contour
//-----------------------------------------------------
vector<Module_Info> RBT::getContour(){
    vector<Module_Info> ms;
    for(int i=0; i<contour.size(); i++){
        if(i>0){
            ms.push_back(contour[i]);
        }
    }
    return ms;
}

//******************Added by Zhenxin*******************
//-----------------------------------------------------
// return a copy of the private attribute clustersContour
//-----------------------------------------------------
vector<Cluster> RBT::get_clt_contour(){
    vector<Cluster> cs;
    for(int i=0; i<clustersContour.size(); i++){
        if(i>0){
            cs.push_back(clustersContour[i]);
        }
    }
    return cs;
}

//******************Added by Zhenxin********************
//-----------------------------------------------------
// get the previous node in the searching order
//-----------------------------------------------------
void RBT::search_last(char data){
    search(root);
    for(int i=0; i<order.size(); i++){
        if (order[i].value==data){
            last = order[i-1];
        }
    }
    order.clear();
}

//---------------------------------------------------------
// traverse tree to check whether node need to be deleted
//----------------------------------------------------------
void RBT::traverseTree(Node *p, char data, stack<Node> *nodes)
{
    if (p == NIL)
        return;
    
    if (p->leftTree)
        traverseTree(p->leftTree, data, nodes);
    
    if(fixed) nodes->push(*p);
    if(p->value == data)	fixed = true;
    
    if (p->rightTree) traverseTree(p->rightTree, data, nodes);
    
}

//-----------------------------------------------------
// delete data from after checking co-ordinations
//-----------------------------------------------------
void RBT::traverseTree_Delete(Node *p, int x_, int y_)
{
    
    if(hv == SS && p->X_ <= x_){
        delete_(p->value);
    }
    
    else if(hv == TT && p->Y_ <= y_){
        delete_(p->value);
    }
}

//-----------------------------------------------------
// delete child of current tree
//-----------------------------------------------------
void RBT::delete_child_(Node *p, char data) {
    if (p->leftTree)
        delete_child_(p->leftTree, data);
    if(p->value == data)
    {
        if (p->rightTree == NIL) {
            delete_one_child_(p);
            return;
        }
        Node *smallest = getSmallestChild(p->rightTree);
        swap(p->value, smallest->value);
        swap(p->X, smallest->X);
        swap(p->Y, smallest->Y);
        swap(p->X_, smallest->X_);
        swap(p->Y_, smallest->Y_);
        swap(p->width, smallest->width);
        swap(p->height, smallest->height);
        delete_one_child_(smallest);
        return;
    }
    if(p->rightTree)
        delete_child_(p->rightTree, data);
}

bool RBT::delete_child(Node *p, char data) {
    if (p->value > data) {
        if (p->leftTree == NIL) {
            return false;
        }
        return delete_child(p->leftTree, data);
    } else if (p->value < data) {
        if (p->rightTree == NIL) {
            return false;
        }
        return delete_child(p->rightTree, data);
    } else {
        if (p->rightTree == NIL) {
            delete_one_child(p);
            return true;
        }
        Node *smallest = getSmallestChild(p->rightTree);
        swap(p->value, smallest->value);
        //swap(p->value, smallest->value);
        swap(p->X, smallest->X);
        swap(p->Y, smallest->Y);
        swap(p->X_, smallest->X_);
        swap(p->Y_, smallest->Y_);
        swap(p->width, smallest->width);
        swap(p->height, smallest->height);
        delete_one_child(smallest);
        
        return true;
    }
}

//----------------------------------------------------------------
// delete one child of current tree
//----------------------------------------------------------------
void RBT::delete_one_child_(Node *p) {
    Node *child = p->leftTree == NIL ? p->rightTree : p->leftTree;
    if (p->parent == NULL && p->leftTree == NIL && p->rightTree == NIL) {
        p = NULL;
        root = p;
        return;
    }
    
    if (p->parent == NULL) {
        delete  p;
        child->parent = NULL;
        root = child;
        root->color = BLACK;
        return;
    }
    
    if (p->parent->leftTree == p) {
        p->parent->leftTree = child;
    } else {
        p->parent->rightTree = child;
    }
    child->parent = p->parent;
    
    if (p->color == BLACK) {
        if (child->color == RED) {
            child->color = BLACK;
        } else
            delete_case(child);
    }
    delete p;
}

void RBT::delete_one_child(Node *p) {
    Node *child = p->leftTree == NIL ? p->rightTree : p->leftTree;
    if (p->parent == NULL && p->leftTree == NIL && p->rightTree == NIL) {
        p = NULL;
        root = p;
        return;
    }
    
    if (p->parent == NULL) {
        delete  p;
        child->parent = NULL;
        root = child;
        root->color = BLACK;
        return;
    }
    
    if (p->parent->leftTree == p) {
        p->parent->leftTree = child;
    } else {
        p->parent->rightTree = child;
    }
    child->parent = p->parent;
    
    if (p->color == BLACK) {
        if (child->color == RED) {
            child->color = BLACK;
        } else
            delete_case(child);
    }
    delete p;
}

//------------------------------------------------------------
// checking whether tree need extra operations after insertion
//------------------------------------------------------------
void RBT::insert_case(Node *p) {
    if (p->parent == NULL) {
        root = p;
        p->color = BLACK;
        return;
    }
    if (p->parent->color == RED) {
        if (p->uncle()->color == RED) {
            p->parent->color = p->uncle()->color = BLACK;
            p->grandparent()->color = RED;
            insert_case(p->grandparent());
        } else {
            if (p->parent->rightTree == p && p->grandparent()->leftTree == p->parent) {
                rotate_left(p);
                rotate_right(p);
                p->color = BLACK;
                p->leftTree->color = p->rightTree->color = RED;
            } else if (p->parent->leftTree == p && p->grandparent()->rightTree == p->parent) {
                rotate_right(p);
                rotate_left(p);
                p->color = BLACK;
                p->leftTree->color = p->rightTree->color = RED;
            } else if (p->parent->leftTree == p && p->grandparent()->leftTree == p->parent) {
                p->parent->color = BLACK;
                p->grandparent()->color = RED;
                rotate_right(p->parent);
            } else if (p->parent->rightTree == p && p->grandparent()->rightTree == p->parent) {
                p->parent->color = BLACK;
                p->grandparent()->color = RED;
                rotate_left(p->parent);
            }
        }
    }
}

//-------------------------------------------------------------
// Check current tree, whether new tree need to delete node p.
//-------------------------------------------------------------
void RBT::delete_case(Node *p) {
    if (p->parent == NULL) {
        p->color = BLACK;
        return;
    }
    if (p->sibling()->color == RED) {
        p->parent->color = RED;
        p->sibling()->color = BLACK;
        if (p == p->parent->leftTree)
            rotate_left(p->sibling());
        else
            rotate_right(p->sibling());
    }
    if (p->parent->color == BLACK && p->sibling()->color == BLACK
        && p->sibling()->leftTree->color == BLACK && p->sibling()->rightTree->color == BLACK) {
        p->sibling()->color = RED;
        delete_case(p->parent);
    } else if (p->parent->color == RED && p->sibling()->color == BLACK
               && p->sibling()->leftTree->color == BLACK && p->sibling()->rightTree->color == BLACK) {
        p->sibling()->color = RED;
        p->parent->color = BLACK;
    } else {
        if (p->sibling()->color == BLACK) {
            if (p == p->parent->leftTree && p->sibling()->leftTree->color == RED
                && p->sibling()->rightTree->color == BLACK) {
                p->sibling()->color = RED;
                p->sibling()->leftTree->color = BLACK;
                rotate_right(p->sibling()->leftTree);
            } else if (p == p->parent->rightTree && p->sibling()->leftTree->color == BLACK
                       && p->sibling()->rightTree->color == RED) {
                p->sibling()->color = RED;
                p->sibling()->rightTree->color = BLACK;
                rotate_left(p->sibling()->rightTree);
            }
        }
        p->sibling()->color = p->parent->color;
        p->parent->color = BLACK;
        if (p == p->parent->leftTree) {
            p->sibling()->rightTree->color = BLACK;
            rotate_left(p->sibling());
        } else {
            p->sibling()->leftTree->color = BLACK;
            rotate_right(p->sibling());
        }
    }
}

//-----------------------------------------------------
// Used for normal RBT, insert a new node
//-----------------------------------------------------
void RBT::insert(Node *p, char data) {
    if (p->value >= data) {
        if (p->leftTree != NIL)
            insert(p->leftTree, data);
        else {
            Node *tmp = new Node();
            tmp->value = data;
            tmp->leftTree = tmp->rightTree = NIL;
            tmp->parent = p;
            p->leftTree = tmp;
            insert_case(tmp);
        }
    } else {
        if (p->rightTree != NIL)
            insert(p->rightTree, data);
        else {
            Node *tmp = new Node();
            tmp->value = data;
            tmp->leftTree = tmp->rightTree = NIL;
            tmp->parent = p;
            p->rightTree = tmp;
            insert_case(tmp);
        }
    }
}

//*************************Added by Zhenxin****************************
//------------------------------------------------------------------------------------
// Get the location relationship between a node and the currently insert node
// Node *p as parent of new node, Module_Info *m as module information, Hv_Matrix *matrix 
// is a triangle matrix which store the location relationship between modules
//------------------------------------------------------------------------------------
int RBT::getRelation(Node *p, Module_Info *m, Hv_Matrix *matrix){
    
    int i = p->lc;
    int j = m->lc;
    
    if (matrix->get_edge_type_c(i, j) == 'H') {
        return 1;
    } else{
        return 0;
    }
}

//*************************Added by Zhenxin****************************
//------------------------------------------------------------------------------------
// get the location relationship between a node and the currently insert node
// Node *p as parent of new node, Module_Info *m as module information, Hv_Matrix *matrix
//is a triangle matrix which store the location relationship between modules
//------------------------------------------------------------------------------------
int RBT::getRelationc(Node *p, Cluster *c, Hv_Matrix *matrix){
    
    int i = p->lc;
    int j = c->getLc();
    
    if (matrix->get_edge_type_c(i, j) == 'H') {
        return 1;
    } else{
        return 0;
    }
}

//**************************Modified by Zhenxin*************************
//*------------------------------------------------------------------------------------
// Used for normal module packing RBT, insert new node(module)
// Node *p as parent of new node, Module_Info *m as module information, Hv_Matrix *matrix
// is a triangle matrix which store the location relationship between modules
//------------------------------------------------------------------------------------
void RBT::insert_rb_(Node *p, Module_Info *m, Hv_Matrix *matrix){
    if(hv == SS)
    {
        if(p->value == 's')//first
        {
            if(p->rightTree != NIL)	insert_rb_(p->rightTree, m, matrix);
            else{
                Node *tmp = new Node();
                tmp->value = m->name;
                tmp->lc = m->lc;
                tmp->leftTree = tmp->rightTree = NIL;
                tmp->parent = p;
                p->rightTree = tmp;
                search_last(tmp->value);
                tmp->width = m->width; tmp->height = m->height;
                m->x = tmp->X = last.X_;
                m->rx = tmp->X_ = tmp->X + m->width;
                insert_case(tmp);
                fixed = false;
                stack<Node> nodes;
                traverseTree(tmp->value, &nodes);
                while(!nodes.empty())
                {
                    traverseTree_Delete(&(nodes.top()), tmp->X_, tmp->Y_);
                    nodes.pop();
                }
                return;
            }
        }
        else
        {
            if (getRelation(p, m, matrix))
            {
                if(p->rightTree != NIL)  insert_rb_(p->rightTree, m, matrix);
                else
                {
                    Node *tmp = new Node();
                    tmp->value = m->name;
                    tmp->lc = m->lc;
                    tmp->leftTree = tmp->rightTree = NIL;
                    tmp->parent = p;
                    p->rightTree = tmp;
                    search_last(tmp->value);
                    tmp->width = m->width; tmp->height = m->height;
                    m->x = tmp->X = last.X_;
                    m->rx = tmp->X_ = tmp->X + m->width;
                    insert_case(tmp);
                    fixed = false;
                    stack<Node> nodes;
                    traverseTree(tmp->value, &nodes);
                    while(!nodes.empty())
                    {
                        traverseTree_Delete(&(nodes.top()), tmp->X_, tmp->Y_);
                        nodes.pop();
                    }
                    return;
                }
            }
            else
            {
                if(p->leftTree != NIL)  insert_rb_(p->leftTree, m, matrix);
                else
                {
                    Node *tmp = new Node();
                    tmp->value = m->name;
                    tmp->lc = m->lc;
                    tmp->leftTree = tmp->rightTree = NIL;
                    tmp->parent = p;
                    p->leftTree = tmp;
                    search_last(tmp->value);
                    tmp->width = m->width; tmp->height = m->height;
                    m->x = tmp->X = last.X_;
                    m->rx = tmp->X_ = tmp->X + m->width;
                    insert_case(tmp);
                    fixed = false;
                    stack<Node> nodes;
                    traverseTree(tmp->value, &nodes);
                    while(!nodes.empty())
                    {
                        traverseTree_Delete(&(nodes.top()), tmp->X_, tmp->Y_);
                        nodes.pop();
                    }
                    return;
                }
            }
        }
    }
    
    if(hv == TT)
    {
        if(p->value == 't')//first
        {
            if(p->rightTree != NIL)	insert_rb_(p->rightTree, m, matrix);
            else{
                Node *tmp = new Node();
                tmp->value = m->name;
                tmp->lc = m->lc;
                tmp->leftTree = tmp->rightTree = NIL;
                tmp->parent = p;
                p->rightTree = tmp;
                search_last(tmp->value);
                tmp->width = m->width; tmp->height = m->height;
                m->y = tmp->Y = last.Y_;
                m->ry = tmp->Y_ = tmp->Y + m->height;
                insert_case(tmp);
                fixed = false;
                stack<Node> nodes;
                traverseTree(tmp->value, &nodes);
                while(!nodes.empty())
                {
                    traverseTree_Delete(&(nodes.top()), tmp->X_, tmp->Y_);
                    nodes.pop();
                }
                return;
            }
        }
        else
        {
            if (getRelation(p, m, matrix))
            {
                if(p->leftTree != NIL)  insert_rb_(p->leftTree, m, matrix);
                else
                {
                    Node *tmp = new Node();
                    tmp->value = m->name;
                    tmp->lc = m->lc;
                    tmp->leftTree = tmp->rightTree = NIL;
                    tmp->parent = p;
                    p->leftTree = tmp;
                    search_last(tmp->value);
                    tmp->width = m->width; tmp->height = m->height;
                    m->y = tmp->Y = last.Y_;
                    m->ry = tmp->Y_ = tmp->Y + m->height;
                    insert_case(tmp);
                    fixed = false;
                    stack<Node> nodes;
                    traverseTree(tmp->value, &nodes);
                    while(!nodes.empty())
                    {
                        traverseTree_Delete(&(nodes.top()), tmp->X_, tmp->Y_);
                        nodes.pop();
                    }
                    return;
                }
            }
            else
            {
                if(p->rightTree != NIL)  insert_rb_(p->rightTree, m, matrix);
                else
                {
                    Node *tmp = new Node();
                    tmp->value = m->name;
                    tmp->lc = m->lc;
                    tmp->leftTree = tmp->rightTree = NIL;
                    tmp->parent = p;
                    p->rightTree = tmp;
                    search_last(tmp->value);
                    tmp->width = m->width; tmp->height = m->height;
                    m->y = tmp->Y = last.Y_;
                    m->ry = tmp->Y_ = tmp->Y + m->height;
                    insert_case(tmp);
                    fixed = false;
                    stack<Node> nodes;
                    traverseTree(tmp->value, &nodes);
                    while(!nodes.empty())
                    {
                        traverseTree_Delete(&(nodes.top()), tmp->X_, tmp->Y_);
                        nodes.pop();
                    }
                    return;
                }
            }
        }
    }
}

//*******************************Added by Zhenxin*****************************
//-----------------------------------------------------------------------------------------
// Used for normal module packing RBT, insert new node(module)
// Node *p as parent of new node, Module_Info *m as module information, Hv_Matrix *matrix
//is a triangle matrix which store the location relationship between modules
//-----------------------------------------------------------------------------------------
void RBT::insert_cluster_(Node *p, Cluster *c, Hv_Matrix *matrix){
    if(hv == SS)
    {
        if(p->value == 's')//first
        {
            if(p->rightTree != NIL)	insert_cluster_(p->rightTree, c, matrix);
            else{
                Node *tmp = new Node();
                tmp->value = c->getName();
                tmp->lc = c->getLc();
                tmp->leftTree = tmp->rightTree = NIL;
                tmp->parent = p;
                p->rightTree = tmp;
                search_last(tmp->value);
                tmp->width = c->getWidth(); tmp->height = c->getHeight();
                tmp->X = last.X_;
                c->setX(tmp->X);
                tmp->X_ = tmp->X + c->getWidth();
                c->setX_(tmp->X_);
                insert_case(tmp);
                fixed = false;
                stack<Node> nodes;
                traverseTree(tmp->value, &nodes);
                while(!nodes.empty())
                {
                    traverseTree_Delete(&(nodes.top()), tmp->X_, tmp->Y_);
                    nodes.pop();
                }
                return;
            }
        }
        else
        {
            if (getRelationc(p, c, matrix))
            {
                if(p->rightTree != NIL)  insert_cluster_(p->rightTree, c, matrix);
                else
                {
                    Node *tmp = new Node();
                    tmp->value = c->getName();
                    tmp->lc = c->getLc();
                    tmp->leftTree = tmp->rightTree = NIL;
                    tmp->parent = p;
                    p->rightTree = tmp;
                    search_last(tmp->value);
                    tmp->width = c->getWidth(); tmp->height = c->getHeight();
                    tmp->X = last.X_;
                    c->setX(tmp->X);
                    tmp->X_ = tmp->X + c->getWidth();
                    c->setX_(tmp->X_);
                    insert_case(tmp);
                    fixed = false;
                    stack<Node> nodes;
                    traverseTree(tmp->value, &nodes);
                    while(!nodes.empty())
                    {
                        traverseTree_Delete(&(nodes.top()), tmp->X_, tmp->Y_);
                        nodes.pop();
                    }
                    return;
                }
            }
            else
            {
                if(p->leftTree != NIL)  insert_cluster_(p->leftTree, c, matrix);
                else
                {
                    Node *tmp = new Node();
                    tmp->value = c->getName();
                    tmp->lc = c->getLc();
                    tmp->leftTree = tmp->rightTree = NIL;
                    tmp->parent = p;
                    p->leftTree = tmp;
                    search_last(tmp->value);
                    tmp->width = c->getWidth(); tmp->height = c->getHeight();
                    tmp->X = last.X_;
                    c->setX(tmp->X);
                    tmp->X_ = tmp->X + c->getWidth();
                    c->setX_(tmp->X_);
                    insert_case(tmp);
                    fixed = false;
                    stack<Node> nodes;
                    traverseTree(tmp->value, &nodes);
                    while(!nodes.empty())
                    {
                        traverseTree_Delete(&(nodes.top()), tmp->X_, tmp->Y_);
                        nodes.pop();
                    }
                    return;
                }
            }
        }
    }
    
    if(hv == TT)
    {
        if(p->value == 't')//first
        {
            if(p->rightTree != NIL)	insert_cluster_(p->rightTree, c, matrix);
            else{
                Node *tmp = new Node();
                tmp->value = c->getName();
                tmp->lc = c->getLc();
                tmp->leftTree = tmp->rightTree = NIL;
                tmp->parent = p;
                p->rightTree = tmp;
                search_last(tmp->value);
                tmp->width = c->getWidth(); tmp->height = c->getHeight();
                tmp->Y = last.Y_;
                c->setY(tmp->Y);
                tmp->Y_ = tmp->Y + c->getHeight();
                c->setY_(tmp->Y_);
                
                insert_case(tmp);
                fixed = false;
                stack<Node> nodes;
                traverseTree(tmp->value, &nodes);
                while(!nodes.empty())
                {
                    traverseTree_Delete(&(nodes.top()), tmp->X_, tmp->Y_);
                    nodes.pop();
                }
                return;
            }
        }
        else
        {
            if (getRelationc(p, c, matrix))
            {
                if(p->leftTree != NIL)  insert_cluster_(p->leftTree, c, matrix);
                else
                {
                    Node *tmp = new Node();
                    tmp->value = c->getName();
                    tmp->lc = c->getLc();
                    tmp->leftTree = tmp->rightTree = NIL;
                    tmp->parent = p;
                    p->leftTree = tmp;
                    search_last(tmp->value);
                    tmp->width = c->getWidth(); tmp->height = c->getHeight();
                    tmp->Y = last.Y_;
                    c->setY(tmp->Y);
                    tmp->Y_ = tmp->Y + c->getHeight();
                    c->setY_(tmp->Y_);
                    insert_case(tmp);
                    fixed = false;
                    stack<Node> nodes;
                    traverseTree(tmp->value, &nodes);
                    while(!nodes.empty())
                    {
                        traverseTree_Delete(&(nodes.top()), tmp->X_, tmp->Y_);
                        nodes.pop();
                    }
                    return;
                }
            }
            else
            {
                if(p->rightTree != NIL)  insert_cluster_(p->rightTree, c, matrix);
                else
                {
                    Node *tmp = new Node();
                    tmp->value = c->getName();
                    tmp->lc = c->getLc();
                    tmp->leftTree = tmp->rightTree = NIL;
                    tmp->parent = p;
                    p->rightTree = tmp;
                    search_last(tmp->value);
                    tmp->width = c->getWidth(); tmp->height = c->getHeight();
                    tmp->Y = last.Y_;
                    c->setY(tmp->Y);
                    tmp->Y_ = tmp->Y + c->getHeight();
                    c->setY_(tmp->Y_);
                    insert_case(tmp);
                    fixed = false;
                    stack<Node> nodes;
                    traverseTree(tmp->value, &nodes);
                    while(!nodes.empty())
                    {
                        traverseTree_Delete(&(nodes.top()), tmp->X_, tmp->Y_);
                        nodes.pop();
                    }
                    return;
                }
            }
        }
    }
}

//-----------------------------------------------------
// delete tree with p is root
// Node *p as root of subtree which need to be deleted
//-----------------------------------------------------
void RBT::DeleteTree(Node *p) {
    if (!p || p == NIL) {
        return;
    }
    DeleteTree(p->leftTree);
    DeleteTree(p->rightTree);
    delete p;
}

//-----------------------------------------------------
// traverse tree from outside
//-----------------------------------------------------
void RBT::inorder() {
    if (root == NULL)
        return;
    inorder(root);
}

void RBT::inorder_clusters(){
    if (root == NULL)
        return;
    inorder_clusters_(root);
}

//-----------------------------------------------------
// first step for construct a RBT.
// char x as value of root node of tree
//-----------------------------------------------------
void RBT::insertHead(char x){
    if (root == NULL) {
        root = new Node();
        root->color = BLACK;
        root->leftTree = root->rightTree = NIL;
        root->value = x;
        if(root->value == 's')
        {
            hv = SS;
            root->X = root->Y = 0;
            root->X_ = 0; root->Y_ = INFI;
        }
        else if(root->value == 't')
        {
            hv = TT;
            root->X = root->Y = 0;
            root->X_ = INFI; root->Y_ = 0;
        }
    } 
}

//-----------------------------------------------------------------------------
// insert node from outside
// module *m as information of inserted module, Hv_Matrix matrix as information
// of relationship between clusters
//-----------------------------------------------------------------------------
void RBT::insert_(Module_Info *m, Hv_Matrix *matrix){
    if (root == NULL) {
        root = new Node();
        root->color = BLACK;
        root->leftTree = root->rightTree = NIL;
        root->value = m->name;
        if(root->value == 's')
        {
            hv = SS;
            root->X = root->Y = 0;
            root->X_ = 0; root->Y_ = INFI;
        }
        else if(root->value == 't')
        {
            hv = TT;
            root->X = root->Y = 0;
            root->X_ = INFI; root->Y_ = 0;
        }
    } else {
        insert_rb_(root, m, matrix);
    }
}

//********************************Added by Zhenxin*******************************
//------------------------------------------------------------------------------
// insert cluster from outside
// cluster c as information of inserted cluster, Hv_Matrix matrix as information
// of relationship between clusters
//------------------------------------------------------------------------------
void RBT::insert_cluster(Cluster *c, Hv_Matrix *matrix){
    if (root == NULL) {
        root = new Node();
        root->color = BLACK;
        root->leftTree = root->rightTree = NIL;
        root->value = c->getName();
        if(root->value == 's')
        {
            hv = SS;
            root->X = root->Y = 0;
            root->X_ = 0; root->Y_ = INFI;
        }
        else if(root->value == 't')
        {
            hv = TT;
            root->X = root->Y = 0;
            root->X_ = INFI; root->Y_ = 0;
        }
    } else {
        insert_cluster_(root, c, matrix);
    }
}

//-------------------------------------------------------------------
// traverse the tree for deletion check from outside
// char data as value of checking destination node, int x_ and
// y_ as co-ordinations of module, stack node used to store nodes need to deleted
//-------------------------------------------------------------------
void RBT::traverseTree(char data, stack<Node> *nodes){
        if(root == NULL)
            return;
        else
            traverseTree(root, data, nodes);
}
    
//-------------------------------------------------------------------
// delete node from tree
// char data is value of node which need to be deleted
//-------------------------------------------------------------------
void RBT::delete_(char data) {
        if(root == NULL)
            return;
        delete_child_(root, data);
}
    
//-------------------------------------------------------------------
// delete part of tree from outside
// char data is value of node need to be deleted
//-------------------------------------------------------------------
bool RBT::delete_value(char data) {
        return delete_child(root, data);
}



