#ifndef ZTL_TREE_BASE_HPP
#define ZTL_TREE_BASE_HPP

#include"ztl_construct.hpp"
#include"ztl_allocator.hpp"
#include"ztl_iterator.hpp"

namespace ztl{
	
	template<typename T, typename _node_type>
	struct __tree_base_node{
		using value_type = T;
		using node_type = _node_type;
		using node_pointer = node_type*;
		value_type _element;
		node_pointer _parent, _left, _right;

		__tree_base_node(){}
		__tree_base_node(const value_type& x):_element(x){}

	};

	template<typename Key, typename Value, typename KeyOfValue, template<typename> class _node_type,
		template<typename, typename, typename, typename, typename, template<typename> class> class _tree_iterator_for,
		typename Compare, typename Equal,
		template<typename> class Alloc>
	class __tree_const_iterator_base;

	template<typename Key, typename Value, typename KeyOfValue, template<typename> class _node_type,
		template<typename, typename, typename, typename, typename, template<typename> class> class _tree_iterator_for,
		typename Compare, typename Equal,
		template<typename> class Alloc = Allocator>
	class __tree_iterator_base:
		public bidirectional_iterator_base<Value, 
		__tree_iterator_base<Key,Value,KeyOfValue,_node_type,_tree_iterator_for,Compare,Equal,Alloc>,
		false>
	{
		using base = bidirectional_iterator_base<Value,
			__tree_iterator_base<Key, Value, KeyOfValue, _node_type, _tree_iterator_for, Compare, Equal, Alloc>,
			false>;

		friend class _tree_iterator_for<Key, Value, KeyOfValue, Compare, Equal, Alloc>;
		using my_tree_iterator_for = _tree_iterator_for<Key, Value, KeyOfValue, Compare, Equal, Alloc>;
		//friend class my_tree_iterator_for;

		friend class __tree_const_iterator_base < Key, Value, KeyOfValue, _node_type, _tree_iterator_for, Compare, Equal, Alloc > ;

	public:
		using value_type = typename base::value_type;
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using node_type =_node_type<value_type>;
		using node_pointer = node_type*;
		using self = typename base::self;
		using iterator_category = typename base::iterator_category;
	private:
		node_pointer _node;
	public:
		__tree_iterator_base() = delete;

		explicit __tree_iterator_base(const node_pointer& pnode) :_node(pnode){}

		virtual reference operator*() const override {
			return _node->_element;
		}

		virtual pointer operator->() const override {
			return &_node->_element;	
		}

		virtual self& operator++() override {
			if (_node->_right != my_tree_iterator_for::nil_node){
				_node = _node->_right;
				while (_node->_left != my_tree_iterator_for::nil_node)
					_node = _node->_left;
			}
			else{
				node_pointer y = _node->_parent;
				while (y != my_tree_iterator_for::nil_node && _node == y->_right){
					_node = y;
					y = y->_parent;
				}
				_node = y;
			}
			return *this;
		}

		virtual self& operator--() override {
			if (_node->_left != my_tree_iterator_for::nil_node){
				_node = _node->_left;
				while (_node->_right != my_tree_iterator_for::nil_node)
					_node = _node->_right;
			}
			else{
				node_pointer y = _node->_parent;
				while (y != my_tree_iterator_for::nil_node && _node == y->_left){
					_node = y;
					y = y->_parent;
				}
				_node = y;
			}
			return *this;
		}

		virtual bool operator==(const self& rit) const override {
			return _node == rit._node;
		}

	};

