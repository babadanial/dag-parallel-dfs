#include "dfs.h"

using namespace std;

// checks which path has first lexicographically smallest element at mismatch,
//   returns 0 for first path (A), 1 for second path (B)
bool whichPathShorter(int * pathA, int * pathB, int & lengthPathA, int & lengthPathB) {
    for (int i = 0; i < min(lengthPathA, lengthPathB) + 1; i++) {
        if (pathA[i] < pathB[i]) {
            return 0;
        } else if (pathB[i] < pathA[i]) {
            return 1;
        }
    }
}

bool parallel_dfs::isRoot(int node) {
    for (int i = 0; i < numRoots; i++) {
        if (roots[i] == node) {
            return true;
        }
    }
    return false;
}

void parallel_dfs::getRootOrder() {
    int * rootOrder = new int[numRoots];

    this->out << "The roots of this graph are: ";
    for (int i = 0; i < numRoots; i++) {
        this->out << roots[i] << " ";
    }
    this->out << endl;

    while (true) {
        this->out << "Please list the order in which you would like DFS to visit the roots\nEnter 0 (to default to lexicographic order) or " 
            << numRoots << " root node numbers, separated by spaces): ";

        int root = -1;
        string line;
        getline(cin, line);
        istringstream ss(line);
        int lenRootOrder = 0;
        while (ss >> root) {
            if (root == -1) {
                break;
            } else if (!isRoot(root)) {
                this->out << "You entered the number " << root << " which does not correspond to a root of the graph. ";
                this->out << "Please try again." << endl;
                continue;
            }
            rootOrder[lenRootOrder] = root;
            lenRootOrder++;
        }

        // once we obtain root ordering for DFS, set roots to be in this order (or keep lexicographic ordering if user enters nothing)
        if (lenRootOrder == numRoots || lenRootOrder == 0) {
            if (lenRootOrder == numRoots) {
                for (int i = 0; i < numRoots; i++) {
                    roots[i] = rootOrder[i];
                }
            }
            break;
        } else {
            this->out << "You did not enter as many numbers as there are roots - either enter " <<
                numRoots << "numbers, or do not enter any numbers at all (to default to lexicographic order)" << endl;
        }
    }

    delete [] rootOrder;
}

parallel_dfs::parallel_dfs(DirectedGraph & g, std::ostream & out) 
    : graph{g},
      out{out},
      n{g.getSize()}
{
    roots = new int[n];

    parents = new int*[n];
    numParents = new int[n];

    children = new int*[n];
    numChildren = new int[n];

    paths = new int*[n];
    pathLengths = new int[n];
    rootAncestor = new int[n];
    edgeWeights = new int[n];
    preOrder = new int[n];
    postOrder = new int[n];

    for (int i = 0; i < n; i++) {
        rootAncestor[i] = -1;
        paths[i] = new int[n];
        children[i] = new int[n];
        parents[i] = new int[n];
        pathLengths[i] = 0;
        preOrder[i] = 0;
        postOrder[i] = 0;

        for (int j = 0; j < n; j++) {
            // initialize with value n+1 to make checking for
            //  first lexicographic smallest element cheap
            paths[i][j] = n + 1;
            parents[i][j] = -1;
        }
    }

    g.getChildren(children, numChildren);
    g.findParents(parents, numParents);
    g.findRoots(roots, &numRoots);
}

parallel_dfs::~parallel_dfs() {
    for (int i = 0; i < n; i++) {
        delete [] paths[i];
        delete [] children[i];
        delete [] parents[i];
    }

    delete [] rootAncestor;
    delete [] preOrder;
    delete [] postOrder;
    delete [] edgeWeights;
    delete [] roots;
    delete [] parents;
    delete [] numParents;
    delete [] children;
    delete [] numChildren;
    delete [] paths;
    delete [] pathLengths;
}

// Algorithm 4 from the paper
void parallel_dfs::computeDFSTree(int root) {
    std::queue<int> Q;
    int n = graph.getSize();
    Q.push(root);
    paths[root][0] = root;
    pathLengths[root]++;

    while (!Q.empty()) {
        std::queue<int> copyQ = Q;
        std::queue<int> P;
        std::deque<thread *> threadDeque;

        while (!copyQ.empty()) {
            int node = copyQ.front();
            copyQ.pop();

            // before we do any work, we have to check if this node
            //  has a root ancestor yet
            if (rootAncestor[node] != -1) {
                continue;
            }
            rootAncestor[node] = root;

            int * childrenSet = children[node];
            int childCount = numChildren[node];

            auto doIteration = [&](int node, int child) {
                int existingPathLength = -1;
                int newPathLength = -1;
                int * existingPath = new int[n];
                int * newPath = new int[n];
                
                memcpy(existingPath, paths[child], n * sizeof(int));
                existingPathLength = pathLengths[child];

                memcpy(newPath, paths[node], n * sizeof(int));
                newPathLength = pathLengths[node];
                newPath[newPathLength] = child;
                newPathLength++;

                bool shorter = whichPathShorter(newPath, existingPath, newPathLength, existingPathLength);

                if (shorter == 0) {
                    // newPath is shorter
                    memcpy(paths[child], newPath, n * sizeof(int));
                    pathLengths[child] = newPathLength;
                }

                P.push(child);
                delete [] existingPath;
                delete [] newPath;
            };

            for (int i = 0; i < childCount; i++) {
                thread * newThread = new thread(doIteration, node, childrenSet[i]);
                threadDeque.push_back(newThread);
            }
        }
        for (auto threadIterator = threadDeque.begin(); threadIterator != threadDeque.end(); threadIterator++) {
            (*threadIterator)->join();
        }
        for (auto threadIterator = threadDeque.begin(); threadIterator != threadDeque.end(); threadIterator++) {
            delete threadDeque.front();
            threadDeque.pop_front();
        }
        Q = P;
    }

}

