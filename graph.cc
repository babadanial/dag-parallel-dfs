#include "graph.h"

using namespace std;

std::ostream &operator<<(std::ostream &out, const Graph &g) {
    int n = g.n;
    bool directed = g.directed;
    out << endl;
    for (int node = 0; node < n; node++) {
        if (directed) {
            out << "Out-neighbours of node " << node << ": ";
        } else {
            out << "Neighbours of node " << node << ": ";
        }
        
        for (int neighbour = 0; neighbour < n; neighbour++) {
            out << g.adjacencyList[node][neighbour] << " ";
        }
        out << endl;
    }
    out << "Adjacency matrix:" << endl;
    for (int node = 0; node < n; node++) {
        out << "Row " << node << ": ";
        for (int adjMatrixCol = 0; adjMatrixCol < n; adjMatrixCol++) {
            out << g.adjacencyMatrix[node][adjMatrixCol] << " ";
        }
        out << endl;
    }
    return out;
}

Graph::Graph(int ** adjacencyList, int * adjacencyListLength, int n, bool directed)
    : n{n},
      directed{directed},
      adjacencyMatrix{nullptr}
{
    // copy in adjacency lists from caller to Graph instance
    this->adjacencyList = new int*[n];
    this->adjacencyListLength = new int[n];
    for (int i = 0; i < n; i++) {
        this->adjacencyListLength[i] = adjacencyListLength[i];
        this->adjacencyList[i] = new int[n];
        memcpy(this->adjacencyList[i], adjacencyList[i], n * sizeof(int));
    }
}

int Graph::adjacencyMatrixElement(int row, int col) {
    return this->adjacencyMatrix[row][col];
}

Graph::~Graph() {
    for (int i = 0; i < n; i++) {
        delete [] this->adjacencyList[i];
    }
    delete [] this->adjacencyList;
}

DirectedGraph::~DirectedGraph() {
    for (int i = 0; i < n; i++) {
        delete [] adjacencyMatrix[i];
        delete [] parents[i];
    }
    delete [] leaves;
    delete [] roots;
    delete [] numParents;
    delete [] parents;
    delete [] adjacencyMatrix;
}

void DirectedGraph::buildAdjacencyMatrixSequential() {
    // allocate adjacency matrix and zero-initialize it
    this->adjacencyMatrix = new int*[n];
    for (int i = 0; i < n; i++) {
        this->adjacencyMatrix[i] = new int[n];
        for (int j = 0; j < n; j++) {
            this->adjacencyMatrix[i][j] = 0;
        }
    }

    // iterate through adjacency list to build adjacency matrix
    for (int node = 0; node < n; node++) {
        for (int neighbourIndex = 0; neighbourIndex < n; neighbourIndex++) {
            int neighbour = this->adjacencyList[node][neighbourIndex];
            if (neighbour == -1) { 
                // we've reached the end of the out-neighbour list for this node, 
                //  move on to next one
                break; 
            }
            this->adjacencyMatrix[node][neighbour] = -1;
            this->adjacencyMatrix[neighbour][node] = 1;
        }
    }
}

void DirectedGraph::buildAdjacencyMatrixParallel() {
    // allocate adjacency matrix and zero-initialize it
    this->adjacencyMatrix = new int*[n];

    // lambda object for use in initialization
    auto initialize = [&](int row, int n) {
        for (int i = 0; i < n; i++) {
            this->adjacencyMatrix[row][i] = 0;
        }
    };

    thread * threads = new thread[n];
    for (int i = 0; i < n; i++) {
        this->adjacencyMatrix[i] = new int[n];
        threads[i] = thread(initialize, i, n);
    }
    for (int i = 0; i < n; i++) {
        threads[i].join();
    }        
    // at this point, all threads have completed

    auto processAdjacencyList = [&](int node, int n) {
        for (int neighbourIndex = 0; neighbourIndex < n; neighbourIndex++) {
            int neighbour = this->adjacencyList[node][neighbourIndex];
            if (neighbour == -1) {
                // we've reached the end of the out-neighbour list for this node, 
                //  move on to next one
                break;
            }
            this->adjacencyMatrix[node][neighbour] = -1;
            this->adjacencyMatrix[neighbour][node] = 1;
        }
    };

    // iterate through adjacency list to build adjacency matrix
    for (int node = 0; node < n; node++) {
        threads[node] = thread(processAdjacencyList, node, n);
    }
    for (int i = 0; i < n; i++) {
        threads[i].join();
    }    
    // at this point, all threads have completed
    delete [] threads;
}

int Graph::getSize() {
    return this->n;
}

