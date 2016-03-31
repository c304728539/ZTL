#ifndef ZTL_RBtree_H
#define ZTL_RBtree_H


#include"ztl_tree_base.hpp"
#include"ztl_functional.hpp"

namespace ztl{
	enum node_color{COLOR_RED,COLOR_BLACK};

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	class RBtree;

	template<typename T>
	class __RBtree_node:public __tree_base_node<T,__RBtree_node<T>>{
	public:
		typedef typename __tree_base_node<T, __RBtree_node<T>>::value_type value_type;
		node_color _color;
	};

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	using  __RBtree_iterator = __tree_iterator_base<Key, Value, KeyOfValue, __RBtree_node, RBtree, Compare, Equal, Alloc>;

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	using  __RBtree_const_iterator = __tree_const_iterator_base<Key, Value, KeyOfValue, __RBtree_node, RBtree, Compare, Equal, Alloc>;

	template<typename Key, typename Value = Key, typename KeyOfValue = ztl::identity<Key>, typename Compare = ztl::less<Key>, typename Equal = equal<Key>, template<typename> class _Alloc = Allocator>
	class RBtree : public __tree_base<Key, Value, KeyOfValue, Compare, Equal, _Alloc, __RBtree_iterator, __RBtree_const_iterator, __RBtree_node>{
	public:
		typedef __tree_base<Key, Value, KeyOfValue, Compare, Equal, _Alloc, __RBtree_iterator, __RBtree_const_iterator, __RBtree_node> base;
		using value_type = Value;
		using base::const_pointer;
		using base::const_reference;
		using base::difference_type;
		using iterator = typename base::iterator;
		using const_iterator = typename base::const_iterator;
		using size_type = typename base::size_type;
		using node_pointer = typename base::node_pointer;
		using base::node_type;
	protected:
		using base::allocate_node;
		using base::deallocate_node;
		

	public:
		RBtree(KeyOfValue get = KeyOfValue(), Compare compare = Compare(), Equal equal = Equal()) :base(get, compare, equal){
			base::nil_node->_color = COLOR_BLACK;
		}

		template<typename InputIterator>
		RBtree(InputIterator first, InputIterator last):RBtree(){
			for (auto it = first; it != last; ++it)
				insert(*it);
		}

		RBtree(std::initializer_list<value_type> list) :RBtree(list.begin(),list.end()){}

		RBtree(const RBtree& rhs) :base(rhs){
			base::nil_node->_color = COLOR_BLACK;
		}

		RBtree& operator=(const RBtree& rhs){
			base::operator=(rhs);
			return *this;
		}

		RBtree(RBtree&& rit) :base(ztl::forward<RBtree>(rit)){}

		~RBtree(){
			clear();
		}

		using base::begin;
		using base::cbegin;
		using base::end;
		using base::cend;

		using base::empty;
		using base::clear;

		using base::find;
		using base::equal_range;

		iterator insert(const value_type& value);
		void erase(const_iterator it);
		
	private:
		using base::left_rotate;
		using base::right_rotate;

		void insert_aux(node_pointer pnode);
		void erase_aux(node_pointer pnode);

