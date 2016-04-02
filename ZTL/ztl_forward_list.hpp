#ifndef ZTL_FORWARD_LIST
#define ZTL_FORWARD_LIST


#include"ztl_allocator.hpp"
#include"ztl_iterator.hpp"
#include"ztl_construct.hpp"

namespace ztl{

	template<typename T>
	struct __forward_list_node
	{
		__forward_list_node* _next = nullptr;
		T element;

		__forward_list_node(const T& x) :element(x){}
	};

	template<typename T, typename iterator, bool is_const>
	class __forward_list_iterator_base :public forward_iterator_base<T, iterator, is_const> {
		using base = forward_iterator_base<T, iterator, false >;
	public:
		using value_type = typename base::value_type;
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using self = typename base::self;
		using size_type = typename base::size_type;

		using difference_type = typename base::difference_type;
		using iterator_category = typename base::iterator_category;

		using node_type = __forward_list_node<value_type>;
		using link_type = node_type*;
	public:
		__forward_list_iterator_base(const __forward_list_iterator_base&) = default;
		__forward_list_iterator_base(const link_type arg = nullptr) :_node(arg) {}

		virtual bool operator==(const self& it) const override {
			return _node == it._node;
		}

		virtual self& operator++() override {
			if (_node)
				_node = _node->_next;
			return base::rself();
		}

		using base::operator++;
	protected:
		link_type _node;
	};

	template<typename T>
	class __forward_list_const_iterator;
	
	template<typename T>
	class __forward_list_iterator:public __forward_list_iterator_base<T, __forward_list_iterator<T>,false>{
		using base = __forward_list_iterator_base<T,__forward_list_iterator<T>, false > ;

		friend class __forward_list_const_iterator<T>;

		template<typename _T, template<typename> class Alloc>
		friend class forward_list;
	public:
		
		using value_type = typename base::value_type;
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using self = typename base::self;
		using size_type = typename base::size_type;
		
		using difference_type = typename base::difference_type;
		using iterator_category = typename base::iterator_category;

		using node_type = __forward_list_node<value_type>;
		using link_type = node_type*;

	public:
		using base::base;
	
		virtual reference operator*() const override {
			return base::_node->element;
		}

		virtual pointer operator->() const override {
			return &base::_node->element;
		}

	};

	template<typename T>
	class __forward_list_const_iterator :public __forward_list_iterator_base<T, __forward_list_iterator<T>, true> {
		using base = forward_iterator_base<T, __forward_list_const_iterator<T>, true > ;

		template<typename _T, template<typename> class Alloc>
		friend class forward_list;
	public:

		using value_type = typename base::value_type;
		using const_reference = typename base::reference;
		using const_pointer = typename base::pointer;
		using self = typename base::self;
		using size_type = typename base::size_type;

		using difference_type = typename base::difference_type;
		using iterator_category = typename base::iterator_category;

		using node_type = __forward_list_node<value_type>;
		using link_type = node_type*;

	private:
		link_type _node;

	public:
		using base::base;
		__forward_list_const_iterator(const __forward_list_iterator<T>& arg) :_node(arg._node){}

		virtual const_reference operator*() const override {
			return static_cast<const_reference>(base::_node->element);
		}

		virtual const_pointer operator->() const override {
			return static_cast<const_pointer>(&base::_node->element);
		}
	};

	template<typename T, template<typename> class Alloc = Allocator>
	class forward_list
	{
	public:
	
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;

		using iterator = __forward_list_iterator<T>;
		using const_iterator = iterator;

		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using list_node = __forward_list_node<T>;
		using link_type = list_node*;
		
	private:
		Alloc<list_node> _alloc;

	protected:
		link_type _node;
		size_type _size;

	private:
		link_type allocate_node(){
			link_type ret = _alloc.allocate(1);
			ret->_next = nullptr;
			return ret;
		}

		link_type allocate_node(const value_type& x) {
			link_type ret = _alloc.allocate(1);
			construct(&ret->element, x);
			ret->_next = nullptr;
			return ret;
		}

		void deallocate_node(link_type& p){ _alloc.deallocate(p, 1); p = nullptr; }
	public:
		forward_list() :_size(0), _node(allocate_node()){
		}


		forward_list(const forward_list& _list) :forward_list() {
			link_type node_last = _node;
			for (iterator it = _list.cbegin(); it != _list.cend(); ++it){
				node_last->_next = allocate_node(*it);
				node_last = node_last->_next;
			}
		}

