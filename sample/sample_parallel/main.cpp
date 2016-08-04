
#include<iostream>
#include"..\..\ZTl\ztl_thread.hpp"
#include"..\..\ZTl\ztl_parallel.hpp"
#include<ctime>

using std::cout;
using std::endl;
using namespace ztl;

void A(int a, int b) {
	cout << a << b;
}

int main(int argc, char *argv[], char *env[])
{
	vector<int> vec0, vec1;
	for (int i = 0; i < 200000; ++i)
	{
		int r = rand();
		//std::cout << r << ' ';
		vec0.push_back(r);
		vec1.push_back(r);
	}
	std::cout << std::endl;
	clock_t start = clock();
	merge_sort(vec0.begin(), vec0.end());
	clock_t end = clock();
	std::cout << static_cast<double>(end - start) / CLOCKS_PER_SEC << std::endl;
	/*for(auto it : vec0)
	std::cout << it << ' ';
	std::cout << std::endl;*/

	start = clock();
	parallel_merge_sort(vec1.begin(), vec1.end());
	end = clock();
	std::cout << static_cast<double>(end - start) / CLOCKS_PER_SEC << std::endl;
	/*for (auto it : vec1)
	std::cout << it << ' ';
	std::cout << std::endl;*/


	return 0;
}