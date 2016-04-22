#ifndef ZTL_DEQUE_HPP
#define ZTL_DEQUE_HPP

#include"ztl_allocator.hpp"
#include"ztl_iterator.hpp"
#include"ztl_construct.hpp"
#include"ztl_algorithm.hpp"
namespace ztl{

	constexpr size_t __deque_buf_size(size_t n, size_t sz){
		return n != 0 ?
				n : 
				sz < 512 ? size_t(512 / sz) : size_t(1);
	}

	template<typename T, size_t Bufsize,typename iterator, bool is_const>
	class __deque_iterator_base:public random_iterator_base<T,iterator,is_const> {
	private:
		using base = random_iterator_base<T, iterator, is_const>;
	public:
		using value_type = typename base::value_type;
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using self = typename base::self;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

		using iterator_category = typename base::iterator_category;

		using map_pointer = pointer*;
	protected:
		static constexpr size_type buffer_size() { return __deque_buf_size(Bufsize, sizeof(T)); }

		pointer current = nullptr;
		pointer first = nullptr;
		pointer last = nullptr;
		map_pointer node = nullptr;

	public:

		__deque_iterator_base() = default;

		void set_node(map_pointer new_node) {
			node = new_node;
			first = *new_node;
			last = *new_node + difference_type(buffer_size());
		}

		virtual self& operator++() {
			++current;
			if (current == last) {
				set_node(node + 1);
				current = first;
			}
			return base::rself();
		}

		using base::operator++;

		virtual self& operator--() {
			if (current == first) {
				set_node(node - 1);
				current = last - 1;
			}
			--current;
			return base::rself();
		}

		using base::operator--;

		virtual self& operator+=(difference_type n) {
			difference_type offset = n + (current - first);
			if (offset >= 0 && offset < static_cast<difference_type>(buffer_size())) {
				current += n;
			}
			else {
				difference_type node_offset = offset>0 ? offset / difference_type(buffer_size())
					: -difference_type((-offset - 1) / buffer_size()) - 1;
				set_node(node + node_offset);
				current = first + (offset - node_offset*difference_type(buffer_size()));
			}
			return base::rself();
		}


		self& operator-=(difference_type n) {
			return *this += -n;
		}

		virtual bool operator==(const self& rit) const override {
			return current == rit.current;
		}

		using base::operator-;

		virtual const difference_type operator-(const self& rit) const {
			return (buffer_size()*(node - rit.node - 1) + (current - first) + (rit.last - rit.current));
		}
	};

	template<typename T, size_t Bufsize>
	struct __deque_const_iterator;

	template<typename T, size_t Bufsize>
	struct __deque_iterator:public __deque_iterator_base<T, Bufsize, __deque_iterator<T, Bufsize>, false>{
		using base = __deque_iterator_base<T, Bufsize, __deque_iterator, false>;
		
		template<typename _T, template<typename> class Alloc, size_t _Bufsize>
		friend class deque;

		friend struct __deque_const_iterator < T, Bufsize >;

	public:
		using value_type = typename base::value_type;
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using self = typename base::self;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

		using iterator_category = typename base::iterator_category;

		using map_pointer = pointer*;

	public:
		using base::base;

		virtual reference operator*() const override {
			return *base::current;
		}

		virtual pointer operator->() const override {
			return &*base::current;
		}

		bool operator<(const self& rit) const {
			return base::node == rit.node ? base::current < rit.current : base::node < rit.node;
		}

	};

	template<typename T, size_t Bufsize>
	struct __deque_const_iterator :public __deque_iterator_base < T, Bufsize, __deque_const_iterator<T, Bufsize>, true> {
		using base = __deque_iterator_base<T, Bufsize, __deque_const_iterator, true>;
		
		template<typename _T, template<typename> class Alloc, size_t _Bufsize>
		friend class deque;
	public:
		using value_type = typename base::value_type;
		using const_reference = typename base::reference;
		using const_pointer = typename base::pointer;
		using self = typename base::self;

		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

