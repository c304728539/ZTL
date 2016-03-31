#ifndef ZTL_TEMPLATE_H
#define ZTL_TEMPLATE_H

#include"ztl_base.hpp"

namespace ztl{

	template<int a,int ...els>
	struct Add{
		enum { result = a + Add<els...>::result};
	};

	template<int a>
	struct Add<a>{
		enum { result = a};
	};

	template<int a,int ...els>
	struct Multi{
		enum{ result = a*Multi<els...>::result };
	};

	template<int a>
	struct Multi<a>{
		enum{ result = a };
	};

	//primary template to compute pow(m,n)
	template<int m, int n>
	struct Pow{
		enum {result = m*Pow<m,n-1>::result};
	};


	template<int m>
	struct Pow<m,0>{
		enum { result = 1 };
	};

	
	template<int n>
	struct Pow<2,n>{
		enum { result = /*2*Pow<2, n - 1>::result*/ 1 << n };
	};

	template<>
	struct Pow<2, 0>{
		enum {result = 1};
	};

	// primary template to compute sqrt(N)
	template <size_t N, size_t LO = 1, size_t HI = N>
	struct Sqrt {
		// compute the midpoint, rounded up
		enum { mid = (LO + HI + 1) / 2 };

		// search a not too large value in a halved interval
		enum {
			result = (N<mid*mid) ? Sqrt<N, LO, mid - 1>::result
			: Sqrt<N, mid, HI>::result
		};
	};
	
	template<size_t LO, size_t HI>
	struct Sqrt<0, LO, HI>{
		enum{result = 0};
	};

	// partial specialization for the case when LO equals HI
	template<size_t N, size_t M>
	struct Sqrt<N, M, M>{
		enum { result = M };
	};

	template<int u>
	struct Log2{
		enum{result = Log2<u/2>::result +1};
	};

	template<>
	struct Log2<1>{
		enum{result = 0};
	};

	template<size_t... args>
	struct _idx {};

	template<size_t begin, size_t end,size_t step, size_t h, size_t... f>
	struct _make_idx_seq_aux {
		using result = typename _make_idx_seq_aux<begin, end, step, h - step, h, f...>::result;
	};

	template<size_t begin, size_t end, size_t step, size_t... f>
	struct _make_idx_seq_aux <begin, end, step, begin, f...> {
		using result = _idx<begin, f...>;
	};

	template<size_t begin, size_t end, size_t step>
	struct make_idx_seq {
		using result = typename _make_idx_seq_aux<begin, end, step, end>::result;
	};

	template<size_t fill_number, size_t n,size_t... args>
	struct _make_idx_num_aux {
		using result = typename _make_idx_num_aux<fill_number, n-1, fill_number, args...>::result;
	};

	template<size_t fill_number, size_t... args>
	struct _make_idx_num_aux<fill_number, 0, args...> {
		using result = _idx<args...>;
	};

	template<size_t fill_number, size_t n>
	struct make_idx_num {
		using result = typename _make_idx_num_aux<fill_number, n>::result;
	};

	template<typename idx1,typename idx2>
	struct cat_idx{};

	template<size_t... args1, size_t... args2>
	struct cat_idx < _idx<args1...>, _idx<args2...>> {
		using result = _idx<args1..., args2...>;
	};


}

#endif