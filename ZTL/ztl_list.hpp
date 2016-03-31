#ifndef ZTL_LIST_HPP
#define ZTL_LIST_HPP

#include"ztl_allocator.hpp"
#include"ztl_iterator.hpp"
#include"ztl_construct.hpp"
namespace ztl{

	template<typename T>
	struct __list_node
	{
		__list_node<T>* _prev;
		__list_node<T>* _next;
		T element;
	};

	template<typename T, typename iterator,bool is_const>
	class __list_iterator_base :public bidirectional_iterator_base < T, iterator, is_const> {
		using base = bidirectional_iterator_base < T, iterator, is_const>;
	public:
		using value_type = typename base::value_type;
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using self = typename base::self;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

		using link_type = __list_node<value_type>*;

		using iterator_category = typename base::iterator_category;
	public:
		explicit __list_iterator_base(const link_type arg = nullptr) :_node(arg) {}
		__list_iterator_base(const __list_iterator_base& arg) = default;

		virtual bool operator==(const self& it) const override {
			return _node == it._node;
		}

		virtual self& operator++() override {
			_node = _node->_next;
			return base::rself();
		}

		using base::operator++;

		virtual self& operator--() override {
			_node = _node->_prev;
			return base::rself();
		}

		using base::operator--;
	protected:
		link_type _node;
	};



	template<typename T, template<typename> class Alloc>                                                                                                
	class list;
       	
	template<typename T>
	class __list_iterator :public __list_iterator_base<T, __list_iterator<T>, false> {
		using base = __list_iterator_base<T, __list_iterator<T>, false>;
		
		template<typename _T, template<typename> class Alloc>
		friend class list;
	public:
		using value_type = typename base::value_type;
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using self = typename base::self;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

		using link_type = __list_node<value_type>*;
		
		using iterator_category = typename base::iterator_category;

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
	class __list_const_iterator:public __list_iterator_base<T, __list_const_iterator<T>, true> {
		using base = __list_iterator_base< T, __list_const_iterator<T>, true>;
		
		template<typename _T, template<typename> class Alloc>
		friend class list;
	public:
		using value_type = typename base::value_type;
		using const_reference = typename base::reference;
		using const_pointer = typename base::pointer;
		using self = typename base::self;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

		using link_type = __list_node<value_type>*;

		using iterator_category = typename base::iterator_category;
		using _list_iterator = __list_iterator<T>;
	public:
		using base::base;
		__list_const_iterator(const _list_iterator& arg) :base(arg._node){}

		virtual const_reference operator*() const override {
			return static_cast<const_reference>(base::_node->element);
		}

		virtual const_pointer operator->() const override {
			return static_cast<const_pointer>(&base::_node->element);
		}
	};

	template<typename T, template<typename> class Alloc = Allocator>
	class list
	{
	public:
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;

		using iterator = __list_iterator<value_type>;
		using const_iterator = __list_const_iterator<value_type>;
		using const_reverse_iterator = _reverse_iterator<const_iterator>;
		using reverse_iterator = _reverse_iterator<iterator>;
		
		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using list_node = __list_node<value_type>;
		using link_type = list_node*;

	private:
		Alloc<list_node> _alloc;

	protected:
		link_type _node;
		size_type _size;

	public:
		list():_size(0){
			_node=_alloc.allocate(1);
			_node->_next = _node;
			_node->_prev = _node;
		}


		list(const list& _list) :list(_list.cbegin(), _list.cend()){
		}

		list(list&& _right) :_node(_right._node), _size(_right._size){
			_right._node = 0;
			_right._size = 0;
		}

		list(size_type nSize, const T& value) :list(){
			for (int i = 0; i < nSize; ++i)
				push_back(value);
		}

		list(std::initializer_list<value_type> init_list) :list(){
			insert(begin(), init_list.begin(), init_list.end());
		}

		template<typename InputIterator>
		list(InputIterator first, InputIterator last) :list(){
			insert(begin(), first, last);
		}

		list& operator=(const list& rhs) {
			clear();
			insert(begin(), rhs.begin(), rhs.end());
			return *this;
		}

		void operator=(list&& rhs) {
			clear();
			_alloc.deallocate(_node, 1);
			_node = rhs._node;
			rhs._node = nullptr;
			_size = rhs._size;
			rhs._size = 0;
		}

		~list(){
			clear();
			_alloc.deallocate(_node,1);
		}

		size_type size(){
			return _size;
		}

		bool empty(){
			return _size == 0;
		}

		iterator begin(){
			return static_cast<iterator>(_node->_next);
		}

		iterator end(){
			return static_cast<iterator>(_node);
		}

		iterator last() {
			return static_cast<iterator>(_node->_prev);
		}

		reverse_iterator rbegin() {
			return static_cast<reverse_iterator>(_node->_prev);
		}

