#include <iostream>
#include <ostream>
#include <thread>

class Graph {
    protected:
        // number of nodes in graph
        int n;
        bool directed;
        int ** adjacencyList;
        int ** adjacencyMatrix;

        Graph(int ** adjacencyList, int n, bool directed);

    public:
        virtual void buildAdjacencyMatrixSequential() = 0;
        virtual void buildAdjacencyMatrixParallel() = 0;
        friend std::ostream &operator<<(std::ostream &out, const Graph &g);
        int adjacencyMatrixElement(int row, int col);
};

class DirectedGraph : public Graph {
    public:
        DirectedGraph(int ** adjacencyList, int n, bool directed);
        void buildAdjacencyMatrixSequential();
        void buildAdjacencyMatrixParallel();
};

// class UndirectedGraph : public Graph {
//     public:
//         UndirectedGraph(int ** adjacencyList);
// };