// Algorithm 2 from the paper
void parallel_dfs::computeEdgeWeights() {
    bool * edgeWeightsComputed = new bool[n];
    // prologue
    for (int i = 0; i < n; i++) {
        edgeWeights[i] = 1;
        // edgeWeights[i] = 0;
        edgeWeightsComputed[i] = false;
    }
    queue<int> Q;

    // find leaves, push them onto the queue Q
    int * leaves = new int[n];
    int numLeaves = 0;
    graph.findLeaves(leaves, &numLeaves);
    for (int i = 0; i < numLeaves; i++) {
        Q.push(leaves[i]);
        edgeWeightsComputed[leaves[i]] = true;
        // edgeWeights[leaves[i]] = 1;
    }
    delete [] leaves;

    auto markParent = [&](int parent, queue<int> * C) {
        bool allRelevantChildrenSeen = true;
        int * parentsChildren = children[parent];
        int numParentsChildren = numChildren[parent];
        for (int j = 0; j < numParentsChildren; j++) {
            int parentsChild = parentsChildren[j];
            if (rootAncestor[parentsChild] == rootAncestor[parent] && edgeWeightsComputed[parentsChild] == false) {
                allRelevantChildrenSeen = false;
            }
        }
        if (allRelevantChildrenSeen) {
            C->push(parent);
        }
    };

    // main loop
    while (!Q.empty()) {
        std::queue<int> copyQ = Q;
        std::queue<int> C;
        std::deque<thread *> threadDeque;

        while (!copyQ.empty()) {
            int node = copyQ.front();
            copyQ.pop();
            int * nodeParents = parents[node];
            int parentCount = numParents[node];
            for (int i = 0; i < parentCount; i++) {
                int parent = nodeParents[i];
                if (rootAncestor[parent] != rootAncestor[node]) {
                    continue;
                }

                thread * newThread = new thread(markParent, parent, &C);
                threadDeque.push_back(newThread);
            }
        }
        // wait for all threads here
        for (auto threadIterator = threadDeque.begin(); threadIterator != threadDeque.end(); threadIterator++) {
            (*threadIterator)->join();
        }
        for (auto threadIterator = threadDeque.begin(); threadIterator != threadDeque.end(); threadIterator++) {
            delete threadDeque.front();
            threadDeque.pop_front();
        }

        queue<int> copyC = C;
        while (!copyC.empty()) {
            int node = copyC.front();
            copyC.pop();
            
            auto computeWeight = [&](int node) {
                int * nodeChildren = children[node];
                int childCount = numChildren[node];
                int weight = 1; // start at 1 to count the node itself
                for (int i = 0; i < childCount; i++) {
                    if (rootAncestor[nodeChildren[i]] == rootAncestor[node]) {
                        weight += edgeWeights[nodeChildren[i]];
                    }
                }
                edgeWeights[node] = weight;
                edgeWeightsComputed[node] = true;
            };
            
            thread * newThread = new thread(computeWeight, node);
            threadDeque.push_back(newThread);
        }
        // wait for all threads here
        for (auto threadIterator = threadDeque.begin(); threadIterator != threadDeque.end(); threadIterator++) {
            (*threadIterator)->join();
        }
        for (auto threadIterator = threadDeque.begin(); threadIterator != threadDeque.end(); threadIterator++) {
            delete threadDeque.front();
            threadDeque.pop_front();
        }
        Q = C;
        if (C.empty()) {
            for (int i = 0; i < n; i++) {
                // need to check that edgeWeight hasn't been computed for this node, but has
                //  been computed for all children of this node
                int * parentsChildren = children[i];
                int numParentsChildren = numChildren[i];
                bool allChildrenComputed = true;
                if (edgeWeights[i] == 0) {
                    for (int j = 0; j < numParentsChildren; j++) {
                        if (!edgeWeightsComputed[parentsChildren[j]]) {
                            allChildrenComputed = false;
                        }
                    }
                    if (allChildrenComputed) {
                        Q.push(i);
                    }
                }
            }
        }
    }

    delete [] edgeWeightsComputed;
}

// Algorithm 3 from the paper
void parallel_dfs::computePreAndPostOrders() {
    queue<int> Q;
    for (int i = 0; i < numRoots; i++) {
        Q.push(roots[i]);
    }

    while (!Q.empty()) {
        std::queue<int> copyQ = Q;
        std::queue<int> P;
        std::deque<thread *> threadDeque;
        
        while (!copyQ.empty()) {
            int node = copyQ.front();
            copyQ.pop();
            int * nodeChildren = children[node];
            int childCount = numChildren[node];

            auto setChild = [&] (int parent, int weightUpToChild, int child) {
                preOrder[child] = preOrder[parent] + weightUpToChild;
                postOrder[child] = postOrder[parent] + weightUpToChild;
            };
        }

        Q = P;
    }
}

void parallel_dfs::directed_dfs() {
    getRootOrder();
    for (int i = 0; i < numRoots; i++) {
        computeDFSTree(roots[i]);
    }
    computeEdgeWeights();
    computePreAndPostOrders();
}
