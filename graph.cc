#include "graph.h"

using namespace std;

std::ostream &operator<<(std::ostream &out, const Graph &g) {
    int n = g.n;
    for (int node = 0; node < n; node++) {
        out << "Neighbours of node " << node << ": ";
        for (int neighbour = 0; neighbour < n; neighbour++) {
            out << g.adjacencyList[node][neighbour] << " ";
        }
        out << endl;
    }
    return out;
}

Graph::Graph(int ** adjacencyList, int n, bool directed)
    : n{n},
      directed{directed},
      adjacencyMatrix{nullptr}
{
    // copy in adjacency lists from caller to Graph instance
    this->adjacencyList = new int*[n];
    for (int i = 0; i < n; i++) {
        this->adjacencyList[i] = new int[n];
        memcpy(this->adjacencyList[i], adjacencyList[i], n * sizeof(int));
    }
}

int Graph::adjacencyMatrixElement(int row, int col) {
    return this->adjacencyMatrix[row][col];
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
}

DirectedGraph::DirectedGraph(int ** adjacencyList, int n, bool directed) 
    : Graph{adjacencyList, n, directed} 
{
    this->buildAdjacencyMatrixParallel();
}