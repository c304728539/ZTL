
#include<iostream>
#include"ztl_thread.hpp"
#include"ztl_parallel.hpp"
#include<ctime>

using std::cout;
using std::endl;
using namespace ztl;

void A(int a, int b){
	cout << a <<b;
}

int main(int argc, char *argv[], char *env[])
{
	vector<int> vec0,vec1;
	for (int i = 0; i < 42220; ++i)
	{
		int r = rand();
		vec0.push_back(r);
		vec1.push_back(r);
	}
	clock_t start = clock();
	merge_sort(vec0.begin(), vec0.end());
	clock_t end = clock();
	std::cout << static_cast<double>(end - start) / CLOCKS_PER_SEC << std::endl;
	start = clock();
	parallel_merge_sort(vec1.begin(), vec1.end());
	end = clock();
	std::cout << static_cast<double>(end - start) / CLOCKS_PER_SEC << std::endl;

	

	return 0;
}