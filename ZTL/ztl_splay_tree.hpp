
#ifndef ZTL_SPLAY_TREE
#define ZTL_SPLAY_TREE

#include"ztl_functional.hpp"
#include"ztl_tree_base.hpp"

namespace ztl{

	template<typename Key, typename Value ,
		typename KeyOfValue,
		typename Compare,
		typename Equal,
		template<typename> class Alloc>
	class splay_tree;

	
	template<typename T>
	class __splay_tree_node :public __tree_base_node<T, __splay_tree_node<T>>{};

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	using __splay_tree_iterator = __tree_iterator_base<Key, Value, KeyOfValue, __splay_tree_node, splay_tree, Compare, Equal, Alloc>;

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	using __splay_tree_const_iterator = __tree_const_iterator_base<Key, Value, KeyOfValue, __splay_tree_node, splay_tree, Compare, Equal, Alloc>;


	template<typename Key, typename Value = Key, 
		typename KeyOfValue = ztl::identity<Key>, 
		typename Compare = ztl::less<Key>, 
		typename Equal = equal<Key>,
		template<typename> class Alloc = Allocator>
	class splay_tree :public __tree_base<Key, Value, KeyOfValue, Compare, Equal, Alloc, __splay_tree_iterator, __splay_tree_const_iterator, __splay_tree_node> {
		
		using node_type = typename __tree_base::node_type;
		using node_pointer = node_type*;
		using iterator =  typename __tree_base::iterator;
		using const_iterator = typename __tree_base::const_iterator;

	private:
		
		void splay(node_pointer x) {
			auto& nil_node = this->nil_node;
			while (x->_parent != nil_node) {
				if (x->_parent->_parent == nil_node) {
					if (x->_parent->_left == x) right_rotate(x->_parent);
					else left_rotate(x->_parent);
				}
				else if (x->_parent->_left == x && x->_parent->_parent->_left == x->_parent) {
					right_rotate(x->_parent->_parent);
					right_rotate(x->_parent);
				}
				else if (x->_parent->_right == x && x->_parent->_parent->_right == x->_parent) {
					left_rotate(x->_parent->_parent);
					left_rotate(x->_parent);
				}
				else if (x->_parent->_left == x && x->_parent->_parent->_right == x->_parent) {
					right_rotate(x->_parent);
					left_rotate(x->_parent);
				}
				else {
					left_rotate(x->_parent);
					right_rotate(x->_parent);
				}
			}
		}

		void replace(node_pointer u, node_pointer v) {
			if (u->_parent == nil_node) root = v;
			else if (u == u->_parent->_left) u->_parent->_left = v;
			else u->_parent->_right = v;
			if (v != nil_node) v->_parent = u->_parent;
		}

		
	public:
		splay_tree(){ }

		using __tree_base::begin;
		using __tree_base::end;
		using __tree_base::find;
		using __tree_base::size;
		using __tree_base::clear;
		using __tree_base::empty;

		iterator insert(const value_type &x) {
			node_pointer z = root;
			node_pointer p = nil_node;
			while (z != nil_node) {
				p = z;
				if (compare(z->_element, x)) z = z->_right;
				else z = z->_left;
			}
			z = allocate_node(x);
			z->_parent = p;
			if (p == nil_node) root = z;
			else if (compare(p->_element, z->_element)) p->_right = z;
			else p->_left = z;
			splay(z);
			++_size;
			return iterator(root);
		}

		void erase(iterator& it) {
			//auto key = *it;
			node_pointer z = it._node;
			//node_pointer z = find(key)._node;
			if (z == nil_node) return;
			splay(z);
			if (z->_left == nil_node) replace(z, z->_right);
			else if (z->_right == nil_node) replace(z, z->_left);
			else {
				node_pointer y = minimum(z->_right);
				if (y->_parent != z) {
					replace(y, y->_right);
					y->_right = z->_right;
					y->_right->_parent = y;
				}
				replace(z, y);
				y->_left = z->_left;
				y->_left->_parent = y;
			}
			deallocate_node(z);
			--_size;
		}
	};
}
#endif // SPLAY_TREE