	template<typename Key, typename Value, typename KeyOfValue, template<typename> class _node_type,
		template<typename, typename, typename, typename, typename, template<typename> class> class _tree_iterator_for,
		typename Compare, typename Equal,
		template<typename> class Alloc = Allocator>
	class __tree_const_iterator_base:
		public bidirectional_iterator_base<Value,
		__tree_const_iterator_base<Key, Value, KeyOfValue, _node_type, _tree_iterator_for, Compare, Equal, Alloc>,
		true> 
	{
		using base = bidirectional_iterator_base<Value,
			__tree_const_iterator_base<Key, Value, KeyOfValue, _node_type, _tree_iterator_for, Compare, Equal, Alloc>,
			true>;
		friend class _tree_iterator_for<Key, Value, KeyOfValue, Compare, Equal, Alloc>;	
		using my_tree_iterator_for = _tree_iterator_for<Key, Value, KeyOfValue, Compare, Equal, Alloc>;
		//friend class my_tree_iterator_for;

	public:
		using value_type = typename base::value_type;
		using const_reference = typename base::reference;
		using const_pointer = typename base::pointer;
		using self = typename base::self;
		using node_type = _node_type<value_type>;
		using node_pointer = node_type*;
		using _tree_iterator_base = __tree_iterator_base < Key, Value, KeyOfValue, _node_type, _tree_iterator_for, Compare, Equal, Alloc >;
		using iterator_category = typename base::iterator_category;
		
	private:
		node_pointer _node;
	public:
		__tree_const_iterator_base() = delete;
		__tree_const_iterator_base(const _tree_iterator_base& arg) :_node(arg._node){}
		explicit __tree_const_iterator_base(const node_pointer& pnode) :_node(pnode){}

		virtual const_reference operator*() const override {
			return static_cast<const_reference>(_node->_element);
		}

		virtual const_pointer operator->() const override {
			return static_cast<const_pointer>(&_node->_element);
		}

		virtual self& operator++() override {
			if (_node->_right != my_tree_iterator_for::nil_node){
				_node = _node->_right;
				while (_node->_left != my_tree_iterator_for::nil_node)
					_node = _node->_left;
			}
			else{
				node_pointer y = _node->_parent;
				while (y != my_tree_iterator_for::nil_node && _node == y->_right){
					_node = y;
					y = y->_parent;
				}
				_node = y;
			}
			return *this;
		}

		virtual self& operator--() override {
			if (_node->_left != my_tree_iterator_for::nil_node){
				_node = _node->_left;
				while (_node->_right != my_tree_iterator_for::nil_node)
					_node = _node->_right;
			}
			else{
				node_pointer y = _node->_parent;
				while (y != my_tree_iterator_for::nil_node && _node == y->_left){
					_node = y;
					y = y->_parent;
				}
				_node = y;
			}
			return *this;
		}

		virtual bool operator==(const self& rit) const override {
			return _node == rit._node;
		}
	};

	template<typename Key, typename Value, typename KeyOfValue,
		typename Compare, typename Equal,
		template<typename> class Alloc,
		template<typename, typename, typename, typename, typename, template<typename> class> class _iterator_type,
		template<typename, typename, typename, typename, typename, template<typename> class> class _const_iterator_type,
		template<typename> class _node_type>
	class __tree_base{
	
	public:
		using key_type = Key;
		using value_type = Value;
		
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const reference;
		
		using iterator = _iterator_type<Key, value_type, KeyOfValue, Compare, Equal, Alloc>;
		using const_iterator = _const_iterator_type<Key, value_type, KeyOfValue, Compare, Equal, Alloc>;
		using const_reverse_iterator = _reverse_iterator<const_iterator>;
		using reverse_iterator = _reverse_iterator<iterator>;
		
		using node_type = _node_type<value_type>;
		using node_pointer = node_type*;
		
		using size_type = size_t;
		using difference_type = ptrdiff_t;

	public:
		__tree_base(KeyOfValue get = KeyOfValue(), Compare compare = Compare(), Equal equal = Equal()) :
			_size(0), key_get(get), key_compare(compare), key_equal(equal){
			if (!nil_node){
				nil_node = allocate_node();
				nil_node->_left = nil_node->_right = nil_node->_parent = nil_node;
			}
			root = nil_node;
		}

		__tree_base(const __tree_base& rhs):_size(rhs._size){
			copy_tree(root,rhs.root,nil_node);
		}

		__tree_base(__tree_base&& rhs) :_size(rhs._size),root(rhs.root){
			rhs._size = 0;
			rhs.root = nil_node;
		}

		__tree_base& operator=(const __tree_base& rhs){
			clear();
			_size = rhs._size;
			copy_tree(root, rhs.root, nil_node);
			return *this;
		}

		~__tree_base(){
			clear();
		}

		Alloc<node_type> _alloc;

	private:
		void copy_tree(node_pointer& pnode_my, const node_pointer pnode,const node_pointer parent){
			if (pnode != nil_node){
				pnode_my = allocate_node(pnode->_element);
				copy_tree(pnode_my->_left, pnode->_left, pnode_my);

				copy_tree(pnode_my->_right, pnode->_right, pnode_my);
			}
			else
				pnode_my = nil_node;
		}

