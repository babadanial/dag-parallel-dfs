#include <queue>
#include <algorithm>
#include <mutex>
#include <thread>
#include <deque>

#ifndef _GRAPH_IMPORT
#define _GRAPH_IMPORT
#include "graph.h"
#endif

class parallel_dfs {
    DirectedGraph & graph;
    int n;
    int * roots;
    int numRoots;
    bool ** parentVisited;
    int * parentVisitedIndex;
    int ** parents;
    int * numParents;
    int ** children;
    int * numChildren;
    int ** paths;
    int * pathLengths;
    int * edgeWeights;

    void computeDFSTree();
    void computeEdgeWeights();
    void computePreAndPostOrders();

    public:
        ~parallel_dfs();
        parallel_dfs(DirectedGraph & g);
        void directed_dfs();
};
