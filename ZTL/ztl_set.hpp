#ifndef ZTL_SET_H
#define ZTL_SET_H

#include"ztl_RBtree.hpp"


namespace ztl{

	template<typename T, 
		typename Compare = ztl::less<T>,
		typename Equal = equal<T>, 
		template<typename, typename, typename, typename, typename, template<typename> class> class Container = RBtree,
		template<typename> class Alloc = Allocator>
	class multiset{
	public:
		using key_type = T;
		using value_type = T;
		using key_compare = Compare;
		using value_compare = key_compare;

	private:
		using tree_t = Container<T, T, identity<T>, Compare, Equal, Alloc>;
		tree_t t;

	public:
		using pointer = typename tree_t::pointer;
		using const_pointer = typename tree_t::const_pointer;
		using reference = typename tree_t::reference;
		using const_reference = typename tree_t::const_reference;
		using iterator = typename tree_t::iterator;
		using const_iterator = typename tree_t::const_iterator;

		using size_type = typename tree_t::size_type;
		using difference_type = typename tree_t::difference_type;

	public:
		multiset() :t(){
			//do nothing
		}

		template<typename InputIterator>
		multiset(InputIterator first, InputIterator last) : t(first, last){}

		multiset(std::initializer_list<value_type> list) :t(list){}

		multiset(const multiset& rit) :t(rit.t){}

		multiset& operator=(const multiset& rit){ t = rit.t; return *this; }

		size_type size()const{ return t.size(); }
		
		iterator begin(){
			return t.begin();
		}

		iterator end(){
			return t.end();
		}

		const_iterator cbegin() const {
			return t.cbegin();
		}

		const_iterator end() const {
			return t.cend();
		}

		iterator insert(value_type& value){
			return t.insert(value);
		}

		void erase(iterator it){
			t.erase(it);
		}

		bool erase(const value_type& value){
			auto it = t.find(value);
			if (it == t.end())
				return false;
			else{
				t.erase(it);
				return true;
			}
		}

		size_type count(const key_type& key){
			auto it_pair = t.equal_tange(key);
			return distance(it_pair.first, it_pair.second);
		}

		iterator find(const value_type& value){
			return t.find(value);
		}

		const_iterator find(const key_type& value) const {
			return t.find(value);
		}

		bool empty(){ return t.empty(); }

	};

	template<typename T, 
		typename Compare = ztl::less<T>, 
		typename Equal = equal<T>,
		template<typename, typename, typename, typename, typename, template<typename> class> class Container = RBtree,
		template<typename> class Alloc = Allocator>
	class set{
	public:
		using key_type = T;
		using value_type = T;
		using key_compare = Compare;
		using value_compare = key_compare;
	private:
		using tree_t = Container<T, T, identity<T>, Compare, Equal, Alloc>;
		tree_t t;

	public:
		using pointer = typename tree_t::pointer;
		using const_pointer = typename tree_t::const_pointer;
		using reference = typename tree_t::reference;
		using const_reference = typename tree_t::const_reference;
		using iterator = typename tree_t::iterator;
		using const_iterator = typename tree_t::const_iterator;

		using size_type = typename tree_t::size_type;
		using difference_type = typename tree_t::difference_type;

	public:
		set() :t(){
			//do nothing
		}

		template<typename InputIterator>
		set(InputIterator first, InputIterator last) : t(first, last){}

		set(std::initializer_list<value_type> list) :t(list){}

		set(const set& rit) :t(rit.t){}

		set& operator=(const set& rit){ t = rit.t; return *this; }

		size_type size()const{ return t.size(); }
		
		iterator begin(){
			return t.begin();
		}

		iterator end(){
			return t.end();
		}

		const_iterator cbegin() const {
			return t.cbegin();
		}

		const_iterator end() const {
			return t.cend();
		}

		ztl::pair<iterator, bool> insert(const value_type& value){
			auto it_find = t.find(value);
			if (it_find == t.end())
				return pair<iterator, bool>(t.insert(value), true);
			else
				return pair<iterator, bool>(it_find, false);
		}

		void erase(iterator it){
			t.erase(it);
		}

		bool erase(const value_type& value){
			auto it = t.find(value);
			if (it == t.end())
				return false;
			else{
				t.erase(it);
				return true;
			}
		}

		iterator find(const value_type& value){
			return (t.find(value));
		}

		const_iterator find(const key_type& value) const {
			return t.find(value);
		}

		bool empty(){ return t.empty(); }
	};
}

#endif
