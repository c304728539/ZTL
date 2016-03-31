
#include<iostream>
#include"..\..\ZTl\ztl_vector.hpp"

using namespace ztl;

void print_vector(const vector<int>& x){
	for(auto it = x.cbegin(); it!=x.cend(); ++it)
		std::cout << *it << " ";
	std::cout << std::endl;
}

int main(){
	vector<int> vint = { 1, 2, 4, 5, 7 };
	std::cout << "capacity:" << vint.capacity() << std::endl;
	vint.push_back(9);
	vint.push_back(11);
	print_vector(vint);
	vint.resize(3);
	std::cout << "capacity:" << vint.capacity() << std::endl;
	print_vector(vint);
	vint.resize(10);
	//vint.insert_n(vint.end(), 10, 1);
	std::cout << "capacity:" << vint.capacity() << std::endl;
	print_vector(vint);
	vint.clear();
	std::cout << "capacity:" << vint.capacity() << std::endl;
	print_vector(vint);



	return 0;
}