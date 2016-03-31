#ifndef ZTL_FUNCTION_HPP
#define ZTL_FUNCTION_HPP

#include"ztl_base.hpp"
#include"ztl_tuple.hpp"
namespace ztl {


	template<typename T>
	struct add {
		T operator()(const T& a1, const T& a2) const
		{
			return a1 + a2;
		}
	};

	template<typename T>
	struct less
	{	// functor for operator<
		bool operator()(const T& _Left, const T& _Right) const
		{	// apply operator< to operands
			return (_Left < _Right);
		}
	};

	template<typename T1, typename T2 = T1>
	struct equal {
		bool operator()(const T1& _Left, const T2& _Right) const
		{
			return _Left == _Right;
		}
	};

	template<class T>
	struct identity {
		const T& operator()(const T& rit)const { return rit; }
	};

	template<class T>
	struct select1st {
		typename T::first_type& operator()(const T& rit) const { return rit.first; }
	};

	template<typename Key>
	struct hashfun {};



#define MAKE_HASH(T)\
	template<>\
	struct hashfun<T>{\
		size_t operator()(const T& x) const { return x; }\
	};

	MAKE_HASH(char)
		MAKE_HASH(wchar_t)
		MAKE_HASH(signed char)
		MAKE_HASH(unsigned char)

		/*MAKE_ISFUNDATYPE(int)*/
		MAKE_HASH(signed int)
		MAKE_HASH(unsigned int)
		/*MAKE_ISFUNDATYPE(short)*/
		MAKE_HASH(unsigned short)
		MAKE_HASH(signed short)

#if LONGLONG_EXISTS
		MAKE_HASH(signed long long)
		MAKE_HASH(unsigned long long)
#endif	//LONGLONG_EXISTS

#undef MAKE_HASH

		inline size_t __zstl_string_hash(const char* s) {
		size_t ret;
		for (; *s; ++s)
			ret = ret * 5 + *s;
		return ret;
	}

	template<>
	struct hashfun<char*> {
		size_t operator()(const char* x) const { return __zstl_string_hash(x); }
	};

	template<>
	struct hashfun<const char*> {
		size_t operator()(const char* x) const { return __zstl_string_hash(x); }
	};

	template<typename _RT, typename... args>
	class FunctionBase
	{
	public:
		typedef _RT(*PtrFuncType)(args...);
		virtual PtrFuncType GetFunc() = 0;
		virtual _RT operator()(args...) = 0;
	};


	template<typename _RT, typename _Class, typename... args>
	class FunctionOfClass : public FunctionBase<_RT, args...>
	{
		using base = FunctionBase<_RT, args...>;
		using PtrFuncType = typename base::PtrFuncType;
	public:
		typedef _RT(_Class::*MemFuncType)(args...);
		MemFuncType m_pfunc;
		_Class* m_obj;
		FunctionOfClass(MemFuncType func, _Class* obj)
		{
			m_pfunc = func;
			m_obj = obj;
		}
		virtual PtrFuncType GetFunc()
		{
			return (PtrFuncType)(*(long*)&m_pfunc);

		}

		virtual _RT operator()(args... _args)
		{
			return (m_obj->*m_pfunc)(_args...);
		}
	};

	template<typename _RT, typename... args>
	class FunctionOfPtr : public FunctionBase<_RT, args...>
	{
		using base = FunctionBase<_RT, args...>;
		using PtrFuncType = typename base::PtrFuncType;
	public:
		PtrFuncType m_pfunc;
		FunctionOfPtr(PtrFuncType pf)
		{
			m_pfunc = pf;
		}

		virtual PtrFuncType GetFunc()
		{
			return m_pfunc;
		}

		virtual _RT operator()(args... _args)
		{
			return (*m_pfunc)(_args...);
		}
	};

	template <class T>
	class function;     // undefined 

	template<typename... args>
	struct TypeList {
		static const size_t num = sizeof...(args);
	};

	template<typename _RT, typename... args>
	class function<_RT(args...)>
	{
	public:
		using argsList = TypeList<args...>;
		using retT = _RT;
		FunctionBase<_RT, args...>* m_pfunc;
		function() : m_pfunc(nullptr)
		{}

		~function()
		{
			if (m_pfunc != nullptr)
			{
				delete m_pfunc;
				m_pfunc = nullptr;
			}
		}

		template<typename _Class>
		function(_RT(_Class::*pMemFunc)(args...), _Class* obj)
		{
			m_pfunc = new FunctionOfClass<_RT, _Class, args...>(pMemFunc, obj);
		}

	function(_RT(*pPtrFunc)(args...))
		{
			m_pfunc = new FunctionOfPtr<_RT, args...>(pPtrFunc);
		}

		_RT operator()(args... _args)
		{
			return (*m_pfunc)(_args...);
		}

		function& operator=(function& f)
		{
			m_pfunc = f.m_pfunc;
			f.m_pfunc = nullptr;
			return *this;
		}
	};

}

#endif	//ZTL_FUNCTION_HPP
