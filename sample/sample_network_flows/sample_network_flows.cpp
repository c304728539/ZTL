
#include<iostream>
#include"..\..\ZTl\ztl_network_flows.hpp"

using namespace ztl;

int main()
{
	network_flows<char> nf;

	auto it_s = nf.insert('s');
	auto it_a = nf.insert('a');
	auto it_b = nf.insert('b');
	auto it_c = nf.insert('c');
	auto it_d = nf.insert('d');
	auto it_t = nf.insert('t');

	nf.insert_edge(it_s, it_a, 16);
	nf.insert_edge(it_s, it_c, 13);
	nf.insert_edge(it_a, it_b, 12);
	nf.insert_edge(it_b, it_c, 9);
	nf.insert_edge(it_c, it_a, 4);
	nf.insert_edge(it_c, it_d, 14);
	nf.insert_edge(it_d, it_b, 7);
	nf.insert_edge(it_b, it_t, 20);
	nf.insert_edge(it_d, it_t, 4);

	nf.set_s_node(it_s);
	nf.set_t_node(it_t);

	std::cout << nf.edmonds_karp().first;

	return 0;
}