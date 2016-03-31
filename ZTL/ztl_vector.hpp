#ifndef ZTL_VECTOR_HPP
#define ZTL_VECTOR_HPP

#include"ztl_uninitialized.hpp"
#include"ztl_allocator.hpp"

namespace ztl{
	template<typename T, typename iterator,bool is_const>
	class __vector_iterator_base:public random_iterator_base<T, iterator, is_const> {
	public:
		using base = random_iterator_base<T, iterator, is_const>;
		using value_type = typename base::value_type;
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using self = typename base::self;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

		using iterator_category = typename base::iterator_category;
	public:
		explicit __vector_iterator_base(pointer p) :_p(p) {}
		__vector_iterator_base(const __vector_iterator_base&) = default;

		virtual bool operator==(const self& rhs) const override {
			return _p == rhs._p;
		}

		virtual self& operator++() override {
			++_p;
			return base::rself();
		}
		
		using base::operator++;

		virtual self& operator--() override {
			--_p;
			return base::rself();
		}

		using base::operator--;

		virtual self& operator+=(difference_type n) override {
			_p += n;
			return static_cast<self&>(*this);
		}

		using base::operator-;

		virtual const difference_type operator-(const self& x) const override {
			return static_cast<difference_type>(_p - x._p);
		}

		virtual self& operator=(const self& x) override {
			_p = x._p;
			return base::rself();
		}
	protected:
		pointer _p;

	};

	template<typename T>
	class __vector_const_iterator;

	template<typename T>
	class __vector_iterator:public __vector_iterator_base<T, __vector_iterator<T>, false>{
		using base = __vector_iterator_base<T, __vector_iterator<T>, false>;
		friend class __vector_const_iterator<T>;
	public:
		using value_type = typename base::value_type;
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using self = typename base::self;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;
		
		using iterator_category = typename base::iterator_category;
	public:  
		using base::base;

		virtual reference operator*() const override {
			return *base::_p;
		}

		virtual pointer operator->() const override {
			return base::_p;
		}
	
	};

	template<typename T>
	class __vector_const_iterator:public __vector_iterator_base<T, __vector_const_iterator<T>, true> {
		using base = __vector_iterator_base<T, __vector_const_iterator<T>, true>;
	public:
		using value_type = typename base::value_type;
		using const_reference = typename base::reference;
		using const_pointer = typename base::pointer;
		using self = typename base::self;

		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

		using iterator_category = typename base::iterator_category;
	public:
		using base::base;
		__vector_const_iterator(const __vector_iterator<T>& x):base(x._p){}

		virtual const_reference operator*() const override {
			return *base::_p;
		}

		virtual const_pointer operator->() const override {
			return base::_p;
		}

	};

	template<typename T, template<typename> class Alloc = Allocator>
	class vector
	{
	public:
		using value_type = T;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using reference = value_type&;
		using const_reference = const value_type&;

		using iterator = __vector_iterator<T>;
		using const_iterator = __vector_const_iterator<T>;
		using const_reverse_iterator = _reverse_iterator<const_iterator>;
		using reverse_iterator = _reverse_iterator<iterator>;

		using size_type = size_t;
		using difference_type = ptrdiff_t;

	private:
		Alloc<T> _alloc;

	public:
		vector():start(0),finish(0),end_of_storage(0){
			checkcapacity();
		}

		vector(const vector& _arg):vector(_arg.cbegin(), _arg.cend()){
		}

		vector(vector&& _right){
			start = _right.start;
			finish = _right.finish;
			end_of_storage = _right.end_of_storage;

			_right.start = _right.finish = _right.end_of_storage = 0;
		}

		explicit vector(size_type nSize) :start(0), finish(0), end_of_storage(0) {
			checkcapacity(nSize);
		}

		vector(size_type nSize, const T& value) :start(0), finish(0), end_of_storage(0){
			checkcapacity(nSize);
			uninitialized_fill_n(start, nSize, value);
			finish += nSize;
		}

