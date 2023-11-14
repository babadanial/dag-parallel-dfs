#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "graph.h"

using namespace std;

// Get graph properties
void getInputs(int * n, bool * directed) { 
    cout << "Please input the number of nodes in the graph: ";
    cin >> *n;
    while (*n <= 1) { 
        cout << "Please input a number greater than 1: ";
        cin >> *n;
    }

    *directed = true;
    // UNCOMMENT WHEN UNDIRECTED GRAPHS ARE SUPPORTED
    // char directedChar;
    // cout << "Is the graph directed? [Y/N]: ";
    // cin >> directedChar;
    // while (!((directedChar == 'Y' || directedChar == 'N'))) {
    //     cout << "Please input the character Y (for yes) or N (for no): ";
    //     cin >> directedChar;
    // }
    // if (directedChar == 'Y') {
    //     *directed = true;
    // } else {
    //     cout << "This program doesn't support undirected graphs yet, sorry." << endl;
    //     exit(0);
    //     // *directed = false;
    // }
}

// For undirected graphs, checks that if node i is in node j's adjacency list,
//  then node j is also in node i's adjacency list
void checkUndirectedGraphInput(int ** adjacencyList, int n) {
    for (int currentNode = 0; currentNode < n; currentNode++) {
        for (int neighbourNode = 0; neighbourNode < n; neighbourNode++) {
            if (adjacencyList[currentNode][neighbourNode] == -1) {
                // we reached end of adjacency list for currentNode
                break;
            }
            for (int i = 0; i < n; i++) {
                if (adjacencyList[neighbourNode][i] == currentNode) {
                    break;
                }
            }
            cout << "Fatal: node " << neighbourNode << " is in adjacency list of node " << currentNode 
                 << ", but node " << currentNode << " is not in adjacency list of node " << neighbourNode << endl;
            exit(1);
        }
    }
}

// Construct adjacency list for graph from inputs
//  Adjacency list for node i stored at index i-1
void getAdjacencyList(int ** adjacencyList, int & n, bool & directed) {
    string line;
    getline(cin, line);
    for (int node = 0; node < n; node++) {
        int * neighbourList = new int[n];
        for (int i = 0; i < n; i++) {
            neighbourList[i] = -1;
        }

        if (directed) {
            cout << "Please input the numbers of the out-neighbour nodes of node " << node << ", separated by spaces: "; 
        } else {
            cout << "Please input the numbers of the neighbour nodes of node " << node << ", separated by spaces: "; 
        }
        int neighbour;
        getline(cin, line);
        istringstream ss(line);
        int index = 0;

        while (ss >> neighbour) {
            if (index == n) {
                cout << "Fatal error: Node cannot have more neighbours than total number of nodes " + to_string(n) + " in the graph!" << endl;
                exit(1);
            } else if (neighbour == node) {
                cout << "Fatal error: Node cannot be a neighbour of itself!" << endl;
                exit(1);
            } else if (neighbour >= n) {
                cout << "Fatal error: node " << neighbour << " is out of range for graph with " << n << " nodes!" << endl;
                exit(1);
            }
            neighbourList[index] = neighbour;
            index++;
        }
        adjacencyList[node] = neighbourList;
    }

    if (!directed) {
        checkUndirectedGraphInput(adjacencyList, n);
    }
}

int main() {
    int n;
    bool directed;
    getInputs(&n, &directed);
    int ** adjacencyList = new int*[n];
    getAdjacencyList(adjacencyList, n, directed);
    // TODO: FIX MEMORY LEAKS + WRITE DESTRUCTOR FOR DIRECTED GRAPH
    DirectedGraph dag = DirectedGraph{adjacencyList, n, directed};
    cout << dag;

    int * roots = new int[n];
    int * leaves = new int[n];
    int ** parents = new int*[n];
    for (int i = 0; i < n; i++) {
        parents[i] = new int[n];
    }
    int * numParents = new int[n];

    int numRoots;
    int numLeaves;
    dag.findRoots(roots, &numRoots);
    dag.findLeaves(leaves, &numLeaves);
    dag.findParents(parents, numParents);

    delete [] roots;
    delete [] numParents;
    delete [] leaves;
    for (int i = 0; i < n; i++) {
        delete [] parents[i];
    }
    delete [] parents;

    for (int i = 0; i < n; i++) {
        delete [] adjacencyList[i];
    }
    delete [] adjacencyList;
}