
#include<iostream>
#include"ztl_functional.hpp"
#include<functional>
#include<ctime>

using std::cout;
using std::endl;
using namespace ztl;
class Ctest {
public:
	void A(int a, int b) {
		cout << a << b;
	}
};


void A(float a, double b) {
	cout << a << b;
}

int main(int argc, char *argv[], char *env[])
{
	Ctest t;
	//bind(&Ctest::A, &t, placeholder::_2, placeholder::_1)(2,1);

	function<void(double,float)> f = bind(&A, placeholder::_2, placeholder::_1);
	f(2, 1);
	//tls_extract_back<2, _tls<double, float, int>>::result a;
	return 0;
}