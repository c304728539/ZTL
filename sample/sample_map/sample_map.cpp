
#include<iostream>
#include"..\..\ZTl\ztl_map.hpp"

using namespace ztl;

int main(){

	multimap<int, int> m;

	for (int a = 0; a < 10; ++a)
		m[a] = 1;

	m[5] = 2;

	while (!m.empty()){
		for (auto it = m.begin(); it != m.end(); ++it)
			std::cout << it->first << '-' << it->second << ' ';
		std::cout << std::endl;
		auto it = m.begin();
		std::cout << it->first << '-' << it->second<<' ';
		std::cout << std::endl;
		m.erase(it);
	}

	for (auto it = m.begin(); it != m.end();++it)
		std::cout << it->first << '-' << it->second << ' ';
	return 0;
}