		using iterator_category = typename base::iterator_category;

		using map_pointer = const_pointer*;
		
		using _deque_iterator = __deque_iterator<T, Bufsize>;
	
	public:
		//__deque_const_iterator(){}
		using base::base;
		__deque_const_iterator(const _deque_iterator& arg) :base::current(arg.current), base::first(arg.first), base::last(arg.last), base::node(arg.node){}

		virtual const_reference operator*() const {
			return static_cast<const_reference>(*base::current);
		}

		virtual const_pointer operator->() const {
			return static_cast<const_pointer>(&*base::current);
		}

		bool operator<(const self& rit) const {
			return base::node == rit.node ? base::current < rit.current : base::node < rit.node;
		}

	};

	template<typename T, template<typename> class Alloc = Allocator, size_t Bufsize = 0>
	class deque{
		enum{ DEFAULT_MAP_SIZE = 8 };
	public:
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;

		using iterator = __deque_iterator<value_type, Bufsize>;
		using const_iterator = __deque_const_iterator<value_type, Bufsize>;

		using size_type = size_t;
		using difference_type = ptrdiff_t;

		using map_pointer = pointer*;

	protected:
		
		iterator start, finish;
		map_pointer	bufmap;
		size_type map_size;

	private:
		Alloc<value_type> _alloc;
		Alloc<pointer> _alloc_map;
		
		pointer allocate_node(){
			return _alloc.allocate(iterator::buffer_size());
		}

		template<typename InputIterator>
		void _construct(InputIterator begin, InputIterator end) {
			for (auto it = begin; begin != end; ++begin)
				push_back(*begin);
		}

	public:
		deque() :bufmap(0), map_size(0){ create_map_and_nodes(0); }

		deque(size_type n, const value_type& value) :bufmap(0), map_size(0){
			fill_initialize(n, value);
		}

		deque(std::initializer_list<value_type> list) :deque(){
			_construct(list.begin(), list.end());
		}

		deque(const deque& rit) :deque(){
			_construct(rit.begin(), rit.end());
		}

		deque(deque&& rit) :deque(){
			start = rit.start;
			finish = rit.finish;
			bufmap = rit.bufmap;
			map_size = rit.map_size;
			rit.start = rit.finish;
			rit.bufmap = nullptr;
		}

		~deque(){
			clear();
			_alloc_map.deallocate(bufmap, map_size);
		}

		iterator begin(){
			return start;
		}

		const_iterator cbegin() const {
			return static_cast<const_iterator>(start);
		}

		iterator end(){
			return finish;
		}

		const_iterator cend() const {
			return static_cast<const_iterator>(finish);
		}

		reference operator[](size_type n){
			return start[n];
		}

		const_reference operator[](size_type n) const {
			return start[n];
		}

		reference front(){
			return *start;
		}

		reference back(){
			return *(finish - 1);
		}

		size_type size() const noexcept{
			return finish - start;
		}

		const size_type max_size() const {
			return size_type(-1);
		}

		bool empty() const noexcept{
			return start == finish;
		}

	private:
		void fill_initialize(size_type n, const value_type& value);

		void create_map_and_nodes(size_type n);

		void check_map_back(size_t num_nodes_to_add = 1) {
			if (num_nodes_to_add + 1 > map_size - (finish.node - bufmap))
				reallocate_map(num_nodes_to_add, false);
		}

		void check_map_front(size_t num_nodes_to_add = 1) {
			if (num_nodes_to_add + 1 > start.node - bufmap)
				reallocate_map(num_nodes_to_add, true);
		}

		void reallocate_map(size_t num_nodes_to_add, bool add_at_front);

	public:
		void push_back(const value_type& value);

		void push_front(const value_type& value);

		value_type pop_back();
		value_type pop_front();

		void clear(){
			destory(begin(), end());
			map_pointer nstart = bufmap + (map_size - 1) / 2;
			map_pointer nfinish = nstart;
			start.set_node(nstart);
			finish.set_node(nfinish);
			start.current = start.first;
			finish.current = finish.first;
		}
	};

