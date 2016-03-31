#ifndef ZTL_ALGORITHM_HPP
#define ZTL_ALGORITHM_HPP

#include"ztl_base.hpp"
#include"ztl_iterator.hpp"
//#include"ztl_vector.hpp"

namespace ztl{

	template<typename InputIterator, typename OutputIterator>
	OutputIterator _copy_aux(InputIterator first, InputIterator last, OutputIterator destination, random_access_iterator_tag, random_access_iterator_tag){
		using value_type = typename iterator_traits<OutputIterator>::value_type;
		return (OutputIterator)(value_type*)memmove(&*destination, &*first, ztl::distance(first, last)*sizeof(typename iterator_traits<OutputIterator>::value_type));
	}

	template<typename InputIterator, typename OutputIterator>
	OutputIterator _copy_aux(InputIterator first, InputIterator last, OutputIterator destination, forward_iterator_tag, forward_iterator_tag){
		using value_type = typename iterator_traits<InputIterator>::value_type;
		OutputIterator ret = destination;
		for (; first !=last; ++first,++destination){
			new(&*destination)value_type(*first);
		}
		return ret;
	}

	template<typename InputIterator,typename OutputIterator>
	inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator destination){
		return _copy_aux(first, last, destination, _iterator_category(first), _iterator_category(destination));
	}

	template<typename InputIterator, typename Size, typename OutputIterator>
	OutputIterator _copy_n_aux(InputIterator first, Size n, OutputIterator destination, random_access_iterator_tag){
		return memmove(destination, first, n*sizeof(typename TypeCheck<InputIterator>::BaseT));
	}

	template<typename InputIterator, typename Size, typename OutputIterator>
	OutputIterator _copy_n_aux(InputIterator first, Size n, OutputIterator destination, forward_iterator_tag){
		OutputIterator ret = destination;
		for (Size i = Size(); i != n; ++i, ++first, ++destination){
			memmove(&*destination, &*first);
		}
		return ret;
	}

	template<typename InputIterator,typename Size, typename OutputIterator>
	inline OutputIterator copy_n(InputIterator first, Size n, InputIterator destination){
		return _copy_n_aux(first, n, destination, iterator_category(destination));
	}

	template<typename InputIterator, typename ValueType> 
	inline InputIterator find(InputIterator first, InputIterator last, ValueType value)
	{
		for (; first != last; ++first)
		if (*first == value)
			break;
		return (first);
	}



	template<class T> inline
	void swap(T& _Left, T& _Right){	// exchange values stored at _Left and _Right
		T _Tmp = ztl::move(_Left);
		_Left = ztl::move(_Right);
		_Right = ztl::move(_Tmp);
	}
}

#include"ztl_vector.hpp"

namespace ztl{

	template<typename InputIterator>
	void _insertion_sort_aux(InputIterator first, InputIterator last, random_access_iterator_tag){
		using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		for (difference_type j = 1; j < last - first; ++j){
			value_type key = move(first[j]);
			difference_type i = j - 1;
			while (i >= 0 && first[i] > key){
				first[i + 1] = move(first[i]);
				--i;
			}
			first[i + 1] = move(key);
		}
	}

	template<typename InputIterator>
	void _insertion_sort_aux(InputIterator first, InputIterator last, bidirectional_iterator_tag){	//只为bidirectional_iterator以上迭代器提供快速排序算法
		using value_type = typename iterator_traits<InputIterator>::value_type;
		//using difference_type = typename iterator_traits<InputIterator>::difference_type;
		InputIterator beg = first;
		++beg;
		for (;beg!=last;++beg){
			value_type key = ztl::move(*beg);
			InputIterator back_it = beg;
			--back_it;
			while (back_it != first && *back_it > key){
				InputIterator move_it = back_it;
				++move_it;
				*move_it = ztl::move(*back_it);
				--back_it;
			}
			if (key < *back_it){
				InputIterator move_it = back_it;
				++move_it;
				*move_it = ztl::move(*back_it);
				*back_it = ztl::move(key);
				continue;
			}
			++back_it;
			*back_it = ztl::move(key);
		}
	}

	template<typename InputIterator>
	void insertion_sort(InputIterator first, InputIterator last){
		_insertion_sort_aux(first, last, _iterator_category(first));
	}

	template<typename InputIterator>
	InputIterator _quick_sort_partition(InputIterator first, InputIterator last){
		using value_type = typename iterator_traits<InputIterator>::value_type;
		//using difference_type = typename iterator_traits<InputIterator>::difference_type;
		value_type x = *--last;
		for (auto j = first; j != last; ++j){
			if (*j < x){
				ztl::swap(*first, *j);
				++first;
			}
		}
		ztl::swap(*first, *last);
		return first;
	}



