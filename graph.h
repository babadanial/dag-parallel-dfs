#include <iostream>
#include <ostream>
#include <thread>
#include <mutex>

class Graph {
    protected:
        // number of nodes in graph
        const int n;
        const bool directed;
        int ** adjacencyList;
        int ** adjacencyMatrix;

        Graph(int ** adjacencyList, int n, bool directed);
        virtual ~Graph();

    public:
        int getSize();
        virtual void buildAdjacencyMatrixSequential() = 0;
        virtual void buildAdjacencyMatrixParallel() = 0;
        friend std::ostream &operator<<(std::ostream &out, const Graph &g);
        int adjacencyMatrixElement(int row, int col);
};

class DirectedGraph : public Graph {
    public:
        void buildAdjacencyMatrixSequential();
        void buildAdjacencyMatrixParallel();
        bool rootsFound;
        int * roots;
        int numRoots;
        bool leavesFound;
        int * leaves;
        int numLeaves;
        int ** parents;
        int * numParents;
        bool parentsFound;

        DirectedGraph(int ** adjacencyList, int n, bool directed);
        virtual ~DirectedGraph();

        // for the following 2 methods, first argument must be array of size n
        void findRoots(int * roots, int * numRoots);
        void findLeaves(int * leaves, int * numLeaves);
        void findParents(int ** parents, int * numParents);
};

// class UndirectedGraph : public Graph {
//     public:
//         UndirectedGraph(int ** adjacencyList);
// };