	template<typename T, template<typename> class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::fill_initialize(size_type n, const value_type& value){
		create_map_and_nodes(n);
		map_pointer current;
		for (current = start.node; current < finish.node; ++current)
			uninitialized_fill(*current, *current + iterator::buffer_size(), value);
		uninitialized_fill(finish.first, finish.current, value);
	}

	template<typename T, template<typename> class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::create_map_and_nodes(size_type n){
		size_type num_nodes = n / iterator::buffer_size() + 1;
		map_size = ztl_max(DEFAULT_MAP_SIZE, num_nodes + 2);
		bufmap = _alloc_map.allocate(map_size);
		map_pointer nstart = bufmap + (map_size - num_nodes) / 2;
		map_pointer nfinish = nstart + num_nodes - 1;
		map_pointer current;
		for (current = nstart; current <= nfinish; ++current)
			*current = allocate_node();
		start.set_node(nstart);
		finish.set_node(nfinish);
		start.current = start.first;
		finish.current = finish.first + n% iterator::buffer_size();
	}

	template<typename T, template<typename> class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::push_back(const value_type& value){
		construct(&*finish,value);
		if (finish.current != finish.last - 1){
			++finish.current;
		}
		else{
			check_map_back();
			*(finish.node + 1) = allocate_node();
			finish.set_node(finish.node + 1);
			finish.current = finish.first;
		}
	}

	template<typename T, template<typename> class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::push_front(const value_type& value){
		if (start.current != start.first){
			--start.current;
			construct(&*start, value);
		}
		else{
			check_map_front();
			*(start.node - 1) = allocate_node();
			start.set_node(start.node - 1);
			start.current = start.last - 1;
			construct(start.current, value);
		}
	}

	template<typename T, template<typename> class Alloc, size_t Bufsize>
	void deque<T, Alloc, Bufsize>::reallocate_map(size_t num_nodes_to_add, bool add_at_front){
		size_t old_num_nodes = finish.node - start.node + 1;
		size_t new_num_nodes = old_num_nodes + num_nodes_to_add;
		map_pointer new_start;
		if (map_size > 2 * new_num_nodes){
			new_start = bufmap + (((map_size - new_num_nodes) / 2) + (add_at_front ? num_nodes_to_add : 0));
			copy(start.node, finish.node + 1, new_start);
		}
		else{
			size_t new_map_size = map_size + ztl_max(map_size, num_nodes_to_add) + 2;
			map_pointer new_bufmap = _alloc_map.allocate(new_map_size);
			new_start = new_bufmap + ((new_map_size - new_num_nodes) / 2) + (add_at_front ? num_nodes_to_add : 0);
			copy(start.node, finish.node + 1, new_start);
			_alloc_map.deallocate(bufmap, map_size);
			bufmap = new_bufmap;
			map_size = new_map_size;
		}
		start.set_node(new_start);
		finish.set_node(new_start + old_num_nodes - 1);
	}

	template<typename T, template<typename> class Alloc, size_t Bufsize>
	auto deque<T, Alloc, Bufsize>::pop_back()
		->value_type{
		if (finish.current != finish.first){
			--finish.current;
		}
		else{
			_alloc_map.deallocate(finish.node,1);
			finish.set_node(finish.node - 1);
			finish.current = finish.last - 1;
		}
		value_type ret = move(*finish.current);
		destory(finish.current);
		return move(ret);
	}

	template<typename T, template<typename> class Alloc, size_t Bufsize>
	auto deque<T, Alloc, Bufsize>::pop_front()
	->value_type{
		value_type ret = move(*start.current);

		destory(start.current);
		if (start.current != start.last - 1)
			++start.current;
		else{
			_alloc_map.deallocate(start.node,1);
			start.set_node(start.node + 1);
			start.current = start.first;
		}
		return move(ret);
	}
}



#endif //ZTL_DEQUE_HPP