	template<typename InputIterator>
	void quick_sort(InputIterator first, InputIterator last){
		//using value_type = typename iterator_traits<InputIterator>::value_type;
		//using difference_type = typename iterator_traits<InputIterator>::difference_type;
		if (ztl::distance(first, last)>1){
			InputIterator mid = _quick_sort_partition(first, last);
			quick_sort(first, mid);
			quick_sort(++mid, last);
		}
	}

	template<typename InputIterator>
	void _merge(InputIterator& first, InputIterator& mid, InputIterator& last){
		using value_type = typename iterator_traits<InputIterator>::value_type;
		//using difference_type = typename iterator_traits<InputIterator>::difference_type;
		vector<value_type> l(first, mid), r(mid, last);
		auto lit = l.begin();
		auto rit = r.begin();
		for (auto it = first; it != last; ++it/*,++first*/)
			if ((lit != l.end() && rit != r.end()) ? *lit < *rit : lit != l.end())
				*it = *lit++;
			else
				*it = *rit++;
	}

	template<typename InputIterator>
	void merge_sort(InputIterator first, InputIterator last){
		//using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		difference_type dis = ztl::distance(first, last);
		if (dis>=2){
			InputIterator mid = first;
			advance(mid, dis / 2);
			merge_sort(first, mid);
			merge_sort(mid, last);
			/*vector<value_type> l(first, mid), r(mid, last);
			auto lit = l.begin();
			auto rit = r.begin();
			for (auto it = first; it != last; ++it)
			if ((lit != l.end() && rit != r.end()) ? *lit < *rit : lit != l.end())
				*it=*lit++;
			else
				*it=*rit++;*/
			_merge(first, mid, last);
		}
	}


	template<typename InputIterator>
	void _shell_sort_aux(InputIterator first, InputIterator last, random_access_iterator_tag) {
		//using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		difference_type len = ztl::distance(first, last);
		for (difference_type div = len / 2; div >= 1; div /= 2)
			for (difference_type i = div; i < len; ++i)
				for (difference_type j = i; j >= div && (first[j] < first[j - div]); j -= div)
					ztl::swap(first[j], first[j - div]);
	}

	template<typename InputIterator>
	void shell_sort(InputIterator first, InputIterator last){
		_shell_sort_aux(first,last,_iterator_category(first));
	}



	template<typename InputIterator>
	void _bubble_sort_aux(InputIterator first, InputIterator last, bidirectional_iterator_tag){
		//using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		difference_type len = ztl::distance(first, last);
		InputIterator begin;
		--last;
		for (difference_type i = 0; i != len; ++i, --last)
		for (begin = first; begin != last; ++begin){
			if (*begin > *(begin + 1))
				swap(*begin, *(begin + 1));
		}
	}

	template<typename InputIterator>
	void _bubble_sort_aux(InputIterator first, InputIterator last, random_access_iterator_tag){
		//using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		difference_type len = ztl::distance(first, last);
		for (difference_type i = 1; i <= len; ++i, --last)
		for (auto j = 0; j<len - i;++j){
			if (first[j]>first[j+1])
				swap(first[j],first[j+1]);
		}
	}

	template<typename InputIterator>
	void bubble_sort(InputIterator first, InputIterator last){
		_bubble_sort_aux(first, last, _iterator_category(first));
	}
	
#ifdef _DEBUG
	
	template<typename InputIterator>
	void _print_range_d(InputIterator first, InputIterator last){
		for (; first != last; ++first)
			std::cout << *first <<' ';
		std::cout << std::endl;
	}

	template<typename InputIterator>
	void _insertion_sort_d_aux(InputIterator first, InputIterator last, random_access_iterator_tag){
		using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		for (difference_type j = 1; j < last - first; ++j){
			value_type key = move(first[j]);
			difference_type i = j - 1;
			while (i >= 0 && first[i] > key){
				first[i + 1] = move(first[i]);
				--i;
			}
			first[i + 1] = move(key);
			_print_range_d(first,last);
		}
	}

	template<typename InputIterator>
	void _insertion_sort_d_aux(InputIterator first, InputIterator last, bidirectional_iterator_tag){	//只为bidirectional_iterator以上迭代器提供快速排序算法
		//using value_type = typename iterator_traits<InputIterator>::value_type;
		//using difference_type = typename iterator_traits<InputIterator>::difference_type;
		InputIterator beg = first;
		++beg;
		for (; beg != last; ++beg){
			value_type key = ztl::move(*beg);
			InputIterator back_it = beg;
			--back_it;
			while (back_it != first && *back_it > key){
				InputIterator move_it = back_it;
				++move_it;
				*move_it = ztl::move(*back_it);
				--back_it;
			}
			if (key < *back_it){
				InputIterator move_it = back_it;
				++move_it;
				*move_it = ztl::move(*back_it);
				*back_it = ztl::move(key);
				continue;
			}
			++back_it;
			*back_it = ztl::move(key);
			_print_range_d(first, last);

		}
	}

