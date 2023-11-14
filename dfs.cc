#include "dfs.h"

void directed_dfs(DirectedGraph & g) {
    // prologue
    int n = g.getSize();
    int ** paths = new int*[n];
    int * roots = new int[n];
    int numRoots;
    int ** parents = new int*[n];
    int * numParents = new int[n];
    for (int i = 0; i < n; i++) {
        paths[i] = new int[n];
        for (int j = 0; j < n; j++) {
            paths[i][j] = -1;
        }
        parents[i] = new int[n];
    }
    g.findParents(parents, numParents);
    g.findRoots(roots, &numRoots);

    // main body
    computeDFSTree(g, paths, roots, numRoots);


    // epilogue
    for (int i = 0; i < n; i++) {
        delete [] paths[i];
        delete [] parents[i];
    }
    delete [] paths;
    delete [] parents;

}

// Algorithm 4 from the paper
void computeDFSTree(DirectedGraph & g, int ** paths, int * roots, int & numRoots) {
    std::queue<int> Q;
    for (int i = 0; i < numRoots; i++) {
        Q.push(roots[i]);
    }
    while (!Q.empty()) {
        std::queue<int> P;
    }
}

// Algorithm 2 from the paper
void computeEdgeWeights() {

}

// Algorithm 3 from the paper
void computePreAndPostOrders() {
    
}
