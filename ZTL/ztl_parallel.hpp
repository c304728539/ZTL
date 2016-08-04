#pragma once
#ifndef ZTL_PARALLEL_HPP
#define ZTL_PARALLEL_HPP

#include"ztl_algorithm.hpp"
#include"ztl_thread.hpp"
namespace ztl {
	template<typename InputIterator>
	void parallel_merge_sort(InputIterator first, InputIterator last) {
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		parallel_merge_sort_aux(first, last,(size_t)log2(si.dwNumberOfProcessors));
	}

	template<typename InputIterator>
	void parallel_merge_sort_aux(InputIterator first, InputIterator last, size_t n) {
		//using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		difference_type dis = ztl::distance(first, last);
		if (n--) {
			InputIterator mid = first;
			advance(mid, dis / 2);
			std::thread th0(&parallel_merge_sort_aux<InputIterator>, first, mid, n);
			std::thread th1(&parallel_merge_sort_aux<InputIterator>, mid, last, n);
			th0.join();
			th1.join();
			_merge<InputIterator, true>(first, mid, last);
		}
		else
		{
			merge_sort<InputIterator,true>(first, last);
		}
	}
}


#endif
