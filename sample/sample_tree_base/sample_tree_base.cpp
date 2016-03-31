#include<iostream>
#include<functional>
#include"..\..\ZTl\ztl_RBtree.hpp"
#include"..\..\ZTl\ztl_vEBtree.hpp"
//#include"ztl_queue.h"

using namespace ztl;

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class _Alloc >
class tree;

template<typename T>
class tree_node :public __tree_base_node<T, tree_node<T>>{

};

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
using  tree_iterator = __tree_iterator_base<Key, Value, KeyOfValue, tree_node, tree, Compare, Equal, Alloc>;

template<typename Key, typename Value, typename KeyOfValue, typename Compare, typename Equal, template<typename> class Alloc>
using  tree_const_iterator = __tree_const_iterator_base<Key, Value, KeyOfValue, tree_node, tree, Compare, Equal, Alloc>;

template<typename Key, typename Value = Key, typename KeyOfValue = ztl::identity<Key>, typename Compare = ztl::less<Key>, typename Equal = equal<Key>, template<typename> class _Alloc = Allocator>
class tree : public __tree_base<Key, Value, KeyOfValue, Compare, Equal, _Alloc, tree_iterator, tree_const_iterator,tree_node>{
public:

	void createtreebystring(std::string str){
		createtreebystring_aux(str, root);
	}

private:

	void createtreebystring_aux(std::string& str, node_pointer& p){
		if (str.length()){
			if (*str.cbegin() == '#') { str.erase(str.cbegin()); p = nil_node; return; }
			p = allocate_node(*str.cbegin());
			str.erase(str.cbegin());
			createtreebystring_aux(str, p->_left);
			createtreebystring_aux(str, p->_right);
		}
	}

public:
	int leafnum(){ return (size() + 1) / 2; }
	int nodenumn(){ return size(); }
};


int main()
{
	using std::cout;
	using std::endl;
	tree<char> t;
	
	std::function<void(char)> f = [](char ch){cout << ch << ' '; };
	char buf[200];
	std::cin >> buf;
	t.createtreebystring(buf);
	cout << "Inorder:";
	t.traverseInOrder(f);
	cout << endl;
	cout << "Preorder:";
	t.traversePreOrder(f);
	cout << endl;
	cout << "Postorder:";
	t.traversePostOrder(f);
	cout << endl;

	cout << "leaf num:" << t.leafnum() << endl << "total num:" << t.nodenumn() << endl;



	//queue<char> q;
	//q.push(1);
	return 0;
}