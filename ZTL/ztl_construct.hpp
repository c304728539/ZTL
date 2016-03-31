#ifndef ZTL_CONSTRUCT_HPP
#define ZTL_CONSTRUCT_HPP

#include"ztl_base.hpp"
namespace ztl{
	//construct functiion copy from <xmemory0>

	template<typename T1, typename T2>
	inline void construct(T1*_Ptr, T2&& _Val){
		new (_Ptr)T1(ztl::forward<T2>(_Val));
	}

	template<class T1> inline
	void construct(T1 *_Ptr){
		new (_Ptr)T1();	//construct by default constructor
	}

	template<typename T1, typename ...Args> inline
	void construct(T1 *_Ptr, Args&&... args){
		new(_Ptr)T1(args...);	//construct by construct
	}
	
	template<typename T>
	inline void destory(T* p){
		p->~T();
	}

	template<typename InputIterator>
	void _destory_aux(InputIterator first, InputIterator last ,__false_type){
		for (; first != last; ++first)
			destory(&*first);
	}

	template<typename InputIterator>
	inline void _destory_aux(InputIterator first, InputIterator last, __true_type){
		//do nothing
	}

	template<typename InputIterator>
	inline void destory(InputIterator first, InputIterator last){
		typedef typename TypeCheck<InputIterator>::BaseT _typecheck;
		typedef typename IfThenElse<
			TypeCheck<_typecheck>::IsPtr || 
			IsFundaType<_typecheck>::Yes,
				__true_type, 
				__false_type
			>::Result R;
		_destory_aux(first, last, R());
	}
	
	template<typename InputIterator, typename T>
	inline void destory(InputIterator p){
		(&*p)->~T();
	}

}

#endif	//ZTL_CONSTRUCT_HPP
