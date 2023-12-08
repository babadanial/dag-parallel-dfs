This project is an implementation of Algorithm 5 in [this paper](https://research.nvidia.com/sites/default/files/publications/nvr-2017-001.pdf) from NVIDIA, allowing multi-threaded depth-first search on directed acyclic graphs (which need not be connected).

However, some modifications have been made to the 3 subroutines to allow depth-first search to be performed
on directed acyclic graphs with multiple roots: you can specify an order $(r_1, r_2, ..., r_n)$ to visit the roots, 
in which case pre- and post-orders will be calculated as though DFS was first run on $r_1$, then on $r_2$ (excluding
nodes that were visited after running DFS from $r_1$), etc.

- Graphs are inputted using adjacency lists (see test cases for format).
- The program outputs pre- and post-orders of all nodes based on performing DFS on the roots in the ordering given.
- Makefile is provided for convenience.

If you want to pass a file as input, the format is as follows:
```
<number n of nodes>
<out-neighbours of node 0, separated by spaces>
...
<out-neighbours of node n-1, separated by spaces>
[<order in which roots of graph should be visited by DFS, separated by spaces>]
```
The last line (where root order for DFS is provided is optional); however, if an order is specified, it must include all roots
of the graph.
