#ifndef ZTL_BASE_H
#define ZTL_BASE_H

#include <cstring>
#include <initializer_list>
#include<climits>
namespace ztl{

	const size_t MAX_SIZE_T = -1;
	const int MAX_INT = INT_MAX;

	template<bool r,typename T1,typename T2>
	class IfThenElse;

	template<typename T1, typename T2>
	class IfThenElse<true, T1, T2>{
	public:
		typedef T1 Result;
	};
	 
	template<typename T1, typename T2>
	class IfThenElse<false, T1, T2>{
	public:
		typedef T2 Result;
	};

	struct __true_type{
		static const bool value = true;
	};
	struct __false_type{
		static const bool value = false;
	};
	


	template<typename T>
	class IsFunction{
	private:
		typedef char one;
		typedef struct{ char a[2]; } two;
		template<typename U> static one test(...);
		template<typename U> static two test(U(*)[1]);
	public:
		enum{ Yes = (sizeof(test<T>(0))==1) };
		enum{ No=!Yes };
	};

	template<typename T>
	class TypeCheck{
	public:
		enum{
			IsPtr = 0,
			IsRef = 0,
			IsArray = 0,
			IsFunc = IsFunction<T>::Yes,
			IsPtemem = 0
		};
		typedef T BaseT;
		typedef T BottomT;
		typedef void ClassT;
	};

	template<typename T>
	class TypeCheck<T&>{
	public:
		enum{
			IsPtr = 0,
			IsRef = 1,
			IsArray = 0,
			IsFunc = 0,
			IsPtemem = 0
		};
		typedef T BaseT;
		typedef typename TypeCheck<T>::BottomT BottomT;
		typedef void ClassT;
	};

	template<typename T>
	class TypeCheck<T*>{
	public:
		enum{
			IsPtr = 1,
			IsRef = 0,
			IsArray = 0,
			IsFunc = 0,
			IsPtemem = 0
		};
		typedef T BaseT;
		typedef typename TypeCheck<T>::BottomT BottomT;
		typedef void ClassT;
	};

	template<typename T,int N>
	class TypeCheck<T[N]>{
	public:
		enum{
			IsPtr = 0,
			IsRef = 0,
			IsArray = 1,
			IsFunc = 0,
			IsPtemem = 0
		};
		typedef T BaseT;
		typedef typename TypeCheck<T>::BottomT BottomT;
		typedef void ClassT;
	};

	template<typename T>
	class TypeCheck<T[]>{
	public:
		enum{
			IsPtr = 0,
			IsRef = 0,
			IsArray = 1,
			IsFunc = 0,
			IsPtemem = 0
		};
		typedef T BaseT;
		typedef typename TypeCheck<T>::Bottom BottomT;
		typedef void ClassT;
	};

	template<typename T,typename C>
	class TypeCheck<T C::*>{
	public:
		enum{
			IsPtr = 0,
			IsRef = 0,
			IsArray = 0,
			IsFunc = 0,
			IsPtemem = 1
		};
		typedef T BaseT;
		typedef typename TypeCheck<T>::Bottom BottomT;
		typedef C ClassT;
	};

	template<typename T>
	class IsFundaType{
	public:
		enum { Yes = 0, No = 1 };
	};



#define MAKE_ISFUNDATYPE(T)	\
	template<> \
	struct IsFundaType<T>{	\
			enum {Yes = 1,No = 0};	\
	};

	MAKE_ISFUNDATYPE(void)
	MAKE_ISFUNDATYPE(bool)

	MAKE_ISFUNDATYPE(char)
	MAKE_ISFUNDATYPE(wchar_t)
	MAKE_ISFUNDATYPE(signed char)
	MAKE_ISFUNDATYPE(unsigned char)
	
	/*MAKE_ISFUNDATYPE(int)*/
	MAKE_ISFUNDATYPE(signed int)
	MAKE_ISFUNDATYPE(unsigned int)

	/*MAKE_ISFUNDATYPE(short)*/
	MAKE_ISFUNDATYPE(unsigned short)
	MAKE_ISFUNDATYPE(signed short)

	MAKE_ISFUNDATYPE(float)
	MAKE_ISFUNDATYPE(double)
	MAKE_ISFUNDATYPE(long double)

#if LONGLONG_EXISTS
	MAKE_ISFUNDATYPE(signed long long)
	MAKE_ISFUNDATYPE(unsigned long long)
#endif	//LONGLONG_EXISTS

#if _DEBUG
#define CHECK_POINTER(p) 
#else
#define CHECK_POINTER(p) 
#endif

#undef MAKE_ISFUNDATYPE

	template<class T>
	struct remove_reference
	{
		typedef T type;
	};

	template<class T>
	struct remove_reference<T&>
	{	
		typedef T type;
	};

	template<class T>
	struct remove_reference<T&&>
	{	
		typedef T type;
	};

	template<class T> inline
	typename remove_reference<T>::type&& move(T&& _Arg)
	{
		return static_cast<typename remove_reference<T>::type&&>(_Arg);
	}

	template<class T> inline
	T&& forward(typename remove_reference<T>::type& _Arg)
	{
		return (static_cast<T&&>(_Arg));
	}

	template<class T> inline
	T&& forward(typename remove_reference<T>::type&& _Arg) 
	{	
		return (static_cast<T&&>(_Arg));
	}

	template<typename T1,typename T2>
	struct pair
	{	// copy from <utility>
		typedef pair<T1, T2> self;
		typedef T1 first_type;
		typedef T2 second_type;

		first_type first;
		second_type second;

		pair()
			: first(), second()
		{	
		}

		pair(const T1& _Val1, const T2& _Val2)
			: first(_Val1), second(_Val2)
		{
		}

		pair& operator=(const pair& rit){
			first = rit.first;
			second = rit.second;
			return *this;
		}

		operator pair<const first_type, second_type>(){
			return pair<const first_type, second_type>(first, second);
		}
	};

	template<typename T1,typename T2>
	pair<T1,T2> make_pair(T1 x1,T2 x2){
		return pair<T1,T2>(x1, x2);
	}


#if 0

#ifdef _WIN32
#define _atomic_addition InterlockedExchangeAdd
#define _atomic_increase InterlockedIncrement
#define _atomic_decrease InterlockedDecrement
#define _atomic_exchange InterlockedExchange

#endif

#ifdef _WIN64
#define _atomic_addition InterlockedExchangeAdd64
#define _atomic_increase InterlockedIncrement64
#define _atomic_decrease InterlockedDecrement64
#define _atomic_exchange InterlockedExchange64

#endif

#endif

#define ztl_max(a,b) ((a)>(b)?(a):(b))
#define ztl_min(a,b) ((a)<(b)?(a):(b))

}


#endif	//ZTL_BASE_H
