#ifndef ZTL_VEBTREE_HPP
#define ZTL_VEBTREE_HPP

#include"ztl_construct.hpp"
#include"ztl_allocator.hpp"
#include"ztl_algorithm.hpp"
#include"ztl_template.hpp"
#include"ztl_functional.hpp"
#include"ztl_iterator.hpp"

namespace ztl{


	template<unsigned int u>
	struct __Divide_2_Up_U_For_vEBtree{
		enum{result = (u+1)/2};
	};

	template<unsigned int u>
	struct __Divide_2_Down_U_For_vEBtree{
		enum{ result = u / 2 };
	};

//#define vEB_s(n) Pow<2,n>::result

/*	const size_t vEBtree_size[] = {
		2,
		4,
		8,
		16,
		32,
		64,
		128,
		256,
		512,
		1024,
		2048,
		4096,
		8192,
		16384,
		32768,
		65536,
	};*/

	constexpr size_t vEBtree_size(size_t n){
		return n?vEBtree_size(n - 1)*2:1;
	}

	template<size_t u, template<typename> class Alloc>
	class vEBtree;

	template<size_t u, template<typename> class Alloc>
	struct __vEBtree_iterator{
		using value_type = size_t;
		using pointer = value_type*;
		using tree = vEBtree<u, Alloc>;
		using tree_pointer = tree*;
		using self = __vEBtree_iterator;
		using iterator_category = bidirectional_iterator_tag;
	private:
		tree_pointer _ptree;
		value_type _value;

	public:
		__vEBtree_iterator(tree_pointer ptree = nullptr, value_type value = u)
			:_ptree(ptree), _value(value > u){}

		value_type& operator*(){
			return _value;
		}

		self& operator++(){
			_value = _ptree->_node->successor(_value);
			if (_value > u)_value = u;
			return *this;
		}

		self operator++(int){
			self ret = *this;
			++*this;
			return ret;
		}

		self& operator--(){
			_value = _ptree->_node->predecessor(_value);
			if (_value > u)_value = u;
			return *this;
		}

		self operator--(int){
			self ret = *this;
			--*this;
			return ret;
		}

		bool operator==(const self& rit){ return _value == rit._value; }
		bool operator!=(const self& rit){ return _value != rit._value; }

	};



	template<size_t u, template<typename> class Alloc>
	struct __vEBtree_node{
		//enum { sqrt_u = Sqrt<u>::result };
		enum { sqrt_u_up = Pow<2,__Divide_2_Up_U_For_vEBtree<Log2<u>::result>::result>::result };
		enum { sqrt_u_down = Pow<2, __Divide_2_Down_U_For_vEBtree<Log2<u>::result>::result>::result };
		enum { nil = MAX_SIZE_T };

		using value_type = size_t;
		using pointer = value_type*;
		using sqrt_down_vBE_node = __vEBtree_node<sqrt_u_down, Alloc>;
		using sqrt_up_vEB_node = __vEBtree_node<sqrt_u_up, Alloc>;
		using sqrt_vBE_node_pointer = sqrt_down_vBE_node*;

		value_type _min, _max;
		sqrt_down_vBE_node _cluster[sqrt_u_up];
		sqrt_up_vEB_node _summary;
		
	private:
		value_type high(value_type x){
			return x / sqrt_u_down;
		}

		value_type low(value_type x){
			return x%sqrt_u_down;
		}

		value_type index(value_type num_cluster, value_type offset){
			return num_cluster*sqrt_u_down + offset;
		}

		bool empty() const{ return _min == nil; }

	public:	
		__vEBtree_node() :_min(nil), _max(nil){
		}

		~__vEBtree_node(){
		}

		value_type minimum() { return _min; }

		value_type maximum() { return _max; }
		
		void insert_base(const value_type& x){
			_min = x;
			_max = x;
		}
		
		bool member(const value_type& x){
			if (empty()) return false;
			if (x == _min || x == _max)
				return true;
			else
				return _cluster[high(x)].member(low(x));
		}

		value_type successor(const value_type& x){
			if (_min!=nil && x<_min){
				return _min;
			}
			else{
				value_type max_low = _cluster[high(x)].maximum();
				if (max_low != nil && low(x) < max_low){
					auto offset = _cluster[high(x)].successor(low(x));
					return index(high(x),offset);
				}
				else{
					value_type succ_cluster = _summary.successor(high(x));
					if (succ_cluster > u/2)
						return nil;
					else{
						auto offset = _cluster[succ_cluster].minimum();
						return index(succ_cluster, offset);
					}
				}
			}
		}

		value_type predecessor(const value_type& x){
			if (_max != u && x > _max)
				return _max;
			else{
				auto min_low = _cluster[high(x)].minimum();
				if (min_low != u && low(x) > min_low){
					auto offset = _cluster[high(x)].predecessor(low(x));
					return index(high(x), offset);
				}
				else{
					value_type pred_cluster = _summary.predecessor(high(x));
					if (pred_cluster == u)
					if (_min != nil && x > _min)
						return _min;
					else
						return nil;
					else{
						auto offset = _cluster[pred_cluster].maximum();
						return index(pred_cluster, offset);
					}
				}
			}
		}

