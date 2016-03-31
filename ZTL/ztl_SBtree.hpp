#ifndef ZTL_SBtree_H

#define ZTL_SBtree_H

#include"ztl_tree_base.hpp"
#include"ztl_function.hpp"

namespace ztl{
	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal , template<typename> class Alloc>
	class SBtree;

	template<typename T>
	struct __SBtree_node : public __tree_base_node<T, __SBtree_node>{
		unsigned int size;
		__SBtree_node() :size(0){}
	};

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	using __SBtree_iterator = __tree_iterator_base < Key, Value, KeyOfValue, __SBtree_node, SBtree, Compare, Equal, Alloc > ;

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	using __SBtree_const_iterator = __tree_const_iterator_base < Key, Value, KeyOfValue, __SBtree_node, SBtree, Compare, Equal, Alloc > ;


	

	template<typename Key, typename Value = Key, typename KeyOfValue = ztl::identity<Value>, typename Compare = ztl::less<Key>, typename Equal = ztl::equal<Key>, template<typename> class _Alloc = Allocator>
	class SBtree :public  __tree_base < Key, Value, KeyOfValue, Compare, Equal, _Alloc, __SBtree_iterator, __SBtree_const_iterator, __SBtree_node > {
	private:
		

		void left_rotate(node_pointer& root);
		void right_rotate(node_pointer& root);
		void maintain(node_pointer& root, bool style);
		void insert_aux(node_pointer& root, const value_type& value, const node_pointer pParent);
		void erase_aux(node_pointer& root, const value_type& value);
		void clear(node_pointer root);
		void travel(node_pointer root);

	public:
		SBtree(){}
		~SBtree(){
			clear();
		};

		unsigned int _S(node_pointer p){
			if (p == nil_node) return 0;
			return p->size;
		}

		using __tree_base::begin;
		using __tree_base::cbegin;
		using __tree_base::end;
		using __tree_base::cend;
		using __tree_base::empty;
		using __tree_base::clear;
		using __tree_base::find;

		void insert(const value_type& key);    
		void erase(const_iterator it);     
	};

	template<typename Key, typename Value = Key, typename KeyOfValue, typename Compare, typename Equal , template<typename> class Alloc>
	void SBtree<Key, Value, KeyOfValue, Compare, Equal, Alloc>::erase(const_iterator it){
		erase_aux(root, *it);
	}

	template<typename Key, typename Value = Key, typename KeyOfValue, typename Compare, typename Equal , template<typename> class Alloc>
	void SBtree<Key, Value, KeyOfValue, Compare, Equal, Alloc>::erase_aux(node_pointer& root, const value_type& value){
		if (root == nil_node) return; 
		root->size--;
		auto key = key_get(value);
		auto key_root = key_get(root->_element);
		if (key_equal(key_root, key)){
			node_pointer tmp;
			if (root->_left != nil_node && root->_right == nil_node){
				tmp = root;  root = tmp->_left; deallocate_node(tmp);
			}
			else if (root->_left == nil_node && root->_right == nil_node){
				tmp = root; root = nil_node; deallocate_node(tmp);
			}
			else {
				tmp = minimum(root->_right);
				root->_element = tmp->_element; 
				erase_aux(root->_right, tmp->_element);
			}
		}
		else if (compare(value,root->_element)) erase_aux(root->_left, value);
		else erase_aux(root->_right, value);
	}

	template<typename Key, typename Value = Key, typename KeyOfValue, typename Compare, typename Equal , template<typename> class Alloc>
	void SBtree<Key, Value, KeyOfValue, Compare, Equal, Alloc>::insert_aux(node_pointer& root, const value_type& value, const node_pointer pParent){
		auto key = key_get(value);
		if (root == nil_node){
			root = allocate_node(value);
			root->size = 1;
			root->_parent = pParent;
			return;
		}
		root->size++;
		bool isLeft;
		if (isLeft = compare(value, root->_element))
			insert_aux(root->_left, key, root);
		else               
			insert_aux(root->_right, key, root);
		maintain(root, !isLeft);
	}

	template<typename Key, typename Value = Key, typename KeyOfValue, typename Compare, typename Equal , template<typename> class Alloc>
	void SBtree<Key, Value, KeyOfValue, Compare, Equal, Alloc>::insert(const value_type& value){
		insert_aux(root, value,nil_node);
	}

	template<typename Key, typename Value = Key, typename KeyOfValue, typename Compare, typename Equal , template<typename> class Alloc>
	void SBtree<Key,Value,KeyOfValue,Compare,Equal,Alloc>::left_rotate(node_pointer& x){
		__tree_base::left_rotate(x);
		auto temp = x->_left;
		x->size = temp->size;
		temp->size = _S(temp->_left) + _S(temp->_right) + 1;
	}

	template<typename Key, typename Value = Key, typename KeyOfValue, typename Compare, typename Equal , template<typename> class Alloc>
	void SBtree<Key,Value,KeyOfValue,Compare,Equal,Alloc>::right_rotate(node_pointer& x){
		__tree_base::right_rotate(x);
		auto temp = x->_right;
		x->size = temp->size;
		temp->size = _S(temp->_left) + _S(temp->_right) + 1;
	}

	template<typename Key, typename Value = Key, typename KeyOfValue, typename Compare, typename Equal , template<typename> class Alloc>
	void SBtree<Key,Value,KeyOfValue,Compare,Equal,Alloc>::maintain(node_pointer& root, bool style){
		if (root == nil_node) return;
		if (!style){
			if (root->_left->_left->size > root->_right->size)
				right_rotate(root);
			else if (root->_left->_right->size > root->_right->size){
				left_rotate(root->_left);
				right_rotate(root);
			}
			else return;
		}
		else {
			if (root->_right->_right->size > root->_left->size)
				left_rotate(root);
			else if (root->_right->_left->size > root->_left->size){
				right_rotate(root->_right);
				left_rotate(root);
			}
			else return;
		}
		maintain(root->_left, false);  maintain(root->_right, true);
		maintain(root, false);          maintain(root, true);
	}
}


#endif