DirectedGraph::DirectedGraph(int ** adjacencyList, int * adjacencyListLength, int n, bool directed) 
    : Graph{adjacencyList, adjacencyListLength, n, directed},
      leavesFound{false},
      rootsFound{false},
      parentsFound{false},
      numLeaves{-1},
      numRoots{-1},
      numParents{nullptr},
      roots{nullptr},
      leaves{nullptr},
      parents{nullptr}
{
    this->buildAdjacencyMatrixParallel();
}

void DirectedGraph::getChildren(int ** children, int * numChildren) {
    for (int node = 0; node < n; node++) {
        memcpy(children[node], this->adjacencyList[node], n * sizeof(int));
    }
    memcpy(numChildren, this->adjacencyListLength, n * sizeof(int));
}

void DirectedGraph::findRoots(int * roots, int * numRoots) {
    if (rootsFound) {
        *numRoots = this->numRoots;
        memcpy(roots, this->roots, this->numRoots * sizeof(int));
        return;
    }

    mutex rootMutex;
    this->numRoots = 0;
    this->roots = new int[n];

    auto checkIfRoot = [&](int && node) {
        for (int adjMatrixCol = 0; adjMatrixCol < n; adjMatrixCol++) {
            if (this->adjacencyMatrix[node][adjMatrixCol] == 1) {
                return;
            }
        }
        // if we made it here, then no value in the row was 1
        //  so the node is a root
        rootMutex.lock();
        this->roots[this->numRoots] = node;
        this->numRoots++;
        rootMutex.unlock();
    };

    thread * threads = new thread[n];
    for (int node = 0; node < this->n; node++) {
        threads[node] = thread(checkIfRoot, node);
    }
    for (int i = 0; i < n; i++) {
        threads[i].join();
    }

    // at this point, all threads have completed
    this->rootsFound = true;
    *numRoots = this->numRoots;
    memcpy(roots, this->roots, this->numRoots * sizeof(int));
    delete [] threads;
}

void DirectedGraph::findLeaves(int * leaves, int * numLeaves) {
    if (leavesFound) {
        *numLeaves = this->numLeaves;
        memcpy(leaves, this->leaves, this->numLeaves * sizeof(int));
        return;
    }

    mutex leafMutex;
    this->numLeaves = 0;
    this->leaves = new int[n];

    auto checkIfLeaf = [&](int && node) {
        for (int adjMatrixCol = 0; adjMatrixCol < n; adjMatrixCol++) {
            if (this->adjacencyMatrix[node][adjMatrixCol] == -1) {
                return;
            }
        }
        // if we made it here, then no value in the row was 1
        //  so the node is a root
        leafMutex.lock();
        this->leaves[this->numLeaves] = node;
        this->numLeaves++;
        leafMutex.unlock();
    };

    thread * threads = new thread[n];
    for (int node = 0; node < this->n; node++) {
        threads[node] = thread(checkIfLeaf, node);
    }
    for (int i = 0; i < n; i++) {
        threads[i].join();
    }

    // at this point, all threads have completed
    this->leavesFound = true;
    *numLeaves = this->numLeaves;
    memcpy(leaves, this->leaves, this->numLeaves * sizeof(int));
    delete [] threads;
}

void DirectedGraph::findParents(int ** parents, int * numParents) {
    if (parentsFound) {
        memcpy(numParents, this->numParents, this->n * sizeof(int));
        for (int node = 0; node < n; node++) {
            numParents[node] = this->numParents[node];
            memcpy(parents[node], this->parents[node], this->numParents[node] * sizeof(int));
        }
        return;
    }

    this->parents = new int*[n];
    for (int i = 0; i < n; i++) {
        this->parents[i] = new int[n];
        for (int j = 0; j < n; j++) {
            this->parents[i][j] = -1;
        }
    }
    this->numParents = new int[n];

    auto findParents = [&](int && node) {
        int i = 0;
        for (int adjMatrixCol = 0; adjMatrixCol < n; adjMatrixCol++) {
            if (this->adjacencyMatrix[node][adjMatrixCol] == 1) {
                this->parents[node][i] = adjMatrixCol;
                i++;
            }
        }
        this->numParents[node] = i;
    };

    thread * threads = new thread[n];
    for (int node = 0; node < this->n; node++) {
        threads[node] = thread(findParents, node);
    }
    for (int i = 0; i < n; i++) {
        threads[i].join();
    }

    // at this point, all threads have completed
    this->parentsFound = true;
    memcpy(numParents, this->numParents, this->n * sizeof(int));
    for (int node = 0; node < n; node++) {
        numParents[node] = this->numParents[node];
        memcpy(parents[node], this->parents[node], this->numParents[node] * sizeof(int));
    }
    delete [] threads;
}