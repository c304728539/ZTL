
#include<iostream>
#include<cstdlib>
#include"..\..\ZTl\ztl_RBtree.hpp"

using namespace ztl;


int main(){

	RBtree<int> n = {1,2,4,7,9};
	RBtree<int> _n(move(n));
	RBtree<int> m;

	for (int i = 0; i < 156; ++i)
		m.insert(i);

	//for (auto it = m.begin(); it != m.end(); ++it)
	//	std::cout << *it << ' ';

	for (int i = 0; i < 150; ++i)
		m.erase(m.begin());

	for (auto it = m.begin(); it != m.end(); ++it)
		std::cout << *it << ' ';
	return 0;
}