		vector(std::initializer_list<value_type> list) :start(0), finish(0), end_of_storage(0){
			checkcapacity(list.size());
			insert(begin(), list.begin(), list.end());
		}

		template<typename InputIterator>
		vector(InputIterator first, InputIterator last) : start(0), finish(0), end_of_storage(0){
			checkcapacity(ztl::distance(first, last)+1);
			insert(begin(), first, last);
		}

		vector& operator=(const vector& rit) {
			destory(start, finish);
			_alloc.deallocate(start, (end_of_storage - start));
			start = finish = end_of_storage = 0;
			checkcapacity(ztl::distance(rit.cbegin(), rit.cend()) + 1);
			insert(begin(), rit.cbegin(), rit.cend());
			return *this;
		}

		vector& operator=(vector&& rit) {
			destory(start, finish);
			_alloc.deallocate(start, (end_of_storage - start));
			//start = finish = end_of_storage = 0;
			start = rit.start;
			finish = rit.finish;
			end_of_storage = rit.end_of_storage;
			rit.start = rit.finish = rit.end_of_storage = 0;
			return *this;
		}

		~vector(){
			destory(start, finish);
			_alloc.deallocate(start, (end_of_storage - start));
		}

		size_type capacity() const {
			return end_of_storage - start;
		}

		size_type size() const {
			return finish - start;
		}

		bool empty() const { return size() == 0; }

		pointer data() const {
			return start;
		}

		iterator begin(){
			return static_cast<iterator>(start);
		}

		iterator end(){
			return static_cast<iterator>(finish);
		}

		reverse_iterator rbegin() {
			return static_cast<reverse_iterator>(iterator(finish - 1));
		}

		reverse_iterator rend() {
			return static_cast<reverse_iterator>(iterator(start - 1));
		}

		const_iterator cbegin() const {
			return static_cast<const_iterator>(start);
		}

		const_iterator cend() const {
			return static_cast<const_iterator>(finish);
		}

		const_reverse_iterator crbegin() const {
			return static_cast<const_reverse_iterator>(const_iterator(finish - 1));
		}

		const_reverse_iterator crend() const {
			return static_cast<const_reverse_iterator>(const_iterator(start - 1));
		}

		iterator insert(iterator position, const T& value);

		template<typename InputIterator>
		iterator insert(iterator position, InputIterator first, InputIterator last);

		iterator insert_n(iterator position, size_type n, const T& value);

		void erase(iterator first, iterator last);

		void clear(){
			erase(begin(), end());
		}

		void push_back(const value_type& value);
		void push_back(value_type&& value);
		template<class... Args>
		void emplace_back(Args&&...);

		value_type pop_back();

		void reserve(size_type n){ checkcapacity(n); }

		void swap(vector& rit){
			ztl::swap(start, rit.start);
			ztl::swap(finish, rit.finish);
			ztl::swap(end_of_storage, rit.end_of_storage);

		}

		void resize(size_type new_size){
			if (new_size < size())
				erase(static_cast<iterator>(start + new_size), static_cast<iterator>(finish));
			else
				insert_n(end(), new_size - size(), value_type());
		}

		reference operator[](size_type n){
			return start[n];
		}

		const_reference operator[](size_type n) const {
			return start[n];
		}

		friend void swap(vector& a, vector& b) noexcept {
			swap(a.start, b.start);
			swap(a.finish, b.finish);
			swap(a.end_of_storage, b.end_of_storage);
		}

		friend bool operator==(const vector& v1, const vector& v2) {
			bool ret = v1.size() == v2.size();
			if (!ret) return false;
			for (auto it1 = v1.cbegin(), it2 = v2.cbegin(); it1 != v1.cend(); ++it1, ++it2)
				if (*it1 != *it2)
					return false;
			return true;
		}
	private:
		void extend(difference_type nCount);

		void shrink();

		void checkcapacity(difference_type nCount = 1){
			if ((end_of_storage - finish) < nCount){
				extend(nCount);
			}
		}

