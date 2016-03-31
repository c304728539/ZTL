
#include<iostream>

#include"..\..\ZTL\ztl_algorithm.hpp"
#include<time.h>
using namespace ztl;
using std::cout;
using std::endl;

using std::string;

template<typename InputIterator>
class Sorttest{
	typedef void(*func)(InputIterator, InputIterator);
	string testname;
	func sort_func;
public:
	Sorttest(string _testname, func _sort_func) :testname(_testname), sort_func(_sort_func){

	}

	template<typename con>
	void run(con c){	//pass by value
		clock_t start = clock();
		sort_func(c.begin(), c.end());
		clock_t end = clock();
		cout << "test function name:" << testname.c_str() << " total time:" << end - start << " msec\n";
	}

};
int main(){
	vector<int> vec;
	for (int i = 0; i != 30000; ++i)
		vec.push_back(rand());
	Sorttest<vector<int>::iterator> bubbletest("bubble sort", bubble_sort);
	bubbletest.run(vec);
	Sorttest<vector<int>::iterator> shelltest("shell sort", shell_sort);
	shelltest.run(vec);
	Sorttest<vector<int>::iterator> quicktest("quick sort", quick_sort);
	quicktest.run(vec);
	Sorttest<vector<int>::iterator> mergetest("merge sort", merge_sort);
	mergetest.run(vec);
	Sorttest<vector<int>::iterator> insertiontest("insertion sort", insertion_sort);
	insertiontest.run(vec);
	return 0;
}