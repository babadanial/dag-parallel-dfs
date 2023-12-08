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
    std::mutex printLock;

    // computed as part of initialization
    int * roots;
    int numRoots;
    int ** parents;
    int * numParents;
    int ** children;
    int * numChildren;

    // computed by dfs helper algorithms
    //  stores root that is the original ancestor of each node
    int * rootAncestor;
    //  stores the immediate parent of each node, depending on what order we perform DFS on the roots
    int * parent;
    int ** paths;
    int * pathLengths;
    int * edgeWeights;
    int * preOrder;
    int * postOrder;

    void getRootOrder();
    void computeDFSTree(int root);
    void computeEdgeWeights();
    void computePreAndPostOrders();
    bool isRoot(int node);

    public:
        ~parallel_dfs();
        parallel_dfs(DirectedGraph & g, std::ostream & out);
        void directed_dfs();
        friend std::ostream &operator<<(std::ostream &out, const parallel_dfs & dfs_obj);
};
