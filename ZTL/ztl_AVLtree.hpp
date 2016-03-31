#ifndef ZTL_AVLTREE_HPP
#define ZTL_AVLTREE_HPP

#include"ztl_tree_base.hpp"
#include"ztl_functional.hpp"

namespace ztl{

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	class AVLtree;

	template<typename T>
	class __AVLtree_node :public __tree_base_node<T,__AVLtree_node>{
		using base = __tree_base_node<T, __AVLtree_node>;
		static __AVLtree_node* nil_node;
	public:
		using value_type = typename base::value_type;
		
		int _height = 0;

		bool HasOneChild() const { return (this->_left != nil_node) ^ (this->_right != nil_node); }
		bool HasChild() const { return this->_left != nil_node || this->_right != nil_node; }
		bool HasTwoChild() const { return this->_left != nil_node && this->_right != nil_node; }
		/* This Func only behaves correctly when HasOneChild() is true.*/
		__AVLtree_node* GetOnlyOneChild(){ return this->_left != nil_node ? this->_left : this->_right; }
	};

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	using __AVLtree_iterator = __tree_iterator_base<Key, Value, KeyOfValue, __AVLtree_node, AVLtree, Compare, Equal, Alloc>;

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
	using __AVLtree_const_iterator = __tree_const_iterator_base<Key, Value, KeyOfValue, __AVLtree_node, AVLtree, Compare, Equal, Alloc>;


	template<typename Key, typename Value = Key, typename KeyOfValue = identity<Value>, typename Compare = ztl::less<Key>, typename Equal = equal<Key>, template<typename> class _Alloc = Allocator>
	class AVLtree :public  __tree_base<Key, Value, KeyOfValue, Compare, Equal, _Alloc, __AVLtree_iterator, __AVLtree_const_iterator, __AVLtree_node>{
//		friend class __AVLtree_iterator<Key, Value, KeyOfValue, Compare, Equal, _Alloc>;
		friend class __AVLtree_node<Value>;
	private:
		using base = __tree_base<Key, Value, KeyOfValue, Compare, Equal, _Alloc, __AVLtree_iterator, __AVLtree_const_iterator, __AVLtree_node>;
		
	public:
		using value_type = typename base::value_type;
		using const_pointer = typename base::const_pointer;
		using const_reference = typename base::const_reference;
		using difference_type = typename base::difference_type;
		using iterator = typename base::iterator;
		using const_iterator = typename base::const_iterator;
		using size_type = typename base::size_type;
		using node_pointer = typename base::node_pointer;
		using node_type = typename base::node_type;

	private:
		void SetNilNode(){
			node_type::nil_node = this->nil_node;
		}
	public:
		AVLtree(){
			SetNilNode();
		}

		template<typename InputIterator>
		AVLtree(InputIterator first, InputIterator last):AVLtree(){
			for (auto it = first; it != last; ++it)
				insert(*it);
		}

		AVLtree(std::initializer_list<value_type> list) :AVLtree(list.begin(), list.end()){}

		explicit AVLtree(const AVLtree& rit) :AVLtree(rit.begin(), rit.end()){}
		
		explicit AVLtree(const AVLtree&& rit) :base(ztl::forward<AVLtree>(rit)){}

		~AVLtree(){
			clear();
		}

		using base::begin;
		using base::cbegin;
		using base::end;
		using base::cend;
		using base::empty;
		using base::clear;
		using base::find;
		
		void insert(const value_type& value);

		void erase(iterator it);
		
	private:
		using base::right_rotate;
		using base::left_rotate;
		
		void insert_aux(const value_type& value, node_pointer& pnode, node_pointer pParent);
		
		int GetHeight(node_pointer pNode){ return pNode != this->nil_node ? pNode->_height : -1; }

		void ResetHeight(node_pointer pNode)
		{
			pNode->_height = max(GetHeight(pNode->_left), GetHeight(pNode->_right)) + 1;
		}

		void left_rotate(node_pointer& x){
			left_rotate(x);
			ResetHeight(x->_left);
			ResetHeight(x);
		}

		void right_rotate(node_pointer& x){
			right_rotate(x);
			ResetHeight(x->_right);
			ResetHeight(x);
		}

		void double_right_rotate(node_pointer& pNode)
		{
			left_rotate(pNode->_left);
			right_rotate(pNode);
		}

		void double_left_rotate(node_pointer& pNode)
		{
			right_rotate(pNode->_right);
			left_rotate(pNode);
		}
	};

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, 
		template<typename> class Alloc>
	inline void AVLtree<Key, Value, KeyOfValue, Compare, Equal, Alloc>::insert(const value_type& value){
		insert_aux(value, this->root, this->nil_node);
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal,
		template<typename> class Alloc>
	void AVLtree<Key, Value, KeyOfValue, Compare, Equal, Alloc>::insert_aux(const value_type& element, node_pointer& pNode, node_pointer pParent){
		if (pNode == this->nil_node)
		{
			pNode = allocate_node(element);
			pNode->_parent = pParent;
		}
		else
		{
			if (compare(element, pNode->_element))
			{
				insert_aux(element, pNode->_left, pNode);
				if (GetHeight(pNode->_left) - GetHeight(pNode->_right) == 2)
				{
					if (compare(element, pNode->_left->_element)) right_rotate(pNode);
					else double_right_rotate(pNode);
				}
			}
			else
			{
				insert_aux(element, pNode->_right, pNode);
				if (GetHeight(pNode->_right) - GetHeight(pNode->_left) == 2)
				{
					if (compare(pNode->_right->_element,element)) left_rotate(pNode);
					else double_left_rotate(pNode);
				}
			}
		}
		ResetHeight(pNode);
	}

	template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal,
		template<typename> class Alloc>
	void AVLtree<Key, Value, KeyOfValue, Compare, Equal, Alloc>::erase(iterator it_erase){
		node_pointer& pNode = it_erase._node;
		if (pNode == this->nil_node) return;
		if (pNode->HasOneChild())
		{

			auto pTemp = pNode->GetOnlyOneChild();
			if (pNode->_parent->_left == pNode)
				pNode->_parent->_left = pTemp;
			else
				pNode->_parent->_right = pTemp;
			pTemp->_parent = pNode->_parent;
			if (this->root == pNode) 
				this->root = pTemp;
			deallocate_node(pNode);
			
			pNode = pTemp;
		}
		else if (!pNode->HasChild())
		{
			auto pTemp = pNode->_parent;
			if (pTemp->_left == pNode)
				pTemp->_left = this->nil_node;
			else
				pTemp->_right = this->nil_node;
			if (this->root == pNode) this->root = pTemp;
			deallocate_node(pNode);
			
			pNode = pTemp;
		}
		else
		{
			/*The Node going to be deleted has two children.*/
			++it_erase;
			pNode->_element = *it_erase;
			erase(it_erase);
			return;
		}

		do{
			ResetHeight(pNode);
			if (GetHeight(pNode->_right) - GetHeight(pNode->_left) == 2)
			{
				if (pNode->_right->_right) left_rotate(pNode);
				else double_left_rotate(pNode);
			}
			else if (GetHeight(pNode->_left) - GetHeight(pNode->_right) == 2)
			{
				if (pNode->_left->_left) right_rotate(pNode);
				else double_right_rotate(pNode);
			}
		} while ((pNode = pNode->_parent) != this->nil_node);

	}



}



#endif	//ZTL_AVLTREE_HPP