		void transplant(node_pointer u, node_pointer v){
			if (u->_parent == base::nil_node)
				base::root = v;
			else if (u == u->_parent->_left)
				u->_parent->_left = v;
			else
				u->_parent->_right = v;
			v->_parent = u->_parent;
		}

	};
	
	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	auto RBtree<Key, Value, KeyOfValue, Compare, Equal, Alloc>::insert(const value_type& value)
		->iterator{
		node_pointer new_node = allocate_node(value);
		const Key key = this->key_get(value);
		new_node->_left = this->nil_node;
		new_node->_right = this->nil_node;

		node_pointer parent_loop = base::nil_node;
		node_pointer root_cpy = base::root;
		while (root_cpy != this->nil_node){
			parent_loop = root_cpy;
			if (this->key_compare(key, this->key_get(root_cpy->_element)))
				root_cpy = root_cpy->_left;
			else
				root_cpy = root_cpy->_right;
		}
		new_node->_parent = parent_loop;
		if (parent_loop == this->nil_node)
			base::root = new_node;
		else if (this->key_compare(key, this->key_get(parent_loop->_element)))
			parent_loop->_left = new_node;
		else
			parent_loop->_right = new_node;

		new_node->_color = COLOR_RED;
		insert_aux(new_node);
		++base::_size;
		return iterator(new_node);
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	void RBtree<Key, Value, KeyOfValue, Compare, Equal, Alloc>::erase(const_iterator it){
		node_pointer z = it._node;
		if (z == base::nil_node) return;
		node_pointer to_deallocate = z;
		node_pointer x, y = z;
		node_color y_original_color = y->_color;

		if (z->_left == base::nil_node){
			x = z->_right;
			transplant(z, z->_right);
		}
		else if (z->_right == base::nil_node){
			x = z->_left;
			transplant(z, z->_left);
		}
		else{
			y = this->minimum(z->_right);
			y_original_color = y->_color;
			x = y->_right;
			if (y->_parent == z)
				x->_parent = y;
			else{
				transplant(y, y->_right);
				y->_right = z->_right;
				y->_right->_parent = y;
			}
			transplant(z, y);
			y->_left = z->_left;
			y->_left->_parent = y;
			y->_color = z->_color;
		}
		if (y_original_color == COLOR_BLACK)
			erase_aux(x);
		--base::_size;
		deallocate_node(to_deallocate);
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	void RBtree<Key, Value, KeyOfValue, Compare, Equal, Alloc>::insert_aux(node_pointer pnode){
		while (pnode->_parent->_color == COLOR_RED){
			if (pnode->_parent == pnode->_parent->_parent->_left){
				node_pointer tem_node = pnode->_parent->_parent->_right;
				if (tem_node->_color == COLOR_RED){
					pnode->_parent->_color = COLOR_BLACK;
					tem_node->_color = COLOR_BLACK;
					pnode->_parent->_parent->_color = COLOR_RED;
					pnode = pnode->_parent->_parent;
				}
				else{
					if (pnode == pnode->_parent->_right){
						pnode = pnode->_parent;
						left_rotate(pnode);
					}
					pnode->_parent->_color = COLOR_BLACK;
					pnode->_parent->_parent->_color = COLOR_RED;
					right_rotate(pnode->_parent->_parent);
				}
			}
			else{
				node_pointer tem_node = pnode->_parent->_parent->_left;
				if (tem_node->_color == COLOR_RED){
					pnode->_parent->_color = COLOR_BLACK;
					tem_node->_color = COLOR_BLACK;
					pnode->_parent->_parent->_color = COLOR_RED;
					pnode = pnode->_parent->_parent;
				}
				else{
					if (pnode == pnode->_parent->_left){
						pnode = pnode->_parent;
						right_rotate(pnode);
					}
					pnode->_parent->_color = COLOR_BLACK;
					pnode->_parent->_parent->_color = COLOR_RED;
					left_rotate(pnode->_parent->_parent);
				}
			}
		}
		base::root->_color = COLOR_BLACK;
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	void RBtree<Key, Value, KeyOfValue, Compare, Equal, Alloc>::erase_aux(node_pointer pnode){
		while (pnode != base::root && pnode->_color == COLOR_BLACK){
			if (pnode == pnode->_parent->_left){
				node_pointer tem_node = pnode->_parent->_right;
				if (tem_node->_color == COLOR_RED){
					tem_node->_color = COLOR_BLACK;
					pnode->_parent->_color = COLOR_RED;
					left_rotate(pnode->_parent);
					tem_node = pnode->_parent->_right;
				}
				if (tem_node->_left->_color == COLOR_BLACK&& tem_node->_right->_color == COLOR_BLACK){
					tem_node->_color = COLOR_RED;
					pnode = pnode->_parent;
				}
				else {
					if (tem_node->_right->_color == COLOR_BLACK){
						tem_node->_left->_color = COLOR_BLACK;
						tem_node->_color = COLOR_RED;
						right_rotate(tem_node);
						tem_node = pnode->_parent->_right;
					}
					tem_node->_color = pnode->_parent->_color;
					pnode->_parent->_color = COLOR_BLACK;
					tem_node->_right->_color = COLOR_BLACK;
					left_rotate(pnode->_parent);
					pnode = base::root;
				}
			}
			else{
				node_pointer tem_node = pnode->_parent->_left;
				if (tem_node->_color == COLOR_RED){
					pnode->_parent->_color = COLOR_RED;
					tem_node->_color = COLOR_BLACK;
					right_rotate(pnode->_parent);
					tem_node = pnode->_parent->_left;
				}
				if (tem_node->_right->_color == COLOR_BLACK&& tem_node->_left->_color == COLOR_BLACK){
					tem_node->_color = COLOR_RED;
					pnode = pnode->_parent;
				}
				else {
					if (tem_node->_left->_color == COLOR_BLACK){
						tem_node->_right->_color = COLOR_BLACK;
						tem_node->_color = COLOR_RED;
						left_rotate(tem_node);
						tem_node = pnode->_parent->_left;
					}
					tem_node->_color = pnode->_parent->_color;
					pnode->_parent->_color = COLOR_BLACK;
					tem_node->_left->_color = COLOR_BLACK;
					right_rotate(pnode->_parent);
					pnode = base::root;
				}
			}
		}
		pnode->_color = COLOR_BLACK;
	}
}

#endif
