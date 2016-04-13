#ifndef ZTL_ITERATOR_HPP
#define ZTL_ITERATOR_HPP

#include<cstddef>

namespace ztl{

	struct input_iterator_tag {};
	struct output_iterator_tag {};
	struct forward_iterator_tag : public input_iterator_tag {};
	struct bidirectional_iterator_tag : public forward_iterator_tag {};
	struct random_access_iterator_tag : public bidirectional_iterator_tag {};

	template <class _Category, class _Tp, class _Distance = ptrdiff_t,
	class _Pointer = _Tp*, class _Reference = _Tp&>
	struct iterator {
		using iterator_category		=	_Category;
		using value_type			=	_Tp;
		using difference_type		=	_Distance;
		using pointer				=	_Pointer;
		using reference				=	_Reference;
	};




	template <typename _Iterator>
	struct iterator_traits {
		using iterator_category		=	typename _Iterator::iterator_category; 
		using value_type			=	typename _Iterator::value_type;
		using difference_type		=	typename _Iterator::difference_type;
		using pointer				=	typename _Iterator::pointer;
		using reference				=	typename _Iterator::reference;
	};


	//Œ™¡À  ≈‰MSstl

#ifdef _VECTOR_
	template <typename value_T>
	struct iterator_traits <std::_Vector_iterator<std::_Vector_val<std::_Simple_types<value_T>>>> {
		using iterator_category		=	random_access_iterator_tag;
		using value_type			=	typename std::_Vector_iterator<std::_Vector_val<std::_Simple_types<value_T>>>::value_type;
		using difference_type		=	ptrdiff_t;
		using pointer				=	value_type*;
		using reference				=	value_type&;
	};
#endif

#ifdef _LIST_
	template <typename value_T>
	struct iterator_traits <std::_List_iterator<std::_List_val<std::_List_simple_types<value_T>>>> {
		using iterator_category		=	bidirectional_iterator_tag;
		using value_type			=	typename std::_List_iterator<std::_List_val<std::_List_simple_types<value_T>>>::value_type;
		using difference_type		=	ptrdiff_t;
		using pointer				=	value_type*;
		using reference				=	value_type&;
	};
#endif


	template <typename _Tp>
	struct iterator_traits<_Tp*> {
		using iterator_category		=	random_access_iterator_tag ;
		using value_type			=	_Tp;
		using difference_type		=	ptrdiff_t;
		using pointer				=	_Tp*;
		using reference				=	_Tp&;
	};

	template <typename _Tp>
	struct iterator_traits<const _Tp*> {
		using iterator_category		=	random_access_iterator_tag ;
		using value_type			=	_Tp;
		using difference_type		=	ptrdiff_t;
		using pointer				=	const _Tp*;
		using reference				=	const _Tp&;
	};

	template <class _Iter>
	inline typename iterator_traits<_Iter>::iterator_category
		_iterator_category(const _Iter&)
	{
		using category = typename iterator_traits<_Iter>::iterator_category;
		return category();
	}



	template <class _InputIterator>
	inline typename iterator_traits<_InputIterator>::difference_type
		_distance(_InputIterator _first, _InputIterator _last, input_iterator_tag)
	{
		typename iterator_traits<_InputIterator>::difference_type ret = 0;
		while (_first != _last) {
			++_first; ++ret;
		}
		return ret;
	}

	template <class _RandomAccessIterator>
	inline typename iterator_traits<_RandomAccessIterator>::difference_type
		_distance(_RandomAccessIterator _first, _RandomAccessIterator _last,
		random_access_iterator_tag) {
			return _last - _first;
		}

	template <class _InputIterator>
	inline typename iterator_traits<_InputIterator>::difference_type
		distance(_InputIterator _first, _InputIterator _last) {
			using category = typename iterator_traits<_InputIterator>::iterator_category;
			return ztl::_distance(_first, _last, category());
		}

	template <class _InputIter, class _Distance>
	inline void _advance(_InputIter& _i, _Distance _n, input_iterator_tag) {
		while (_n--) ++_i;
	}

	template <class _BidirectionalIterator, class _Distance>
	inline void _advance(_BidirectionalIterator& _i, _Distance _n,
		bidirectional_iterator_tag) {
		if (_n >= 0)
		while (_n--) ++_i;
		else
		while (_n++) --_i;
	}

	template <class _RandomAccessIterator, class _Distance>
	inline void _advance(_RandomAccessIterator& _i, _Distance _n,
		random_access_iterator_tag) {
		_i += _n;
	}

	template <class _InputIterator, class _Distance>
	inline void advance(_InputIterator& _i, _Distance _n) {

		_advance(_i, _n, _iterator_category(_i));
	}

	template<typename T, typename Iterator>
	class __iterator_base_r {
	public:
		using self = Iterator;
		using value_type = T;
		using size_type = size_t;
		using difference_type = ptrdiff_t;

