#include <iostream>
#include <ostream>
#include <thread>
#include <pthread.h>
#include <mutex>
#include <cstring>

class Graph {
    protected:
        // number of nodes in graph
        const int n;
        const bool directed;
        int ** adjacencyList;
        int * adjacencyListLength;
        int ** adjacencyMatrix;

        Graph(int ** adjacencyList, int * adjacencyListLength, int n, bool directed);
        virtual ~Graph();

    public:
        int getSize();
        virtual void buildAdjacencyMatrixSequential() = 0;
        virtual void buildAdjacencyMatrixParallel() = 0;
        friend std::ostream &operator<<(std::ostream &out, const Graph &g);
        int adjacencyMatrixElement(int row, int col);
};

class DirectedGraph : public Graph {
    void buildAdjacencyMatrixSequential();
    void buildAdjacencyMatrixParallel();

    // roots - no in-neighbours
    bool rootsFound;
    int * roots;
    int numRoots;

    // leaves - no out-neighbours
    bool leavesFound;
    int * leaves;
    int numLeaves;

    // sets of in-neighbours for each node
    bool parentsFound;
    int ** parents;
    // counts of in-neighbours for each node 
    //  i.e. numParents[i] = length of parents[i]
    int * numParents;

    public:
        DirectedGraph(int ** adjacencyList, int * adjacencyListLength, int n, bool directed);
        virtual ~DirectedGraph();
        void getChildren(int ** children, int * numChildren);

        // for the following 2 methods, first argument must be an int array of size n
        void findRoots(int * roots, int * numRoots);
        void findLeaves(int * leaves, int * numLeaves);
        // for the following method, first argument must be an int* array of size n
        //  and second argument must be an int array of size n
        void findParents(int ** parents, int * numParents);
};

// class UndirectedGraph : public Graph {
// };