	protected:
		node_pointer root;
		Compare key_compare;
		Equal key_equal;
		KeyOfValue key_get;
		size_type _size;
	public:
		static node_pointer nil_node;	//null_node
		
	protected:
		node_pointer allocate_node(const value_type& x=value_type()){
			node_pointer ret = _alloc.allocate(1);
			construct(&ret->_element, x);
			ret->_left = ret->_right = ret->_parent = nil_node;
			return ret;
		}

		void deallocate_node(node_pointer to_deallocate){ _alloc.deallocate(to_deallocate, 1); }
		
		bool compare(const value_type& x1, const value_type& x2) const {
			return key_compare(key_get(x1), key_get(x2));
		}

		bool equal(const value_type& x1, const value_type& x2) const {
			return key_equal(key_get(x1), key_get(x2));
		}

		static node_pointer minimum(node_pointer x){
			while (x->_left != nil_node)
				x = x->_left;
			return x;
		}

		static node_pointer maximum(node_pointer x){
			while (x->_right != nil_node)
				x = x->_right;
			return x;
		}

		void left_rotate(node_pointer x);
		void right_rotate(node_pointer x);

		template<typename F>
		void traverseInOrder_aux(node_pointer p, F f){
			if (p != nil_node){
				traverseInOrder_aux(p->_left, f);
				f(p->_element);
				traverseInOrder_aux(p->_right, f);
			}
		}

		template<typename F>
		void traversePreOrder_aux(node_pointer p, F f){
			if (p != nil_node){
				f(p->_element);
				traversePreOrder_aux(p->_left, f);
				traversePreOrder_aux(p->_right, f);
			}
		}

		template<typename F>
		void traversePostOrder_aux(node_pointer p, F f){
			if (p != nil_node){
				traversePostOrder_aux(p->_left, f);
				f(p->_element);
				traversePostOrder_aux(p->_right, f);
			}
		} 

	public:
		iterator begin(){
			return static_cast<iterator>(minimum(root));
		}

		const_iterator cbegin() const {
			return static_cast<const_iterator>(minimum(root));
		}

		reverse_iterator rbegin() {
			return static_cast<reverse_iterator>(maximum(root));
		}

		const_reverse_iterator crbegin() {
			return static_cast<const_reverse_iterator>(maximum(root));
		}

		iterator end(){
			return static_cast<iterator>(nil_node);
		}

		const_iterator cend() const {
			return static_cast<const_iterator>(nil_node);
		}

		reverse_iterator rend() {
			return static_cast<reverse_iterator>(nil_node);
		}

		const_reverse_iterator crend() const {
			return static_cast<const_reverse_iterator>(nil_node);
		}

		bool empty() const { return _size == 0; }

		void clear(){
			erase_for_clear(root);
			root = nil_node;
		}

		template<typename F>
		inline void traversePreOrder(F f){
			traversePreOrder_aux(root, f);
		}		

		template<typename F>
		inline void traverseInOrder(F f){
			traverseInOrder_aux(root, f);
		}

		template<typename F>
		inline void traversePostOrder(F f){
			traversePostOrder_aux(root, f);
		}

		size_type size() const { return _size; }

		void swap(__tree_base& x){
			ztl::swap(root, x.root);
			ztl::swap(_size, x._size);
		}

		

		const_iterator find(const key_type& key_to_find) const;
		iterator find(const key_type& key_to_find);

		pair<iterator, iterator> equal_range(const key_type& key){
			node_pointer node_find = root;
			if (!empty())
			while (node_find != nil_node &&
				!key_equal(key, key_get(node_find->_element))){

				if (key_compare(key, key_get(node_find->_element)))
					node_find = node_find->_left;
				else
					node_find = node_find->_right;
			}
			iterator first(node_find), last(node_find);
			while (key_get(*last++) == key);
			return make_pair(first, last);
		}

		iterator minimum(){
			return iterator(minimum(root));
		}

		iterator maximum(){
			return iterator(maximum(root));
		}


