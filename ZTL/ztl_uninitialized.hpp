#ifndef ZTL_UNINITIALIZED_H
#define ZTL_UNINITIALIZED_H
#include"ztl_base.hpp"
#include"ztl_construct.hpp"
#include"ztl_algorithm.hpp"
namespace ztl{

	template<typename InputIterator, typename OutputIterator>
	inline OutputIterator _uninitialized_copy_aux(InputIterator first, InputIterator last, OutputIterator destination, __true_type){
		return copy(first, last, destination);
	}

	template<typename InputIterator, typename OutputIterator>
	inline OutputIterator _uninitialized_copy_aux(InputIterator first, InputIterator last, OutputIterator destination, __false_type){
		OutputIterator ret = destination;
		for (; first != last; first++,destination++){
			ztl::construct(&*destination, *first);
		}
		return ret;
	}

	template<typename InputIterator,typename OutputIterator>
	inline OutputIterator uninitialized_copy(InputIterator first, InputIterator last, OutputIterator destination){
		using _typecheck = typename TypeCheck<InputIterator>::BaseT;
		using R = typename IfThenElse<
			TypeCheck<_typecheck>::IsPtr ||
			IsFundaType<_typecheck>::Yes,
			__true_type,
			__false_type
		>::Result;
		return _uninitialized_copy_aux(first, last, destination, R());
	}



	template<typename OutputIterator, typename T>
	inline OutputIterator _uninitialized_fill_aux(OutputIterator first, OutputIterator last, const T& value,__true_type){
		for (OutputIterator it = first; it != last; ++it)
			*it = value;
		return first;
	}

	template<typename OutputIterator, typename T>
	inline OutputIterator _uninitialized_fill_aux(OutputIterator first, OutputIterator last, const T& value, __false_type){
		for (OutputIterator it=first; it != last; ++it)
			construct(&*it, value);
		return first;
	}

	template<typename OutputIterator,typename T>
	inline OutputIterator uninitialized_fill(OutputIterator first, OutputIterator last, const T& value){
		using _typecheck = typename TypeCheck<OutputIterator>::BaseT;
		using R = typename IfThenElse<
			TypeCheck<_typecheck>::IsPtr ||
			IsFundaType<_typecheck>::Yes,
			__true_type,
			__false_type
		>::Result;
		return _uninitialized_fill_aux(first, last, value, R());
	}

	template<typename OutputIterator, typename Size, typename T>
	inline OutputIterator _uninitialized_fill_n_aux(OutputIterator first, Size n, const T& value, __true_type){
		OutputIterator it = first;
		for (Size i = 0; i != n; ++i, ++it)
			*it = value;
		return first;
	}

	template<typename OutputIterator, typename Size, typename T>
	inline OutputIterator _uninitialized_fill_n_aux(OutputIterator first, Size n, const T& value, __false_type){
		OutputIterator it = first;
		for (Size i(); i != n; ++i, ++it)
			construct(&*it, value);
		return first;
	}

	template<typename OutputIterator, typename Size, typename T>
	inline OutputIterator uninitialized_fill_n(OutputIterator first, Size n, const T& value){
		using _typecheck = typename TypeCheck<OutputIterator>::BaseT;
		using R = typename IfThenElse<
			TypeCheck<_typecheck>::IsPtr ||
			IsFundaType<_typecheck>::Yes,
			__true_type,
			__false_type
		>::Result;
		return _uninitialized_fill_n_aux(first, n, value, R());
	}
}

#endif