		forward_list(forward_list&& _right) :_node(_right._node), _size(_right._size){
			_right._node = nullptr;
			_right._size = 0;
		}

		template<typename InputIterator>
		forward_list(InputIterator begin,InputIterator end) : forward_list() {
			auto it_list = begin;
			auto it_insert = insert_after(before_begin(), *(it_list++));
			for (; it_list != end; ++it_list)
				it_insert = insert_after(it_insert, *it_list);
		}


		forward_list(std::initializer_list<value_type> list) :forward_list(list.begin(),list.end()) {
			
		}

		forward_list& operator=(const forward_list& rhs){
			clear();
			link_type node_last = _node;
			for (iterator it = rhs.cbegin(); it != rhs.cend();++it){
				node_last->_next = allocate_node();
				construct(&node_last->_next->element, *it);
				node_last = node_last->_next;
			}
			return *this;
		}

		forward_list& operator=(forward_list&& rhs) {
			clear();
			deallocate_node(_node);
			_node = rhs._node;
			_size = rhs._size;
			rhs._node = nullptr;
			rhs._size = 0;
			return *this;
		}

		~forward_list(){
			clear();
			deallocate_node(_node);
		}

		size_type size(){
			return _size;
		}

		bool empty(){
			return _size == 0;
		}

		iterator before_begin(){
			return static_cast<iterator>(cbefore_begin());
		}

		iterator begin(){
			return static_cast<iterator>(cbegin());
		}

		iterator end(){
			return static_cast<iterator>(cend());
		}
		
		const_iterator cbefore_begin() const {
			return static_cast<const_iterator>(_node);
		}

		const_iterator cbegin() const {
			return static_cast<const_iterator>(_node->_next);
		}

		const_iterator cend() const {
			return static_cast<const_iterator>(nullptr);
		}

		void swap(forward_list& x) noexcept {
			swap(_node, x._node);
			swap(_size, x._size);
		}

		friend void swap(forward_list& a, forward_list& b) noexcept {
			a.swap(b);
		}
		void clear();
		iterator insert_after(iterator position, const value_type& x);
		void erase_after(iterator position);
		void push_front(const T& value);
		void pop_front();

		iterator find(const value_type& x) const;
		iterator find_before(const value_type& x) const;
	};

	template<typename T, template<typename> class Alloc>
	void forward_list<T, Alloc>::push_front(const T& value){
		link_type newnode = allocate_node(value);//= _alloc.allocate(1);
		newnode->_next = _node->_next;
		_node->_next = newnode;
		++_size;
	}

	template<typename T, template<typename> class Alloc>
	void forward_list<T, Alloc>::pop_front(){
		link_type to_deallocate = _node->_next;
		_node->_next = _node->_next->_next;
		deallocate_node(to_deallocate);
		--_size;
	}


	template<typename T, template<typename> class Alloc>
	void forward_list<T, Alloc>::erase_after(iterator it){
		if (!it._node) return;
		link_type to_erase = it._node->_next;
		if (!to_erase) return;
		link_type new_next = to_erase->_next;
		it._node->_next = new_next;
		deallocate_node(to_erase);
	}

	template<typename T, template<typename> class Alloc>
	auto forward_list<T, Alloc>::insert_after(iterator position, const value_type& x)
		->iterator{
		link_type new_node = allocate_node(x);
		new_node->_next = position._node->_next;
		position._node->_next = new_node;
		++_size;
		return iterator(new_node);
	}


	template<typename T, template<typename> class Alloc>
	void forward_list<T, Alloc>::clear(){
		if (empty()) return;
		link_type cur = _node->_next, next = cur->_next;
		while (cur = next){
			next = cur->_next;
			deallocate_node(cur);
		}
	}


	template<typename T, template<typename> class Alloc>
	auto forward_list<T, Alloc>::find(const value_type& x)const ->iterator{
		iterator ret = cbegin();
		for (; ret != cend(); ++ret)
		if (*ret == x)
			break;
		return ret;
	}

	template<typename T, template<typename> class Alloc>
	auto forward_list<T, Alloc>::find_before(const value_type& x)const ->iterator {
		iterator ret = cbefore_begin(), cur = cbegin();
		for (; cur != cend(); ret = cur++)
		if (*cur == x)
			break;
		return ret;
	}

}


#endif