	public:
		virtual bool operator==(const self&) const = 0;
		virtual self& operator=(const self&) = 0;

		self& rself(){
			return static_cast<self&>(*this);
		}

		const self& rself() const {
			return static_cast<const self&>(*this);
		}

		bool operator!=(const self& rhs) const {
			return !(*this == rhs);
		};
	};

	template<typename T, typename Iterator>
	class __iterator_base :public __iterator_base_r<T, Iterator>{
		using base = __iterator_base_r<T, Iterator>;
	public:
		using value_type = typename base::value_type;
		using self = typename base::self;
		using pointer = typename base::value_type*;
		using reference = typename base::value_type&;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

	};

	template<typename T, typename Iterator>
	class __const_iterator_base :public __iterator_base_r<T, Iterator> {
		using base = __iterator_base_r<T, Iterator>;
	public:
		using value_type = typename base::value_type;
		using self = typename base::self;
		using const_pointer = const typename base::value_type*;
		using const_reference = const typename base::value_type&;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

	};


	template<typename T, typename Iterator, bool is_const >
	class iterator_base;

	template<typename T, typename Iterator>
	class iterator_base<T, Iterator, false> :public __iterator_base<T, Iterator> {
		using base = __iterator_base<T, Iterator>;
	public:
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using value_type = typename base::value_type;
		using self = typename base::self;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;
	};

	template<typename T, typename Iterator>
	class iterator_base<T, Iterator, true> :public __const_iterator_base<T, Iterator> {
		using base = __const_iterator_base<T, Iterator>;
	public:
		using reference = typename base::const_reference;
		using pointer = typename base::const_pointer;
		using value_type = typename base::value_type;
		using self = typename base::self;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;
	};

	template<typename T, typename Iterator, bool is_const >
	class forward_iterator_base : public iterator_base<T, Iterator, is_const> {
		using base = iterator_base<T, Iterator, is_const>;
	public:
		using value_type = typename base::value_type;
		using self = typename base::self;
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

		using iterator_category = forward_iterator_tag;

	public:
		virtual reference operator*() const = 0;

		virtual pointer operator->() const = 0;

		virtual self& operator++() = 0;

		self operator++(int) {
			self ret(base::rself());
			++*this;
			return ret;
		}

	};

	template<typename T, typename Iterator, bool is_const >
	class bidirectional_iterator_base : public forward_iterator_base<T, Iterator, is_const> {
		using base = forward_iterator_base<T, Iterator, is_const>;
	public:
		using value_type = typename base::value_type;
		using self = typename base::self;
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

		using iterator_category = bidirectional_iterator_tag;

	public:
		virtual self& operator--() = 0;

		self operator--(int) {
			self ret(*this);
			--*this;
			return ret;
		}

	};

	template<typename T, typename Iterator, bool is_const >
	class random_iterator_base : public bidirectional_iterator_base<T, Iterator, is_const>{
		using base = bidirectional_iterator_base<T, Iterator, is_const>;
	public:
		using value_type = typename base::value_type;
		using self = typename base::self;
		using reference = typename base::reference;
		using pointer = typename base::pointer;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;

		using iterator_category = random_access_iterator_tag;
	public:

		reference operator[](difference_type n) const {
			return *(*this + n);
		}

		self operator-(difference_type n) const {
			self ret = static_cast<self&>((random_iterator_base&)*this);
			return ret -= n;
		}

		self& operator-=(difference_type n) {
			return *this += -n;
		}

		const self operator+(difference_type n) const {
			self ret(base::rself());
			return ret += n;
		}

		virtual self& operator+=(difference_type n) = 0;
		virtual const difference_type operator-(const self& x) const = 0;
	};


	template<typename Iterator>
	class _reverse_iterator
	{
	public:
		using iterator_category = typename iterator_traits<Iterator>::iterator_category ;
		using difference_type = typename iterator_traits<Iterator>::difference_type;
		using value_type = typename iterator_traits<Iterator>::value_type;
		using reference = typename iterator_traits<Iterator>::reference;
		using pointer = typename iterator_traits<Iterator>::pointer;
		using self = _reverse_iterator;

	public:
		explicit _reverse_iterator(const Iterator& it) :_it(it){}
		~_reverse_iterator() = default;

		reference operator*() const {
			return *_it;//_it.operator*(); useless to ordinary pointer
		}

		pointer operator->() const {
			return _it.operator->();
		}

		self& operator++(){ 
			--_it; 
			return *this;
		}
		
		const self operator++(int){ 
			auto ret = *this;
			++*this;
			return ret;
		}

		self& operator--(){
			++_it;
			return *this;
		}

		const self operator--(int){
			auto ret = *this;
			--*this;
			return ret;
		}

		bool operator==(const self& rit) const {
			return _it == rit._it;
		}

		bool operator!=(const self& rit) const {
			return _it != rit._it;
		}

	private:
		Iterator _it;

	};

}

#endif	//ZTL_ITERATOR_HPP
