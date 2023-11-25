#include <queue>
#include <algorithm>
#include <mutex>
#include <thread>
#include <cstring>
#include <deque>
#include <sstream>
#include <iostream>

#ifndef _GRAPH_IMPORT
#define _GRAPH_IMPORT
#include "graph.h"
#endif

class parallel_dfs {
    // supplied at initialization
    std::ostream & out;
    DirectedGraph & graph;
    int n;

    // computed as part of initialization
    int * roots;
    int numRoots;
    int * parentsVisited;
    int ** parents;
    int * numParents;
    int ** children;
    int * numChildren;

    // computed by dfs helper algorithms
    int * firstAncestor;
    int ** paths;
    int * pathLengths;
    int * edgeWeights;
    int * preOrder;
    int * postOrder;

    void getRootOrder();
    void computeDFSTree();
    void computeEdgeWeights();
    void computePreAndPostOrders();
    bool isRoot(int node);

    public:
        ~parallel_dfs();
        parallel_dfs(DirectedGraph & g, std::ostream & out);
        void directed_dfs();
};
