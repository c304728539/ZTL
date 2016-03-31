#ifndef ZTL_FIBONACCI_HEAP_HPP
#define ZTL_FIBONACCI_HEAP_HPP

#include<cmath>

#include"ztl_vector.hpp"
#include"ztl_list.hpp"
#include"ztl_functional.hpp"
#include"ztl_construct.hpp"

namespace ztl{

	template<typename T, typename Key, typename ExtractKey, typename Compare, template<typename> class Alloc>
	class fibonacci_heap;

	template<typename T>
	struct __fibonacci_heap_node;

	template<typename T>
	struct __fibonacci_heap_iterator{
		template<typename _T, typename Key, typename ExtractKey, typename Compare, template<typename> class Alloc>
		friend class fibonacci_heap;


		typedef list<__fibonacci_heap_node<T>*> node_list;
		typedef node_list* list_pointer;
		typedef typename node_list::iterator list_iterator;
		typedef __fibonacci_heap_iterator iterator;
		typedef __fibonacci_heap_iterator self;

		typedef forward_iterator_tag iterator_category;

		typedef T value_type;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
	private:
		list_iterator list_it;
		list_pointer plist;

	public:
		__fibonacci_heap_iterator(const list_iterator& it, list_pointer plist) :
			list_it(it), plist(plist){}

		__fibonacci_heap_iterator(const self& rit) :
			list_it(rit.list_it), plist(rit.plist){}

		self& operator=(const self& rit){
			list_it = rit.list_it;
			plist = rit.plist;
			return *this;
		}

		reference operator*(){
			return (*list_it)->element;
		}

		const_reference operator*() const {
			return static_cast<const_reference>((*list_it)->element);
		}

		pointer operator->(){
			return &(*list_it)->element;
		}

		const_pointer operator->() const {
			return static_cast<const_pointer>(&(*list_it)->element);
		}

		iterator operator++(){
			list_iterator copy = list_it;
			if ((*copy)->child.size()){
				plist = &(*copy)->child;
				list_it = (*copy)->child.begin();
				return *this;
			}
			
			while (++copy, (*list_it)->parent.plist && copy == plist->end()){
				*this = (*list_it)->parent;
				copy = list_it;
			};
			list_it = copy;

			return *this;
		}

		iterator operator++(int){
			iterator ret = *this;
			++*this;
			return ret;
		}


		bool operator==(const iterator& rit){ return plist == rit.plist && list_it == rit.list_it; }
		bool operator!=(const iterator& rit){ return plist != rit.plist || list_it != rit.list_it; }
		bool in_a_list(const iterator& rit){ return plist == rit.plist; }
	};

	template<typename T>
	struct __fibonacci_heap_node{

		typedef list<__fibonacci_heap_node*> node_list;
		typedef node_list* list_pointer;
		typedef __fibonacci_heap_iterator<T> iterator;

		T element;
		bool mark = false;
		node_list child;
		iterator parent = { typename node_list::iterator(), nullptr };

		__fibonacci_heap_node(const T& x):element(x){}
	};

	template<typename T, typename Key = T, typename ExtractKey = identity<Key>, typename Compare = ztl::less<Key>, template<typename> class Alloc = Allocator>
	class fibonacci_heap{
	public:
		typedef Key key_type;
		typedef T value_type;
		typedef __fibonacci_heap_node<value_type> node_type;
		typedef node_type* node_pointer;
		typedef size_t size_type;
		typedef list<node_pointer,Alloc> node_list;
		typedef node_list* list_pointer;
		typedef __fibonacci_heap_iterator<T> iterator;
		typedef const iterator const_iterator;
		typedef value_type& reference;
		typedef const value_type& const_reference;

	private:
		ExtractKey key_geter;
		Compare key_comparer;
		node_list tree_list;
		Alloc<node_type> _alloc_node;
		Alloc<node_list> _alloc_list;

		typename node_list::iterator min_it;
		size_type num_element = 0;

		static const float FIBO;
	private:


		node_pointer new_node(const value_type& x){
			node_pointer ret = _alloc_node.allocate(1);
			construct(ret, x);
			return ret;
		}

		void delete_node(const node_pointer p){
			_alloc_node.deallocate(p,1);
		}

		list_pointer new_list(){
			list_pointer ret = _alloc_list.allocate(1);
			construct(ret, node_list());
			return ret;
		}

		void delete_list(const list_pointer p){
			_alloc_list.deallocate(p,1);
		}

		bool compare(const value_type& x1, const value_type& x2) const {
			return key_comparer(key_geter(x1), key_geter(x2));
		}

		void link(node_pointer y, typename node_list::iterator it){
			node_pointer& x = *it;
			x->child.push_back(y);

			auto r = x->child.last();
			y->parent = iterator(it, &tree_list);
			for (auto& it_for : y->child)
				it_for->parent = iterator(r, &(x->child));
			y->mark = false;
		}