		void insert(value_type x){
			if (empty()){
				insert_base(x);
			}
			else {
				if (x < _min)
					ztl::swap(x, _min);
				if (_cluster[high(x)].minimum() == nil){
					_summary.insert(high(x));
					_cluster[high(x)].insert_base(low(x));
				}
				else
					_cluster[high(x)].insert(low(x));
				if (x>_max)
					_max = x;
			}
		}

		void erase(const value_type x){
			if (_min == _max)
				_min = _max = nil;
			else {
				if (x == _min){
					value_type first_cluster = _summary.minimum();
					value_type x = index(first_cluster, _cluster[first_cluster].minimum());
					_min = x;
				}
				_cluster[high(x)].erase(low(x));
				if (_cluster[high(x)].minimum() == nil){
					_summary.erase(high(x));
					if (x == _max){
						value_type summary_max = _summary.maximum();
						if (summary_max == nil)
							_max = _min;
						else{
							_max = index(summary_max, _cluster[summary_max].maximum());
						}
					}
				}
				else if (x == _max){
					_max = index(high(x), _cluster[high(x)].maximum());
				}
				
			}
		}
		
	};

	template<template<typename> class Alloc>
	struct __vEBtree_node<2,Alloc>{
		friend struct __vEBtree_node<4, Alloc>;

		enum { sqrt_u = 0 };
		enum {nil = MAX_SIZE_T};
		typedef size_t value_type;
		typedef value_type* pointer;
		//typedef __vEBtree_node<sqrt_u, Alloc> sqrt_vBE_node;
		//typedef sqrt_vBE_node* sqrt_vBE_node_pointer;

		value_type _min, _max;
		
	private:		

		bool empty(){ return _min == nil; }

	public:
		__vEBtree_node() :_min(nil), _max(nil){}
		~__vEBtree_node(){}

		void insert_base(const value_type& x){
			_min = x;
			_max = x;
		}
		value_type minimum(){
			return _min;
		}

		value_type maximum(){
			return _max;
		}

		bool member(const value_type& x){
			if (empty()) return false;
			if (x == _min || x == _max)
				return true;
			else
				return false;
		}

		value_type successor(const value_type& x){
			if (x == 0 && _max == 1)
				return _max;
			else
				return nil;
		}

		value_type predecessor(const value_type& x){
			if (x == 1 && _min == 0)
				return 0;
			else
				return nil;
		}

		bool insert(value_type x){
			if (empty()){
				insert_base(x);
			}
			else {
				if (x < _min)
					ztl::swap(x, _min);
				if (x > _max)
					_max = x;
			}
			return true;
		}
		
		void erase(const value_type x){
			if (_min == _max)
				_min = _max = nil;
			else{
				if (x == 0)
					_min = 1;
				else
					_min = 0;
				_max = _min;
			}
		}

	};

	template<size_t u, template<typename> class Alloc = Allocator>
	class vEBtree{

		static_assert(u <= 65536, "the size of vEBtree shoule be not great than 2^16");

		friend struct __vEBtree_iterator<u, Alloc>;

		//enum { sqrt_u = Sqrt<u>::result };
	public:
		typedef size_t value_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;

		typedef __vEBtree_node<u, Alloc> node_type;
		typedef node_type* node_pointer;

		typedef __vEBtree_iterator<u,Alloc> iterator;

	private:
		Alloc<value_type> _alloc;
		Alloc<node_type> _alloc_node;
		node_pointer _node;
	private:
		node_pointer new_node(){
			auto ret = _alloc_node.allocate(1);
			construct(ret, node_type());
			return ret;
		}

		

	public:
		vEBtree(){ 
			_node = new_node(); 
		}

		//template<size_t con_u>
		//_vEBtree(__vEBtree_node<u, Alloc>* pnode):_node(pnode){
//
//		}

		~vEBtree(){
			_alloc_node.deallocate(_node,1);
		}

		bool empty(){
			return _node->minimum() > u;
		}

		iterator begin(){
			return iterator(this, _node->minimum() < u ? _node->minimum() : u);
		}

		iterator end(){
			return iterator(this, u);
		}

		value_type minimum() const {
			return _node->minimum();
		}
		
		value_type maximum() const {
			return _node->maximum();
		}
	
		bool member(value_type x){
			return _node->member(x);
		}
		


		bool insert(value_type x){
			if (member(x))
				return false;
			_node->insert(x);
			return true;
		}

		void clear(){

		}

		bool erase(value_type x){
			if (!member(x))
				return false;
			_node->erase(x);
			return true;
		}
	};

}

#endif	//ZTL_VEBTREE_HPP