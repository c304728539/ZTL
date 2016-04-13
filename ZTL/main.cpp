

#include<iostream>
#include"ztl_functional.hpp"
using std::cout;
using std::endl;
using namespace ztl;

void A(int a, int b){
	cout << a <<b;
}

int main(int argc, char *argv[], char *env[])
{
	using namespace placeholder;
	auto c = bind(A, _2, _1);
	c(1,2,3,4);
	
	// cout << typeid(typename tuple_element<4, tuple<int, float, double>>::type).name();
	//tuple<int, int, int> t(2, 3,4);
	//cout<<get<0>(t);
	//cout << get<1>(t);
	//cout << get<2>(t);


	return 0;
}