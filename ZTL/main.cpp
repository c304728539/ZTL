

#include<iostream>
#include"ztl_splay_tree.hpp"
using std::cout;
using std::endl;
using namespace ztl;

int main(int argc, char *argv[], char *env[])
{

	splay_tree<int> st;
	st.insert(1);
	cout << *st.begin();
	st.erase(st.begin());
	if (st.empty())
		cout << "!!!";
	//cout << a%(b) << endl;
	
	return 0;
}