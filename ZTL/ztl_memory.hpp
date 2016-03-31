#ifndef ZTL_MEMORY_HPP
#define ZTL_MEMORY_HPP

#include<exception>
#include<atomic>
#include"ztl_base.hpp"
#include"ztl_allocator.hpp"
#include"ztl_string.hpp"
#include<utility>
namespace ztl{
	template<typename T>
	class default_delete{
	public:
		void operator()(T* p){
			delete p;
		}
	};

	template<typename T>
	class _deleter_allocator_default{
	public:
		void operator()(T* p){
			Allocator<T> alloc;
			alloc.deallocate(p, 1);
		}
	};
	
	template<typename T>
	class shared_ptr;

	template<typename T>
	class _Ref_Count{
		class _deleter_base{
		public:
			virtual void operator()() = 0;
			virtual void* get_deleter() = 0;
		};

		template<typename _DT>
		class _deleter :public _deleter_base{
		public:
			_deleter(T* p, _DT dt) :_p(p), _dt(dt){}

			virtual void operator()() override {
				_dt(_p);
			}

			virtual void* get_deleter() override { return static_cast<void*>(&_dt); }
		private:
			_DT _dt;
			T* _p;
		};

	public:
		typedef T element_type;

		explicit _Ref_Count(element_type* p):
			_p_real(p),
			__deleter(new _deleter<default_delete<T>>(p, default_delete<T>()))
		{

		}

		template<typename DT>
		_Ref_Count(element_type* p, DT& dt) :
			_p_real(p),
			__deleter(new _deleter<DT>(p, dt))
		{
			
		}
		
		
		template<typename T0, typename DT>
		_Ref_Count(const shared_ptr<T0>& p0, element_type* p, DT& dt) :
			_p_real(p),
			__deleter(new _deleter<DT>(p0.get(), dt))
		{

		}

		~_Ref_Count(){

		}

		void Increase_Ref_Count_Real(){ ++_ref_count_real; }

		void Increase_Ref_Count_Self(){ ++_ref_count_self; }

		void Decrease_Ref_Count_Real(){
			if (0 == --_ref_count_real){
				__deleter->operator()();
				delete __deleter;
			}
		}
		
		void Decrease_Ref_Count_Self(){
			if (0 == --_ref_count_self)
				delete this;
		}

		element_type* get(){ return _p_real; }

		void* get_deleter() {return __deleter->get_deleter(); }

		void construct(){
			Increase_Ref_Count_Real();
			Increase_Ref_Count_Self();
		}

		void destruct(){
			Decrease_Ref_Count_Real();
			Decrease_Ref_Count_Self();
		}

		unsigned int use_count(){ return _ref_count_real; }

	private:
		element_type* _p_real;
		std::atomic_long _ref_count_real = 1;
		std::atomic_long _ref_count_self = 1;
		_deleter_base* __deleter;
	};

	class bad_weak_ptr: public std::exception{
	public:
		const char* what() const throw() { return "std::bad_weak_ptr"; }
	};

	template<typename T>
	class weak_ptr{
	public:
		typedef T element_type;
		typedef weak_ptr self;

	public:
		weak_ptr():_ref_count(nullptr){

		}

		explicit weak_ptr(const shared_ptr<T>& p) :_ref_count(p._ref_count)
		{
			_ref_count->Increase_Ref_Count_Self();
		}

		weak_ptr(const self& p) :_ref_count(p._ref_count){
			_ref_count->Increase_Ref_Count_Self();
		}

		const self& operator=(const self& rhs){
			_ref_count = rhs._ref_count;
			_ref_count->Increase_Ref_Count_Self();
		}

		const self& operator=(const shared_ptr<T>& rhs){
			_ref_count = rhs._ref_count;
			_ref_count->Increase_Ref_Count_Self();
			return *this;
		}

		unsigned int use_count(){ return _ref_count->use_count(); }

		~weak_ptr(){
			_ref_count->Decrease_Ref_Count_Self();
		}
	private:
		_Ref_Count<T>* _ref_count;
		
	};




	template<typename T>
	class shared_ptr{
		template<typename _T>
		friend class shared_ptr;

		friend class weak_ptr<T>;
	public:

		template< class Deleter>
		friend Deleter* get_deleter(const shared_ptr<T>& p){
			return static_cast<Deleter*>(p._ref_count->get_deleter());
		}

		using element_type = T;
		using self = shared_ptr;

	public:
		shared_ptr() :_ref_count(nullptr)
		{

		}

