
#include<iostream>
#include"..\..\ZTl\ztl_graph.hpp"

using namespace ztl;


int main(){
	weighted_directional_graph<char> G;
	auto it_a = G.insert('a');
	auto it_b = G.insert('b');
	auto it_c = G.insert('c');
	auto it_d = G.insert('d');


	G.insert_edge(it_a, it_b, 4);
	G.insert_edge(it_a, it_c, 8);
	G.insert_edge(it_b, it_c, 7);
	G.insert_edge(it_c, it_b, 17);
	G.insert_edge(it_c, it_d, 9);


	//G.BFS(1);
	//G.searchR(it_a, it_d);
	return 0;
	
}