#ifndef ZTL_HASHTABLE_H
#define ZTL_HASHTABLE_H

#include"ztl_allocator.hpp"
#include"ztl_vector.hpp"
#include"ztl_function.hpp"

namespace ztl{


	template<typename T, typename Key, typename HashFun, typename ExtractKey, typename Equal, template<typename> class Alloc>
	class hashtable;


	template<typename T>
	struct __hashtable_node{
		T element;
		__hashtable_node* next = nullptr;

		__hashtable_node(const T& x) :element(x){}
	};



	template<typename T, typename Key, typename HashFun, typename ExtractKey, typename Equal, template<typename> class Alloc>
	class __hashtable_iterator{
	public:
		typedef hashtable<T, Key, HashFun, ExtractKey, Equal, Alloc> hashtable;
		typedef __hashtable_iterator iterator;
		typedef forward_iterator_tag iterator_category;
		typedef __hashtable_node<T> node_type;
		typedef node_type* node_pointer;
		typedef T value_type;
		typedef value_type& reference;
		typedef const reference const_reference;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef size_t size_type;
	private:
		node_pointer cur = nullptr;
		hashtable* ht = nullptr;
	public:
		__hashtable_iterator(){}
		__hashtable_iterator(node_pointer pnode, hashtable* ht) :cur(pnode), ht(ht){}

		/*reference operator*() {
			return cur->element;		//only define the implementation by const
		}*/

		const_reference operator*() const {
			return cur->element;
		}

		/*pointer operator->(){
			return &operator*();	//ditto
		}*/

		const_pointer operator->() const {
			return &operator*();
		}

		iterator operator++(){
			node_pointer old = cur;
			cur = cur->next;
			if (!cur){
				size_type bucket = ht->bkt_num(old->element);
				while (!cur&&++bucket < ht->buckets.size())
					cur = ht->buckets[bucket];
			}
			return *this;
		}

		iterator operator++(int){
			auto ret = *this;
			++*this;
			return ret;
		}

		bool operator==(const iterator& rit) const { return cur == rit.cur; }
		bool operator!=(const iterator& rit) const { return cur != rit.cur; }
	};


	template<typename T, typename Key = T, typename HashFun = hashfun<T>, typename ExtractKey = identity<T>, typename Equal = equal<T>, template<typename> class Alloc = Allocator>
	class hashtable{
		friend class __hashtable_iterator<T, Key, HashFun, ExtractKey, Equal, Alloc>;
	public:
		typedef T value_type;
		typedef Key key_type;
		typedef __hashtable_node<value_type> node_type;
		typedef node_type* node_pointer;
		typedef const node_type* node_const_pointer;
		typedef size_t size_type;
		typedef __hashtable_iterator<T, Key, HashFun, ExtractKey, Equal, Alloc> iterator;
		typedef const iterator const_iterator;

		friend class iterator;

	private:
		HashFun hashfun;
		Alloc<node_type> _alloc;
		ExtractKey get_key;
		Equal is_equal;
		vector<node_pointer, Alloc> buckets;
		size_type num_elements;
	public:
		hashtable(size_type n = 100, HashFun hfn = HashFun(), Equal equ = Equal()) :hashfun(hfn), is_equal(equ), get_key(ExtractKey()){}

		~hashtable(){ clear(); }

	private:
		static const int __hashtable_num_prime = 28;
		static const size_type __hashtable_prime_list[__hashtable_num_prime];

		size_type _next_prime(size_type num){
			const size_type* it = __hashtable_prime_list;
			while (it != __hashtable_prime_list + __hashtable_num_prime && *it < num)
				++it;
			return *it;
		}

		size_type bkt_num(const value_type& value, size_type n){
			return hashfun(value)%n;
		}

		size_type bkt_num(const value_type& value){
			return hashfun(value) % buckets.size();
		}

		node_pointer new_node(const value_type& value){
			node_pointer ret = _alloc.allocate(1);
			construct(ret, value);
			return ret;
		}

		void delete_node(node_pointer pnode){
			destory(&pnode->element);
			_alloc.deallocate(pnode, 1);
		}

		void initialize_buckets(size_type n){
			const size_type size_to = _next_prime(n);
			buckets.reserve(size_to);
			buckets.insert(buckets.end(), size_to, nullptr);
			num_elements = 0;
		}
	public:

		const_iterator begin(){
			auto it = buckets.cbegin();
			while (!*it) ++it;
			return static_cast<const_iterator>(iterator(*it, this));
		}

		const_iterator end(){
			return static_cast<const_iterator>(iterator(nullptr, this));
		}

		ztl::pair<iterator, bool> insert_unique(const value_type& value){
			resize(num_elements + 1);
			return _insert_unique_aux(value);
		}