		reverse_iterator rend() {
			return static_cast<reverse_iterator>(_node);
		}

		reverse_iterator rlast() {
			return static_cast<reverse_iterator>(_node->_next);
		}

		const_iterator cbegin() const{
			return static_cast<const_iterator>(_node->_next);
		}

		const_iterator cend() const{
			return static_cast<const_iterator>(_node);
		}

		const_iterator clast() const {
			return static_cast<const_iterator>(_node->_prev);
		}

		const_reverse_iterator crbegin() const {
			return static_cast<const_reverse_iterator>(_node->_prev);
		}

		const_reverse_iterator crend() const {
			return static_cast<const_reverse_iterator>(_node);
		}

		const_reverse_iterator crlast() const {
			return static_cast<iterator>(_node->_next);
		}

		iterator insert(iterator position, const T& value);

		template<typename InputIterator>
		iterator insert(iterator position, InputIterator first, InputIterator last);

		void erase(iterator first, iterator last);
		void erase(iterator it);

		void clear(){
			erase(begin(), end());
		}

		reference front() {
			return *begin();
		}

		const_reference front() const {
			return *begin();
		}

		reference back() {
			return *last();
		}

		const_reference back() const {
			return *last();
		}

		void push_back(const T& value);
		void push_back(T&& value);
		void pop_back();

		void push_front(const T& value);
		void push_front(T&& value);
		void pop_front();
	};


	template<typename T, template<typename> class Alloc>
	void list<T, Alloc>::push_back(T&& value){
		link_type newnode = _alloc.allocate(1);
		construct(&(newnode->element), ztl::move(value));
		newnode->_next = _node;
		newnode->_prev = _node->_prev;
		_node->_prev->_next = newnode;
		_node->_prev = newnode;
		++_size;
	}

	template<typename T, template<typename> class Alloc>
	void list<T, Alloc>::push_back(const T& value){
		link_type newnode = _alloc.allocate(1);
		construct(&(newnode->element), value);
		newnode->_next = _node;
		newnode->_prev = _node->_prev;
		_node->_prev->_next = newnode;
		_node->_prev = newnode;
		++_size;
	}

	template<typename T, template<typename> class Alloc>
	void list<T, Alloc>::pop_back(){
		link_type to_deallocate = _node->_prev;
		destory(&to_deallocate->element);
		_node->_prev = _node->_prev->_prev;
		_node->_prev->_next = _node;
		_alloc.deallocate(to_deallocate,1);
		--_size;
	}

	template<typename T, template<typename> class Alloc>
	void list<T, Alloc>::push_front(const T& value){
		link_type newnode = _alloc.allocate(1);
		construct(&(newnode->element), value);
		newnode->_next = _node->_next;
		newnode->_prev = _node;
		_node->_next->_prev = newnode;
		_node->_next = newnode;
		++_size;
	}

	template<typename T, template<typename> class Alloc>
	void list<T, Alloc>::push_front(T&& value) {
		link_type newnode = _alloc.allocate(1);
		construct(&(newnode->element), (ztl::move(value)));
		newnode->_next = _node->_next;
		newnode->_prev = _node;
		_node->_next->_prev = newnode;
		_node->_next = newnode;
		++_size;
	}

	template<typename T, template<typename> class Alloc>
	void list<T, Alloc>::pop_front(){
		link_type to_deallocate = _node->_next;
		destory(&to_deallocate->element);
		_node->_next = _node->_next->_next;
		_node->_prev = _node;
		_alloc.deallocate(to_deallocate, 1);
		--_size;
	}

	template<typename T, template<typename> class Alloc>
	auto list<T, Alloc>::insert(iterator position, const T& value)
		->iterator{
		link_type newnode = _alloc.allocate(1);
		construct(&newnode->element, value);
		newnode->_next = position._node->_next;
		newnode->_prev = position._node;
		position._node->_next->_prev = newnode;
		position._node->_next = newnode;
		++_size;
		return iterator(newnode);
	}

	template<typename T, template<typename> class Alloc>
	template<typename InputIterator>
	auto list<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last)
		->iterator{
		iterator loop = position;
		iterator ret = position;
		for (InputIterator it = first; it != last; ++it, ++loop, ++_size)
			insert(loop, *it);
		return ++ret;
	}

	template<typename T, template<typename> class Alloc>
	void list<T, Alloc>::erase(iterator first, iterator last){
		_size -= distance(first, last);
		last._node->_prev = first._node->_prev;
		first._node->_prev->_next = last._node;
		decltype(first._node) next;
		for (auto node = first._node; node != last._node; node = next){
			next = node->_next;
			destory(&node->element);
			_alloc.deallocate(node,1);
		}
	}

	template<typename T, template<typename> class Alloc>
	void list<T, Alloc>::erase(iterator it){
		iterator end = it;
		erase(it,++end);
	}
}

#endif	//ZTL_LIST_HPP