		void consolidate(){
			vector<node_pointer, Alloc> v(max_degree(), nullptr);
			for (auto it = tree_list.begin(); it != tree_list.end(); ++it){
				node_pointer& x = *it;
				size_type d = x->child.size();
				while (v[d] != nullptr){
					node_pointer& y = v[d];
					if (compare(y->element, x->element)){
						swap(x->element, y->element);
					}
					link(y, it);
					v[d] = nullptr;
					++d;
				}
				v[d] = x;
			}
			tree_list.clear();
			min_it = tree_list.end();
			for (size_type i = 0; i != v.size(); ++i)
			if (v[i] != nullptr){
				tree_list.push_back(v[i]);
				auto r = tree_list.last();
				for (auto child_it : (*r)->child)		//important update
					child_it->parent = iterator(r, &tree_list);
				if (min_it == tree_list.end() || compare(v[i]->element, (*min_it)->element))
					min_it = r;
			}
		}

		void cut(iterator& x, iterator& y){
			node_pointer pnode = (*x.list_it);
			(*y.list_it)->child.erase(x.list_it);
			tree_list.push_back(pnode);
			x = iterator(tree_list.last(), &tree_list);
			for (auto& it : pnode->child)
				it->parent = x;
			pnode->parent = iterator(typename node_list::iterator(), nullptr);
		}

		void cascading_cut(iterator y){
			iterator z = (*y.list_it)->parent;
			if (nullptr == z.plist) return;
			if ((*z.list_it)->mark == false)
				(*z.list_it)->mark = true;
			else{
				cut(y, z);
				cascading_cut(z);
			}

		}

		size_type max_degree(){
			return static_cast<size_type>(logf(static_cast<float>(num_element)) / logf(FIBO));
		}


	public:
		fibonacci_heap() :min_it(tree_list.end()){}

		iterator begin(){
			return iterator(tree_list.begin(),&tree_list);
		}

		const_iterator cbegin() const {
			return static_cast<const_iterator>(iterator(tree_list.begin(), &tree_list));
		}

		iterator end(){
			return iterator(tree_list.end(), &tree_list);
		}

		const_iterator cend() const {
			return static_cast<const_iterator>(iterator(tree_list.end(), &tree_list));
		}

		bool empty(){ return min_it == tree_list.end(); }

		void insert(const value_type& x){
			node_pointer pnode = new_node(x);
			auto r = tree_list.insert(min_it, pnode);
			if (min_it == tree_list.end() || compare(x, (*min_it)->element))
				min_it = r;
			++num_element;
		}

		void union_from(fibonacci_heap& x){
			min_it = tree_list.insert(tree_list.end(), x.tree_list.begin(), x.tree_list.end());
			if (min_it == tree_list.end() ||
				(x.min_it != x.tree_list.end() && compare((*min_it)->element, (*x.min_it)->element)))
				advance(min_it, distance(x.tree_list.begin(), x.min_it));
			num_element += x.num_element;
			x.tree_list.clear();
			x.num_element = 0;
			x.min_it = x.tree_list.end();
		}

		ztl::pair<const_reference,bool> get_min(){
			if (min_it!=tree_list.end())
				return make_pair<const_reference, bool>(static_cast<const_reference>((*min_it)->element), true);
			else
				return make_pair<const_reference, bool>(value_type(), false);
		}

		void delete_min(){
			if (min_it != tree_list.end()){
				node_list& list = (*min_it)->child;
				for (auto& it : list){
					it->parent.plist = nullptr;
					tree_list.push_back(it);
					auto update_it = tree_list.last();
					for (auto& child_it : it->child)
						child_it->parent = iterator(update_it, &tree_list);
				}
				//list.clear();
				delete_node(*min_it);
				tree_list.erase(min_it);
				min_it = tree_list.begin();
				consolidate();
				--num_element;
			}
		}

		ztl::pair<iterator,bool> decrease_key(iterator it,const value_type& new_value){
			if (it==end() || compare(*it, new_value)) return make_pair(it,false);
			*it = new_value;
			iterator y = (*it.list_it)->parent;
			
			if (y.plist && compare(*it, *y)){
				cut(it, y);
				cascading_cut(y);
			}
			if (compare(*it, (*min_it)->element))
				min_it = it.list_it;
			return make_pair(it, true);
		}

		iterator find(const key_type& key){
			iterator ret = begin();
			for (; ret != end(); ++ret)
			if (key_geter(*ret) == key)
				break;
			return ret;
		}

	};

	template<typename T, typename Key, typename ExtractKey, typename Compare, template<typename> class Alloc>
	const float fibonacci_heap<T, Key, ExtractKey, Compare, Alloc>::FIBO = 1.61803f;


}

#endif	//ZTL_FIBONACCI_HEAP_HPP
