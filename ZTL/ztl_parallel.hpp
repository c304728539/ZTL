#pragma once
#ifndef ZTL_PARALLEL_HPP
#define ZTL_PARALLEL_HPP

#include"ztl_algorithm.hpp"
#include"ztl_thread.hpp"
namespace ztl {
	template<typename InputIterator>
	void parallel_merge_sort(InputIterator first, InputIterator last) {
		threadmanage<0> tm;
		vector<pair<threadi*, vector<threadi*>>> vti;
		parallel_merge_sort_aux(first, last, vti);
		for(auto& pti : vti) {
			for (auto it = pti.second.cbegin(); it != pti.second.cend(); ++it)
				if (*it != nullptr)
					(*it)->over();
			tm.AddThread(pti.first);
		}
		tm.Exit();
		for (auto& pti : vti)
			delete pti.first;
	}

	template<typename InputIterator>
	threadi* parallel_merge_sort_aux(InputIterator first, InputIterator last, vector<pair<threadi*, vector<threadi*>>>& vti) {
		//using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		difference_type dis = ztl::distance(first, last);
		if (dis >= 2) {
			InputIterator mid = first;
			advance(mid, dis / 2);
			auto ti0 = parallel_merge_sort_aux(first, mid, vti);
			auto ti1 = parallel_merge_sort_aux(mid, last, vti);
			auto ret = new threadi(std::bind(_merge<InputIterator>, first, mid, last));
			vti.push_back(make_pair(ret, vector<threadi*>({ ti0,ti1 })));
			return ret;
		}
		return nullptr;
	}
}


#endif
