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



	template<typename ...args>
	struct _tls;

	template<int num, typename tls>
	struct _make_tls_void_aux;

	template<int num>
	struct make_tls_void {
		using result = typename _make_tls_void_aux<num, _tls<>>::result;
	};

	template<int num, typename... args>
	struct _make_tls_void_aux<num, _tls<args...>> {
		using result = typename _make_tls_void_aux<num - 1, _tls<args..., void>>::result;
	};

	template<typename... args>
	struct _make_tls_void_aux<0, _tls<args...>> {
		using result = _tls<args...>;
	};

	template<typename tls>
	struct tls_size;

	template<typename... args>
	struct tls_size<_tls<args...>> {
		static const size_t size = sizeof...(args);
	};

	template<int num, typename tls, typename r>
	struct _tls_extract_front_aux;

	template<int num, typename tls>
	struct tls_extract_front {
		using result = typename IfThenElse < num != 0,
			typename _tls_extract_front_aux<num, tls, _tls<>>::result,
			_tls < >> ::Result;
	};

	template<int num, typename arg, typename... args, typename... r>
	struct _tls_extract_front_aux<num,_tls<arg,args...>,_tls<r...>> {
		using list = typename IfThenElse < num != 0, _tls<args...>, void>::Result;
		using result = typename IfThenElse<num!=0,
			typename _tls_extract_front_aux<num - 1, list, _tls<r..., arg>>::result,
			_tls<r...>>::Result;
	};

	template<int num, typename r>
	struct _tls_extract_front_aux<num, _tls<>, r> {
		using result = r;
	};

	template<typename r>
	struct _tls_extract_front_aux<-1, void, r> {
		using result = r;
	};

	template<int num, typename tls>
	struct _tls_extract_back_aux;

	template<int num, typename arg, typename... args>
	struct _tls_extract_back_aux<num, _tls<arg, args...>> {
		using list = typename IfThenElse < num != 0, _tls<args...>, void>::Result;
		using result = typename IfThenElse < (((int)sizeof...(args) -  num) != 0),
			typename _tls_extract_back_aux<num - 1, list>::result,
			_tls < args... >> ::Result;
	};

	template<int num, typename tls>
	struct tls_extract_back {
		using result = typename IfThenElse <num != 0, typename _tls_extract_back_aux<num, tls>::result, _tls<>>::Result;
	};

	template<int num>
	struct _tls_extract_back_aux<num, _tls<>> {
		using result = _tls<>;
	};

	template<>
	struct _tls_extract_back_aux<-1, void> {
		using result = void;
	};

	template<typename tls1, typename tls2>
	struct tls_cat;

	template<typename... args1, typename... args2>
	struct tls_cat<_tls<args1...>, _tls<args2...>> {
		using result = _tls<args1..., args2...>;
	};

	template<typename tls1, typename arg, typename tls2>
	struct tls_cat3;

	template<typename... args1, typename arg, typename... args2>
	struct tls_cat3<_tls<args1...>, arg, _tls<args2...>> {
		using result = _tls<args1..., arg, args2...>;
	};

	template<typename tls, size_t num, typename T>
	struct replace_typelist {
		static const int dis = IfThenElseN<(tls_size<tls>::size > num), 0, num - tls_size<tls>::size>::Result;
		using result = typename IfThenElse<(tls_size<tls>::size > num),
			typename tls_cat3 < typename tls_extract_front < num, tls>::result,
			T, typename tls_extract_back < tls_size<tls>::size - num - 1, tls>::result>::result,
			typename tls_cat3 < 
			typename tls_extract_front< num , typename tls_cat<tls, typename make_tls_void<dis>::result>::result>::result,
			T, typename tls_extract_back < tls_size<tls>::size - num - 1, typename tls_cat<tls, typename make_tls_void<dis>::result>::result>::result>::result
		>::Result;
		static_assert(num!=2, "!!!!");
	};

}

#endif