		void checkerase(){
			if (capacity() > 5 && capacity() / (size()?size():1) >= 3)
				shrink();
		}


	private:
		pointer start;
		pointer finish;
		pointer end_of_storage;

		iterator start_it() { return static_cast<iterator>(start); }
		iterator finish_it() { return static_cast<iterator>(finish); }
		iterator end_of_storage_it() { return static_cast<iterator>(end_of_storage); }

	};


	template<typename T, template<typename> class Alloc>
	void vector<T, Alloc>::push_back(const T& value){
		checkcapacity();
		construct(finish++, value);
	}

	template<typename T, template<typename> class Alloc>
	void vector<T, Alloc>::push_back(T&& value){
		checkcapacity();
		construct(finish++, ztl::move(value));
	}

	template<typename T, template<typename> class Alloc>
	auto vector<T, Alloc>::pop_back()
		->value_type {
		value_type ret = *--finish;
		destory(finish);
		checkerase();
		return ret;
	}

	template<typename T, template<typename> class Alloc>
	void vector<T, Alloc>::extend(difference_type nCount){
		size_type sizeneed = nCount + size();
		size_type sizedefault = size_type(capacity()*1.5);
		size_type newsize = sizedefault > sizeneed ? (start ? sizedefault : 2) : sizeneed;
		pointer newstart = _alloc.allocate(newsize);
		ztl::uninitialized_copy(start, finish, newstart);
		if (start)
			_alloc.deallocate(start, (end_of_storage - start));
		finish = newstart + (finish - start);
		start = newstart;
		end_of_storage = start + newsize;
	}

	template<typename T, template<typename> class Alloc>
	auto vector<T, Alloc>::insert(iterator position, const T& value)
		->iterator{
		size_type dis = distance(start_it(), position);
		checkcapacity();
		position = start_it() + dis;
		//memmove(position + 1, position, (finish - position)*sizeof(T));
		copy(position, finish_it(), position + 1);
		new(&*position)T(value);
		++finish;
		return position;
	}

	template<typename T, template<typename> class Alloc>
	template<typename InputIterator>
	auto vector<T, Alloc>::insert(iterator position, InputIterator first, InputIterator last)
		->iterator{
		using ptrdiff = typename iterator_traits<InputIterator>::difference_type;
		ptrdiff_t j = ztl::distance(start_it(), position);
		ptrdiff i = ztl::distance(first, last);
		checkcapacity(i+1);
		position = start_it() + j;
		if ((position) != finish_it())
			copy(position, finish_it(), position + i);
		ztl::copy(first, last, position);
		finish += i;
		return position - j;
	}

	template<typename T, template<typename> class Alloc>
	auto vector<T, Alloc>::insert_n(iterator position, size_type n, const T& value)
	->iterator{
		for (size_type i = 0; i != n; i++)
			position = insert(position, value);
		return position;
	}


	template<typename T, template<typename> class Alloc>
	void vector<T, Alloc>::erase(iterator first, iterator last){
		destory(first, last);
		copy(static_cast<iterator>(last), static_cast<iterator>(finish), static_cast<iterator>(first));
		finish = finish - (last - first);
		checkerase();
	}

	template<typename T, template<typename> class Alloc>
	void vector<T, Alloc>::shrink(){
		size_type old_size = capacity();
		size_type new_size = old_size / 2;
		pointer new_start = _alloc.allocate(new_size);
		uninitialized_copy(start, finish, new_start);
		_alloc.deallocate(start, (end_of_storage - start));
		finish = new_start + (finish - start);
		start = new_start;
		end_of_storage = start + new_size;
	}

	template<typename T, template<typename> class Alloc>
	template<class... Args>
	void vector<T, Alloc>::emplace_back(Args&&... args){
		checkcapacity();
		construct(finish++, ztl::forward(args...));
	}
}

#endif	//ZTL_VECTOR_HPP
