#ifndef ZTL_TUPLE_HPP
#define ZTL_TUPLE_HPP

#include"ztl_algorithm.hpp"
#include"ztl_template.hpp"
namespace ztl{
	template<typename... typelist>
	class tuple;

	template<>
	class tuple<>{
	public:
		using type = void;
	public:
		void swap(tuple<>){}
		tuple(){}
	};

	template<>
	class tuple<void>{
	public:
		tuple() = delete;
	};

	template<typename _This, typename... _Other>
	class tuple<_This, _Other...> :public tuple < _Other... >
	{
	public:
		using base = tuple<_Other...>;
		using type = _This;
		using self = tuple<_This, _Other...>;
	private:
		_This x;
	public:
		tuple(){}
		explicit tuple(_This _x, _Other... _other_arg) :x(_x), base(_other_arg...){}

		const self& operator=(const self& _rhs){
			x = _rhs.x;
			base::operator=((base&)_rhs);
			return *this;
		}

		void swap(self& _rhs){
			ztl::swap(x, _rhs.x);
			((base&)*this).swap((base&)_rhs);
		}

	public:
		operator _This&(){
			return x;
		}
	};

	template<typename T>
	struct tuple_size;

	template<typename... typelist>
	struct tuple_size <tuple<typelist...>> {
		static const size_t value = sizeof...(typelist);
	};

	template<>
	struct tuple_size <tuple<>> {
		static const size_t value = 0;
	};

	template<typename idx1, typename idx2, size_t args_num, size_t m, typename... Tuples>
	struct _tuple_cat;

	template<size_t... args1, size_t... args2, size_t args_num, size_t m, typename ...typelist, typename ...rest>
	struct _tuple_cat <_idx<args1...>, _idx<args2...>, args_num, m, tuple<typelist...>, rest...> {
		using idx1 = typename cat_idx<_idx<args1...>, typename make_idx_seq<0, sizeof...(typelist) - 1, 1>::result>::result;
		using idx2 = typename cat_idx<_idx<args2...>, typename make_idx_num<m, sizeof...(typelist)>::result>::result;

		using result = typename _tuple_cat<idx1, idx2, args_num, m + 1, rest..., typelist...>::result;
		using idx_seq = typename _tuple_cat<idx1, idx2, args_num, m + 1, rest..., typelist...>::idx_seq;
		using idx_num = typename _tuple_cat<idx1, idx2, args_num, m + 1, rest..., typelist...>::idx_num;
	};

	template<size_t... args1, size_t... args2, size_t args_num, typename ...rest>
	struct _tuple_cat <_idx<args1...>, _idx<args2...>, args_num, args_num, rest...> {
		using idx_seq = _idx<args1...>;
		using idx_num =_idx<args2...>;
		using result = tuple<rest...>;
	};

	template<size_t n, typename _This, typename... _Other>
	struct _tuple_base{
		using Base = typename _tuple_base<n - 1, _Other...>::Base;
	};

	//template<typename _This, typename... _Other>
	//struct _tuple_base<1, _This, _Other...>{
	//	using Base = tuple<_Other...>;
	//};

	template<typename... _Other>
	struct _tuple_base<0, _Other...> {
		using Base = tuple<_Other...>;
	};

	template<size_t n, typename _tuple_type>
	struct tuple_element;

	template<size_t n, typename... typelist>
	struct tuple_element<n, tuple<typelist...>>{
		using type = typename tuple_element<n - 1, typename tuple<typelist...>::base>::type;
	};

	template<typename... typelist>
	struct tuple_element<0, tuple<typelist...>>{
		using type = typename tuple<typelist...>::type;
	};

	template<size_t n, typename... typelist>
	inline auto get(tuple<typelist...>& _t)->typename tuple_element<n, tuple<typelist...>>::type&{
		//return (typename tuple_element<n, tuple<typelist...>>::type&)_t;	//this can work normally only when all types of typelist are different 
		//return _t.tuple_base<n, typelist...>::Base::operator tuple_element<n, tuple<typelist...>>::type&();	//error C1001
		using type = typename tuple_element<n, tuple<typelist...>>::type;
		return static_cast<typename _tuple_base<n, typelist...>::Base>(_t).operator type&();
	}

	template<typename... typelist>
	tuple<typelist...> make_tuple(typelist&&... _arg_list){
		return tuple<remove_reference<typelist>...>(ztl::forward(_arg_list)...);
	}

	template<class... typelist>
	tuple<typelist&&...> forward_as_tuple(typelist&&... args)
	{
		return tuple<typelist&&...>(std::forward<typelist>(args)...);
	}

	struct 	IgnoreType{
		IgnoreType() = default;

		template<class T>
		void operator=(const T&) const
		{	// do nothing
		}
	};

	const IgnoreType ignore;

	template<typename... typelist> 
	inline tuple<typelist&...> tie(typelist&... _Args)
	{
		return (tuple<typelist&...>(_Args...));
	}


	template<size_t... size>
	struct _get_index{
	};

	template<size_t size>
	struct _get_index<size>{
		using result = typename _get_index<size - 2,size - 1>::result;
	};

	template<>
	struct _get_index<1>{
		using result = _get_index<0>;
	};

	template<>
	struct _get_index<2>{
		using result = _get_index<0,1>;
	};

	template<size_t m, size_t... size>
	struct _get_index<m ,size...>{
		using result = _get_index<m - 1, m, size...>;
	};

	template<size_t... size>
	struct _get_index<0, size...>{
	};

	template<typename _RT, typename Tuples, size_t... idx_seq, size_t... idx_num>
	_RT _tuple_cat_aux(Tuples&& tuples, _idx<idx_seq...>, _idx<idx_num...>){
		return _RT(get<idx_seq>(get<idx_num>(tuples))...);
	}

	template<typename... Tuples>
	auto tuple_cat(Tuples&&... tuples)->typename _tuple_cat<_idx<>, _idx<>, sizeof...(Tuples), 0, typename remove_reference<Tuples>::type...>::result{
		using r = _tuple_cat<_idx<>, _idx<>, sizeof...(Tuples), 0, typename remove_reference<Tuples>::type...>;
		return _tuple_cat_aux<typename r::result>(ztl::forward_as_tuple(ztl::forward<Tuples>(tuples)...), typename r::idx_seq(), typename r::idx_num());
	}
}

#endif	//ZTL_TUPLE_HPP