	private:
		void erase_for_clear(node_pointer pnode_erase){
			if (pnode_erase == nil_node) return;
			erase_for_clear(pnode_erase->_left);
			erase_for_clear(pnode_erase->_right);
			deallocate_node(pnode_erase);
		}
	};
	template<typename Key, typename Value, typename KeyOfValue,
		typename Compare, typename Equal,
		template<typename> class Alloc,
		template<typename, typename, typename, typename, typename, template<typename> class> class _iterator_type,
		template<typename, typename, typename, typename, typename, template<typename> class> class _const_iterator_type,
		template<typename> class _node_type>
	typename __tree_base<Key, Value, KeyOfValue, Compare, Equal, Alloc, _iterator_type, _const_iterator_type, _node_type>::node_pointer __tree_base<Key, Value, KeyOfValue, Compare, Equal, Alloc, _iterator_type, _const_iterator_type, _node_type>::nil_node = nullptr;


	template<typename Key, typename Value, typename KeyOfValue,
		typename Compare, typename Equal,
		template<typename> class Alloc,
		template<typename, typename, typename, typename, typename, template<typename> class> class _iterator_type,
		template<typename, typename, typename, typename, typename, template<typename> class> class _const_iterator_type,
		template<typename> class _node_type>
	void __tree_base<Key, Value, KeyOfValue, Compare, Equal, Alloc, _iterator_type, _const_iterator_type, _node_type>::left_rotate(node_pointer x){
		if (x->_right == nil_node) return;
		node_pointer y = x->_right;
		x->_right = y->_left;
		x->_right->_parent = x;
		if ((y->_parent = x->_parent) == nil_node)
			root = y;
		else if (x == x->_parent->_left)
			x->_parent->_left = y;
		else
			x->_parent->_right = y;
		y->_left = x;
		x->_parent = y;
	}

	template<typename Key, typename Value, typename KeyOfValue,
		typename Compare, typename Equal,
		template<typename> class Alloc,
		template<typename, typename, typename, typename, typename, template<typename> class> class _iterator_type,
		template<typename, typename, typename, typename, typename, template<typename> class> class _const_iterator_type,
		template<typename> class _node_type>
	void __tree_base<Key, Value, KeyOfValue, Compare, Equal, Alloc, _iterator_type, _const_iterator_type, _node_type>::right_rotate(node_pointer x){
		if (x->_left == nil_node) return;
		node_pointer y = x->_left;
		x->_left = y->_right;
		x->_left->_parent = x;
		if ((y->_parent = x->_parent) == nil_node)
			root = y;
		else if (x == x->_parent->_left)
			x->_parent->_left = y;
		else
			x->_parent->_right = y;
		y->_right = x;
		x->_parent = y;
	}

	template<typename Key, typename Value, typename KeyOfValue,
		typename Compare, typename Equal,
		template<typename> class Alloc,
		template<typename, typename, typename, typename, typename, template<typename> class> class _iterator_type,
		template<typename, typename, typename, typename, typename, template<typename> class> class _const_iterator_type,
		template<typename> class _node_type>
	auto __tree_base<Key, Value, KeyOfValue, Compare, Equal, Alloc, _iterator_type, _const_iterator_type, _node_type>::find(const key_type& key_to_find) const
		->const_iterator{
		node_pointer node_find = root;
		if (!empty())
			while (node_find != nil_node && 
				!key_equal(key_to_find, key_get(node_find->_element)))
				if (key_compare(key_to_find, key_get(node_find->_element)))
					node_find = node_find->_left;
				else
					node_find = node_find->_right;
		return const_iterator(node_find);
	}
	
	template<typename Key, typename Value, typename KeyOfValue,
		typename Compare, typename Equal,
		template<typename> class Alloc,
		template<typename, typename, typename, typename, typename, template<typename> class> class _iterator_type,
		template<typename, typename, typename, typename, typename, template<typename> class> class _const_iterator_type,
		template<typename> class _node_type>
	auto __tree_base<Key, Value, KeyOfValue, Compare, Equal, Alloc, _iterator_type, _const_iterator_type, _node_type>::find(const key_type& key_to_find)
		->iterator {
		node_pointer node_find = root;
		if (!empty())
			while (node_find != nil_node &&
				!key_equal(key_to_find, key_get(node_find->_element)))
				if (key_compare(key_to_find, key_get(node_find->_element)))
					node_find = node_find->_left;
				else
					node_find = node_find->_right;
		return iterator(node_find);
	}

}

#endif	//ZTL_TREE_BASE_HPP
