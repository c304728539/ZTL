#ifndef ZTL_MAP_H
#define ZTL_MAP_H

#include"ztl_RBtree.hpp"

namespace ztl{

	template<typename Key, typename Value,
		typename Compare = less<Key>,
		typename Equal = equal<Key>,
		template<typename, typename, typename, typename, typename, template<typename> class> class Container = RBtree,
		template<typename> class Alloc = Allocator>
	class multimap{
	public:
		using key_type = Key;
		using data_type = Value;
		using value_type = ztl::pair<const Key, Value>;
	
	private:
		using tree_t = Container<key_type, value_type, select1st<value_type>, Compare, Equal, Alloc>;
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
		multimap() :t(){
			//do nothing
		}

		template<typename InputIterator>
		multimap(InputIterator first, InputIterator last) : t(first, last){}

		multimap(std::initializer_list<value_type> list) :t(list){}

		multimap(const multimap& rit) :t(rit.t){}

		multimap(multimap&& rit){
			t.swap(rit.t);
		}

		multimap& operator=(const multimap& rit){ t = rit.t; return *this; }

		~multimap(){ t.clear(); }


		size_type size() const { return t.size(); }
		void swap(multimap& x){ t.swap(x.t); }

		iterator begin(){
			return t.begin();
		}

		iterator end(){
			return t.end();
		}

		const_iterator cbegin() const {
			return t.cbegin();
		}

		const_iterator cend() const {
			return t.cend();
		}

		iterator insert(const value_type& value){
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

		iterator find(const key_type& key){
			return t.find(key);
		}

		pair<iterator, iterator> equal_range(const key_type& key){
			return t.equal_range(key);
		}

		pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
			return t.equal_range(key);
		}

		size_type count(const key_type& k) const {
			auto it = equal_range(k);
			return distance(it.fitst, it.second);
		}

		data_type& operator[](const key_type key){
			return t.insert(ztl::make_pair(key, data_type()))->second;
		}

		bool empty() const { return t.empty(); }
		void clear(){ t.clear(); }
		//iterator maximum(){ return t.maximum(); }
		//iterator minimum(){ return t.minimum(); }

	};

	template<typename Key, typename Value, 
		typename Compare = less<Key>,
		typename Equal = equal<Key>,
		template<typename, typename, typename, typename, typename, template<typename> class> class Container = RBtree,
		template<typename> class Alloc = Allocator>
	class map{

	public:
		using key_type = Key;
		using data_type = Value;
		using value_type = ztl::pair<const Key, Value>;

	private:
		using tree_t = Container<const key_type, value_type, select1st<value_type>, Compare, Equal, Alloc>;
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
		map() :t(){
			//do nothing
		}

		template<typename InputIterator>
		map(InputIterator first, InputIterator last) : t(first, last){}

		map(std::initializer_list<value_type> list) :t(list){}

		map(const map& rit) :t(rit.t){}

		map(map&& rit){
			t.swap(rit.t);
		}

		map& operator=(const map& rit){ t = rit.t; return *this; }

		~map(){ t.clear(); }

		size_type size() const { return t.size(); }
		void swap(map& x){ t.swap(x.t); }

		iterator begin(){
			return t.begin();
		}

		iterator end(){
			return t.end();
		}

		const_iterator cbegin() const {
			return t.cbegin();
		}

		const_iterator cend() const {
			return t.cend();
		}

		ztl::pair<iterator, bool> insert(const value_type& value){
			auto it_find = t.find(value.first);
			if (it_find == t.end())
				return pair<iterator, bool>(t.insert(value), true);
			else
				return pair<iterator, bool>(it_find, false);
		}

		void erase(iterator it){
			t.erase(it);
		}

		bool erase(const key_type& key){
			auto it = t.find(key);
			if (it == t.end())
				return false;
			else{
				t.erase(it);
				return true;
			}
		}

		iterator find(const key_type& value){
			return t.find(value);
		}

		const_iterator find(const key_type& value) const {
			return t.find(value);
		}

		data_type& operator[](const key_type& key){
			return (*(insert(ztl::make_pair(key, data_type())).first)).second;
		}

		bool empty() const { return t.empty(); }
		void clear(){ t.clear(); }

		//iterator maximum(){ return t.maximum(); }
		//iterator minimum(){ return t.minimum(); }
	};

}

#endif