	template<typename InputIterator>
	void insertion_sort_d(InputIterator first, InputIterator last){
		_insertion_sort_d_aux(first, last, _iterator_category(first));
	}

	template<typename InputIterator>
	InputIterator _quick_sort_partition_d(InputIterator first, InputIterator last){
		using value_type = typename iterator_traits<InputIterator>::value_type;
		//using difference_type = typename iterator_traits<InputIterator>::difference_type;
		value_type x = *--last;
		for (auto j = first; j != last; ++j){
			if (*j < x){
				ztl::swap(*first, *j);
				++first;
			}
		}
		ztl::swap(*first, *last);
		return first;
	}



	template<typename InputIterator>
	void _quick_sort_d_aux(InputIterator first, InputIterator last,const InputIterator& first_copy,const InputIterator& last_copy){
		using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		if (ztl::distance(first, last)>1){
			InputIterator mid = _quick_sort_partition_d(first, last);
			_print_range_d(first_copy, last_copy);
			_quick_sort_d_aux(first, mid,first_copy,last_copy);
			_quick_sort_d_aux(++mid, last,first_copy,last_copy);
		}
	}

	template<typename InputIterator>
	void quick_sort_d(InputIterator first, InputIterator last){
		_quick_sort_d_aux(first, last, first, last);
	}

	template<typename InputIterator>
	void _merge_sort_d_aux(InputIterator first, InputIterator last, const InputIterator& first_copy, const InputIterator& last_copy){
		using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		difference_type dis = ztl::distance(first, last);
		if (dis >= 2){
			InputIterator mid = first;
			advance(mid, dis / 2);
			_merge_sort_d_aux(first, mid, first_copy, last_copy);
			_merge_sort_d_aux(mid, last, first_copy, last_copy);
			vector<value_type> l(first, mid), r(mid, last);
			auto lit = l.begin();
			auto rit = r.begin();
			for (auto it = first; it != last; ++it, ++first){
				if ((lit != l.end() && rit != r.end()) ? *lit < *rit : lit != l.end())
					*it = *lit++;
				else
					*it = *rit++;
			}
			_print_range_d(first_copy, last_copy);
		}
	}

	template<typename InputIterator>
	void merge_sort_d(InputIterator first, InputIterator last){
		_merge_sort_d_aux(first, last, first, last);
	}

	template<typename InputIterator>
	void _shell_sort_d_aux(InputIterator first, InputIterator last, random_access_iterator_tag) {
		using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		difference_type len = ztl::distance(first, last);
		for (difference_type div = len / 2; div >= 1; div /= 2)
			for (difference_type i = div; i < len; ++i)
				for (difference_type j = i; j >= div && (first[j] < first[j - div]); j -= div)
				{
					ztl::swap(first[j], first[j - div]);
					_print_range_d(first, last);
				}
	}

	template<typename InputIterator>
	void shell_sort_d(InputIterator first, InputIterator last) {
		_shell_sort_d_aux(first, last, _iterator_category(first));
	}

	template<typename InputIterator>
	void _bubble_sort_d_aux(InputIterator first, InputIterator last, bidirectional_iterator_tag){
		using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		difference_type len = ztl::distance(first, last);
		InputIterator begin;
		auto last_copy = last;
		--last;
		for (difference_type i = 0; i != len; ++i, --last){
			for (begin = first; begin != last; ++begin){
				if (*begin > *(begin + 1))
					swap(*begin, *(begin + 1));
			}
			_print_range_d(first, last_copy);

		}
	}

	template<typename InputIterator>
	void _bubble_sort_d_aux(InputIterator first, InputIterator last, random_access_iterator_tag){
		using value_type = typename iterator_traits<InputIterator>::value_type;
		using difference_type = typename iterator_traits<InputIterator>::difference_type;
		difference_type len = ztl::distance(first, last);
		auto last_copy = last;
		for (difference_type i = 1; i <= len; ++i, --last){
			for (auto j = 0; j<len - i; ++j){
				if (first[j]>first[j + 1])
					swap(first[j], first[j + 1]);
			}
			_print_range_d(first, last_copy);
		}
	}

	template<typename InputIterator>
	void bubble_sort_d(InputIterator first, InputIterator last){
		_bubble_sort_d_aux(first, last, _iterator_category(first));
	}

#endif
}


#endif	//ZTL_ALGORITHM_HPP
