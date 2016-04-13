ZTL
==========
## Overview
This template library extends STL. It has rewrote some containers, which have already existed, like Vector, List, Deque and so on. And it also implements other data structure such as Graph, van Emde Boas Tree(vEBtree), Splay Tree(splay_tree), Size Balanced Tree(SBtree). Otherwise, the library exposes some data structure’s interface that we cannot access in STL. A well-known fact is Red Black Tree(RBtree) as the base of Map(map). Now, you have a direct access to them.
## Container Template Parameter Difference from STL
You may have noticed the template parameter of container have a little difference. When you input “ztl::vector<”, the tip text “vector<typename T, template<typename> class Alloc = ztl::Allocator>” reminds you should provide a name of template that have one template parameter. Therefore, you can just play:
```cpp
ztl::vector<int, ztl::Allocator>
```
rather than:
```cpp
std::vector<int, std::allocator<int>>
```
If you want to have a custom-made allocator, you are supposed to make a specialized template of Allocator. In this situation, your program will work even though in the default template parameter.
## Alloc & Alloc_z
Alloc is a memory pool, whose prototype coming from SGI STL. It can support multi-thread by the template parameter named thread. But it’s performance is not very well because it always enters critical even though when allocate sizes are different. Alloc_z solve this. The test result(sample_allocator_test) shows that you can get an acceleration 17 times on a X5650 X2 PLATFORM.
## Vector
In C++11, Vector has a member function called shrink_to_fit, which shrink the capacity according to size. ztl::vector don’t have this function. However, it will adjust capacity automatically. ztl::vector continue to have the growth factor of 1.5 in STL, it also have a decay factor of 2. When the effective size compared with capacity less than one third, it will call shrink function to shrink to half, and it just will happen in the situation that the capacity greater than 5.
## vEBtree
I implement van Emde Boas tree, whitch has efficient operator.

| Operator | Time Complexity |
| ------------- | ------------- |
| Space | O(M) |
| Search | O(log log M) |
| Insert | O(log log M) |
| Delete | O(log log M) |

The algorithm is from《Introduction to Algorithms》, it also break the rule which force the size of the tree must be the Nth power of 2 and again(2^(2^n )). Follow the method of the book, the size of it just be the Nth power of 2(2^n).
## Graph
In ztl_graph_base.hpp, the class __graph_base defines how to inser or delete a node on a graph. It also defines some basic algorithms of graph, such as depth-first-search and breadth-first-search. The class __weighted_graph_base inherits from class __graph_base, it defines more algorithms of graph refer to weight/cost of edge, such as generate a Minimum Spanning Tree including by Kruskal and Prim, find shortest path by Dijkstra and Bellman-ford in case of single source and Floyd-warshall in case of all.

| Class | Base | description |
| ------------- |  ------------- | ------------- | 
| Graph | __graph_base | undirected graph |
| directional_graph | __graph_base | directed graph |
| weighted_graph | __weighted_graph_base | weighted |
| weighted_directional_graph | __weighted_graph_base| weighted directed graph |


In ztl_graph.hpp, it defines four kind of graph like the sheet shown above. All of them maintain an adjacency list internally. It will be convert to an adjacency matrix when needed. The difference between the four kind of graph occurs when insert or delete an edge on them. In general, undirected graph can be equal to a directed graph whose edge appear paired and opposite each pair. In fact, class directed graph does like this to realize itself.
## About support Linux
The most files can compile by gcc with error or warning so far now. a few of others is under correcting. What I can promise is that the header file ztl_graph.hpp and headers it rely on can be used.
## Some known bug
In the case of run single source dijkstra process in a large node weighted directional graph, Fibonacci heap return a iterator to a list, whose size is nor zero, and this iterator may point to the head node of that list. But both the previous node and next node are itself.



