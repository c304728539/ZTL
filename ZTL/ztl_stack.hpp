#ifndef ZTL_STACK_HPP
#define ZTL_STACK_HPP

#include"ztl_vector.hpp"


namespace ztl{

	template<typename T,typename Container = vector<T>>
	class stack{
		typedef T value_type;
		typedef value_type& reference;
		typedef value_type* pointer;
		typedef const value_type& const_reference;
		typedef const value_type* const_pointer;
		typedef size_t size_type;

		

	private:
		Container _imp;

	public:
		stack(){}

		~stack(){
			_imp.clear();
		}

		//void clear(){ _imp.clear(); }

		void push(const value_type& x){ _imp.push_back(x); }

		const value_type& top(){ return *(_imp.end()-1); }

		void pop(){ return _imp.pop_back(); }

		value_type&& _pop(){
			auto ret = ztl::move(*(_imp.end() - 1));
			pop();
			return ret;
		}

		size_type size(){ return _imp.size(); }

		bool empty(){ return _imp.empty(); }

	};

}

#endif	//ZTL_STACK_HPP