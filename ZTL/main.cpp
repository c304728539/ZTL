

#include<iostream>
#include"ztl_largenum.hpp"
using std::cout;
using std::endl;
using namespace ztl;

int main(int argc, char *argv[], char *env[])
{
	LargeFloat a("-11111111111123123.34");
	LargeFloat b(1111.1f);
	cout << a <<endl;
	cout << b << endl;

	cout << a+(b) << endl;
	//cout << a-(b) << endl;
	//cout << a*(b) << endl;
	//cout << a/(b) << endl;
	//cout << a%(b) << endl;

	return 0;
}