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

parallel_dfs::parallel_dfs(DirectedGraph & g) 
    : graph{g}
{
    n = g.getSize();
    roots = new int[n];

    parents = new int*[n];
    parentVisited = new bool*[n];
    parentVisitedIndex = new int[n];
    numParents = new int[n];

    children = new int*[n];
    numChildren = new int[n];

    paths = new int*[n];
    pathLengths = new int[n];

    edgeWeights = new int[n];

    for (int i = 0; i < n; i++) {
        parentVisited[i] = new bool[n];
        parentVisitedIndex[i] = 0;
        paths[i] = new int[n];
        children[i] = new int[n];
        parents[i] = new int[n];
        pathLengths[i] = 0;

        for (int j = 0; j < n; j++) {
            // initialize with value n+1 to make checking for
            //  first lexicographic smallest element cheap
            paths[i][j] = n + 1;
            parents[i][j] = -1;
            parentVisited[i][j] = false;
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
        delete [] parentVisited[i];
    }

    delete [] edgeWeights;
    delete [] roots;
    delete [] parents;
    delete [] parentVisited;
    delete [] parentVisitedIndex;
    delete [] numParents;
    delete [] children;
    delete [] numChildren;
    delete [] paths;
    delete [] pathLengths;
}

// Algorithm 4 from the paper
void parallel_dfs::computeDFSTree() {
    std::queue<int> Q;
    int n = graph.getSize();

    for (int i = 0; i < numRoots; i++) {
        Q.push(roots[i]);
        paths[roots[i]][0] = roots[i];
        pathLengths[roots[i]]++;
    }

    std::mutex * nodeMutexes = new std::mutex[n];

    while (!Q.empty()) {
        std::queue<int> copyQ = Q;
        std::queue<int> P;
        int numThreads = copyQ.size();
        std::deque<thread *> threadDeque;

        while (!copyQ.empty()) {
            int node = copyQ.front();
            copyQ.pop();
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
                } else {
                    // existingPath is shorter
                }

                nodeMutexes[child].lock();
                parentVisited[child][parentVisitedIndex[child]] = true;
                parentVisitedIndex[child]++;
                if (parentVisitedIndex[child] == numParents[child]) {
                    P.push(child);
                }
                nodeMutexes[child].unlock();

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

    delete [] nodeMutexes;
}

// Algorithm 2 from the paper
void parallel_dfs::computeEdgeWeights() {
    for (int i = 0; i < n; i++) {
        edgeWeights[i] = 0;
    }
}

// Algorithm 3 from the paper
void parallel_dfs::computePreAndPostOrders() {
    
}

void parallel_dfs::directed_dfs() {
    // main body
    computeDFSTree();

}
