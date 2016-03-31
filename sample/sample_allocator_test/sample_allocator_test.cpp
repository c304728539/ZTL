#include"..\..\ZTL\ztl_allocator.hpp"
#include"..\..\ZTL\ztl_list.hpp"
#include<thread>
#include<vector>
#include<iostream>
#include<ctime>
template<size_t n>
struct B {
	char data[n];
};

template<template<typename T> class allocator, typename T>
void thread_B() {
	ztl::list<T, allocator> a;
	//std::cout << "thread:" << std::this_thread::get_id() << "excuting!\n";
	for (int i = 0; i < 100000; i++) {
		a.push_back(T());
	}
	//std::cout << "thread:" << std::this_thread::get_id() << "excute over!\n";
}

template<template<typename> class allocator, size_t n, size_t... args>
void init(std::vector<std::thread>& vec,ztl::_idx<n,args...>) {
	vec.push_back(std::thread(thread_B<allocator,B<n>>));
	init<allocator>(vec, ztl::_idx<args...>());
}

template<template<typename> class, size_t... T>
void init(std::vector<std::thread>& vec, ztl::_idx<>) {

}

template<template<typename> class allocator>
void test() {
	std::vector<std::thread> vec;
	init<allocator>(vec, ztl::make_idx_seq<8, 128, 8>::result());
	clock_t start = clock();
	for (auto& it : vec)
		it.join();
	clock_t end = clock();
	std::cout << static_cast<double>(end - start) / CLOCKS_PER_SEC << std::endl;
}

int main() {
	test<ztl::Allocator_thread>();
	test<ztl::Allocator_z_thread>();
	return 0;
}