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
		using Result = T1;
	};
	 
	template<typename T1, typename T2>
	class IfThenElse<false, T1, T2>{
	public:
		using Result = T2;
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
		using one = char ;
		using two = struct{ char a[2]; } ;
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
		using BaseT = T;
		using BottomT = T;
		using ClassT = void;
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
		using BaseT = T;
		using BottomT = typename TypeCheck<T>::BottomT;
		using ClassT = void;
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
		using BaseT = T;
		using BottomT = typename TypeCheck<T>::BottomT;
		using ClassT = void;
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
		using BaseT = T;
		using BottomT = typename TypeCheck<T>::BottomT;
		using ClassT = void;
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
		using BaseT = T;
		using BottomT = typename TypeCheck<T>::BottomT;
		using ClassT = void;
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
		using BaseT = T;
		using BottomT = typename TypeCheck<T>::BottomT;
		using ClassT = C;
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
		using type = T;
	};

	template<class T>
	struct remove_reference<T&>
	{	
		using type = T;
	};

	template<class T>
	struct remove_reference<T&&>
	{	
		using type = T;
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
		using self = pair<T1, T2>;
		using first_type = T1;
		using second_type = T2;

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
