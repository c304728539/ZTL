
#include<iostream>
#include"..\..\ZTl\ztl_fibonacci_heap.hpp"

using namespace ztl;


int main(){
	fibonacci_heap<int> fh;
	for (int i = 0; i < 1000; ++i)
		fh.insert(i);
	for (int i = 0; i < 500; ++i)
		fh.delete_min();

	for (auto it = fh.begin(); it != fh.end();++it)
		std::cout << *it<<' ';

	std::cout << std::endl;
	return 0;
}