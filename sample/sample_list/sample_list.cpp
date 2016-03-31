
#include<iostream>
#include"..\..\ZTl\ztl_list.hpp"

using namespace ztl;

void print_list(const list<int>& x) {
	for (auto it = x.cbegin(); it != x.cend(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;
}

int main() {
	list<int> l = { 1,2,3,4,5,6,7,8,9 };
	auto c = l.begin();
	print_list(l);
	advance(c, 5);
	l.erase(++l.begin(), c);
	print_list(l);
	return 0;
}