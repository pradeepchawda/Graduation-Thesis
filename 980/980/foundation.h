//
//  foundation.h
//  980MergeSA2
//
//  Created by Hensin on 2016-04-03.
//  Copyright (c) 2016 Hensin. All rights reserved.
//

#ifndef _80MergeSA2_foundation_h
#define _80MergeSA2_foundation_h

#include <boost/utility.hpp>                // for boost::tie
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

using namespace boost;

struct VertexProperty {
    unsigned int index;
};

struct EdgeProperty {
    int flag;
};

typedef adjacency_list<vecS, vecS, bidirectionalS, VertexProperty, no_property> Graph;

typedef graph_traits<Graph>::vertex_descriptor G_node;
typedef graph_traits<Graph>::edge_descriptor G_edge;

#endif