		ztl::pair<iterator,bool> _insert_unique_aux(const value_type& value){
			const size_type n = bkt_num(value);
			node_pointer first = buckets[n];

			for (node_pointer pnode = first; pnode; pnode = pnode->next)
			if (is_equal(get_key(pnode->element), get_key(value)))
				return make_pair(iterator(pnode,this), false);
			node_pointer pnew_node = new_node(value);
			pnew_node->next = first;
			buckets[n] = pnew_node;
			++num_elements;
			return make_pair(iterator(pnew_node,this), true);
		}

		iterator insert_equal(const value_type& value){
			resize(num_elements + 1);
			return _insert_equal_aux(value);
		}

		iterator _insert_equal_aux(const value_type& value){
			const size_type n = bkt_num(value);
			node_pointer first = buckets[n];
			node_pointer pnew_node = new_node(value);
			for (node_pointer pnode = first; pnode; pnode = pnode->next)
			if (is_equal(get_key(pnode->element), get_key(value))){
				
				pnew_node->next = pnode->next;
				pnode->next = pnew_node;
				goto ret;
			}

			pnew_node->next = first;
			buckets[n] = pnew_node;
		ret:
			++num_elements;
			return iterator(pnew_node, this);
		}

		void resize(size_type nsize){
			const size_type old_size = buckets.size();
			if (nsize > old_size){
				const size_type n = _next_prime(nsize);
				if (n > old_size){
					vector<node_pointer, Alloc> temp(n, nullptr);
					for (size_type bucket = 0; bucket != old_size; ++bucket){
						node_pointer pnode = buckets[bucket];
						while (pnode){
							size_type new_bucket = bkt_num(pnode->element, n);
							buckets[bucket] = pnode->next;
							pnode->next = temp[new_bucket];
							temp[new_bucket] = pnode;
							pnode = buckets[bucket];
						}
					}
					buckets.swap(temp);
				}
			}
		}

		size_type buckets_num() const {
			return buckets.size();
		}

		void clear(){
			for (size_t i = 0; i < buckets_num(); ++i){
				node_pointer pnode = buckets[i];
				if (pnode){
					node_pointer pnode_next = pnode->next;
					delete_node(pnode);
					pnode = pnode->next;
				}
				buckets[i] = nullptr;
			}
			
			num_elements = 0;
		}

		void copy_from(const hashtable& ht){
			if (ht == *this) return;
			buckets.clear();
			buckets.reserve(ht.buckets.size());
			buckets.insert(buckets.end(), ht.buckets.size(), nullptr);
			for (size_type i = 0; i != ht.buckets.size();++i)
			if (node_pointer cur = ht.buckets[i]){
				node_pointer copy = new_node(cur->element);
				buckets[i] = copy;
				for (node_pointer pnode = cur->next; pnode; cur = pnode, pnode = cur->next){
					copy->next = new_node(pnode->element);
					copy = copy->next;
				}
			}
			num_elements = ht.num_elements;
		}

		iterator find(const value_type& value_to_find){
			size_type n = bkt_num(value_to_find);
			node_pointer pnode = buckets[n];
			for (; pnode && !is_equal(get_key(pnode->element), get_key(value_to_find)); pnode = pnode->next);
			return iterator(pnode, this);
		}

		iterator find_key(const key_type& key_to_find){
			size_type n = key_to_find%buckets.size();
			node_pointer pnode = buckets[n];
			for (; pnode && !is_equal(get_key(pnode->element),key_to_find); pnode = pnode->next);
			return iterator(pnode, this);
		}

		size_type count(const key_type& key) const {
			const size_type n = key%buckets.size();
			size_type ret = 0;
			for (node_pointer pnode = buckets[n]; pnode; pnode = pnode->next)
			if (is_equal(get_key(pnode->element), key))
				++ret;
			return ret;
		}

		bool operator==(const hashtable& rit) const {
			return buckets.data() == rit.buckets.data();
		}
	};

	template<typename T, typename Key, typename HashFun, typename ExtractKey, typename Equal, template<typename> class Alloc>
	const typename hashtable<T, Key, HashFun, ExtractKey, Equal, Alloc>::size_type 
		hashtable<T, Key, HashFun, ExtractKey, Equal, Alloc>::__hashtable_prime_list[__hashtable_num_prime] = {
		53, 97, 193, 389, 769,
		1543, 3079, 6151, 12289, 24593,
		49157, 98317, 196613, 343241, 786433,
		1572869, 3145739, 6291469, 12582917, 25165843,
		50331653, 100663319, 201326611, 402653189, 805306457,
		1610612741, 3221226473ul, 4294967291ul
	};

}


#endif	//ZTL_HASHTABLE_H