		explicit shared_ptr(T* p) :_ref_count(new _Ref_Count<element_type>(p))
		{
			
		}

		template<typename DT>
		shared_ptr(T* p, DT& dt) : _ref_count(new _Ref_Count<element_type>(p, dt))
		{
		}

		template<typename T0>
		shared_ptr(const shared_ptr<T0>& ptr, T* p) : _ref_count(new _Ref_Count<element_type>(ptr, p))
		{
	
		}

		template <class U> 
		explicit shared_ptr(const weak_ptr<U>& x):_ref_count(nullptr){
			if (0 == x.use_count())
				throw bad_weak_ptr();
			else
				_init(_ref_count = x._ref_count);
		}
		
		shared_ptr(const self& rhs){
			_init(_ref_count = rhs._ref_count);
		}

		shared_ptr(self&& rhs){
			_init(_ref_count = rhs._ref_count);
			rhs._detr = nullptr;
			rhs._count = nullptr;
			rhs._p = nullptr;
		}

		self& operator=(const self& rhs){
			_destruct();
			_init(_ref_count = rhs._ref_count);
		}

		~shared_ptr(){
			_destruct();
		}

		void reset(){
			_destruct();
		}

		void reset(element_type* p){
			_destruct();
			_ref_count = new _Ref_Count<T>(p);
		}

		template<typename DT>
		void reset(element_type* p, DT& deleter){
			_destruct();
			_ref_count = new _Ref_Count<T>(p,deleter);
		}

		element_type* get() const { return _ref_count->get(); }

		element_type& operator*() const { return *_ref_count->get(); }

		element_type* operator->() const { return _ref_count->get(); }

		unsigned int use_count(){ return _ref_count->use_count(); }

		bool unique() const {
			return _ref_count->use_count() == 1 ? true : false;
		}

		operator bool() const {
			return static_cast<bool>(_ref_count->get() == nullptr);
		}

	private:

		_Ref_Count<element_type>* _ref_count;

		void _init(const _Ref_Count<element_type>* rhs){
			rhs->construct();
		}

		void _destruct(){
			_ref_count->destruct();
			_ref_count = nullptr;
		}

	};
	
	template < class T, class U >
	bool operator==(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs){
		return lhs.get() == rhs.get();
	}

	template < class T, class U >
	bool operator!=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs){
		return !(lhs == rhs);
	}

	template< class T, class U >
	bool operator<(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs){
		return less<U>()(lhs.get(), rhs.get());
	}

	template< class T, class U >
	bool operator>(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs){
		return rhs < lhs;
	}


	template< class T, class U >
	bool operator<=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs){
		return !(lhs > rhs);
	}

	template< class T, class U >
	bool operator>=(const shared_ptr<T>& lhs, const shared_ptr<U>& rhs){
		return !(lhs < rhs);
	}

	template< class T >
	bool operator==(const shared_ptr<T>& lhs, nullptr_t rhs){
		return lhs.get() == rhs;
	}

	template< class T >
	bool operator==(nullptr_t lhs, const shared_ptr<T>& rhs){
		return lhs == rhs.get();
	}

	template< class T >
	bool operator!=(const shared_ptr<T>& lhs, nullptr_t rhs){
		return lhs.get() != rhs;
	}

	template< class T >
	bool operator!=(nullptr_t lhs, const shared_ptr<T>& rhs){
		return lhs != rhs.get();
	}

	template< class T >
	bool operator<(const shared_ptr<T>& lhs, nullptr_t rhs){
		return less<T>()(lhs.get(), rhs);
	}

	template< class T >
	bool operator<(nullptr_t lhs, const shared_ptr<T>& rhs){
		return less<T>()(lhs, rhs.get());
	}

	template< class T >
	bool operator>(const shared_ptr<T>& lhs, nullptr_t rhs){
		return rhs < lhs;
	}

	template< class T >
	bool operator>(nullptr_t lhs, const shared_ptr<T>& rhs){
		return rhs < lhs;
	}

	template< class T >
	bool operator<=(const shared_ptr<T>& lhs, nullptr_t rhs){
		return !(lhs > rhs);
	}

	template< class T >
	bool operator<=(nullptr_t lhs, const shared_ptr<T>& rhs){
		return !(lhs > rhs);
	}

	template< class T >
	bool operator>=(const shared_ptr<T>& lhs, nullptr_t rhs){
		return !(lhs < rhs);
	}

	template< class T >
	bool operator>=(nullptr_t lhs, const shared_ptr<T>& rhs){
		return !(lhs < rhs);
	}

}



#endif	//ZTL_MEMORY_HPP
