#ifndef ZTL_STRING_HPP
#define ZTL_STRING_HPP

#include"ztl_allocator.hpp"
#include"ztl_iterator.hpp"
//#include<string>

namespace ztl{
	

	//using string = std::string;
	//using wstring = std::wstring;


	template<typename CharT,
		//class Traits = std::char_traits<CharT>,
		template<typename > class _Alloc = Allocator> 
	class basic_string{
	public:
		
		using value_type = CharT;
		using allocator_type = _Alloc<value_type>;
		using size_type = typename allocator_type::size_type;
		using difference_type = typename allocator_type::difference_type;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using iterator = value_type*;
		using const_iterator = const value_type* ;
		using const_reverse_iterator = _reverse_iterator<const_iterator> ;
		using reverse_iterator = _reverse_iterator<iterator> ;
		
		using self = basic_string;
	private:

		allocator_type _alloc;
		pointer _p_str = nullptr;
		size_type _cap = 0;
		size_type _len = 0;

	private:
		basic_string(const_pointer p, size_type len) {
			_construct(p, len);
		}

	public:
		basic_string(const_pointer p){
			_construct(p,_length(p));
		}

		basic_string(const basic_string& s){
			_construct(s._p_str,s._len);
		}

		basic_string(basic_string&& s){
			_p_str = s._p_str;
			_len = s._len;
			_cap = s._cap;
			s._p_str = nullptr;
			s._len = s._cap = 0;
		}

		basic_string(std::initializer_list<value_type> s){
			_construct(s.begin(), s.size());
		}

		const self& operator=(const pointer p){
			_construct(p, _length(p));
			return *this;
		}

		const self& operator=(const self& s){
			_construct(s._p_str, s._len);
			return *this;
		}

		const self& operator=(const self&& s){
			_destruct();
			_p_str = s._p_str;
			_len = s._len;
			_cap = s._cap;
			s._p_str = nullptr;
			s._len = s._cap = 0;
			return *this;
		}

		const self& operator=(std::initializer_list<value_type> s){
			_construct(s.begin(), s.size());
			return *this;
		}

		~basic_string(){ _destruct(); }

		allocator_type get_allocator() const { return _alloc; }

		iterator begin(){ return _p_str; }
		reverse_iterator rbegin(){ return iterator(_p_str + _len - 1); }

		const_iterator cbegin(){ return begin(); }
		reverse_iterator crbegin(){ return rbegin(); }

		iterator end(){ return _p_str + _len; }
		const_reverse_iterator rend(){ return iterator(_p_str - 1); }

		const_iterator cend(){ return end(); }
		const_reverse_iterator crend(){ return rend(); }

		reference at(size_type n){ return n < _len?_p_str[n]:0; }
		const_reference at(size_type n) const { return n < _len ? _p_str[n] : 0; }
		
		reference operator[](size_type n){ return _p_str[n]; }
		const_reference operator[](size_type n) const { return _p_str[n]; }

		reference front(){ return *_p_str; }
		const_reference front() const { return *_p_str; }

		reference back(){ return _p_str[_len - 1]; }
		const_reference back() const { return _p_str[_len - 1]; }

		bool empty() const { return _len == 0; }

		size_type size() const { return _len; }
		size_type length() const { return _len; }

		pointer data() const { return _p_str; }

		pointer c_str() const { return _p_str; }

		//size_t find(const string& str, size_t pos = 0) const noexcept;

		//size_t find(const char* s, size_t pos = 0) const;

		//size_t find(const char* s, size_t pos, size_type n) const;


		size_t find(char c, size_t pos = 0) const noexcept {
			for (difference_type p = pos; p != _cap; ++p)
				if (c == this->_p_str[p])
					return p;
			return npos;
		}


		self& erase(size_t pos = 0, size_t len = npos) {
			if (pos > _len) return *this;
			if (len > _len - pos) len = _len - pos;
			auto _p_str_new = _alloc.allocate(_cap - len);
			for (difference_type p = 0; p != pos; ++p)
				_p_str_new[p] = _p_str[p];
			for (difference_type p = pos + len; p != _cap; ++p)
				_p_str_new[p - len] = _p_str[p];
			_p_str_new[_cap - len -1] = 0;
			_alloc.deallocate(_p_str, _cap);
			_cap -= len;
			_len -= len;
			_p_str = _p_str_new;
			return *this;
		}

		//iterator erase(const_iterator p);

		//iterator erase(const_iterator first, const_iterator last);

		const size_type max_size() const { return static_cast<size_type>(-1); }
		
		void reserve(size_type nsize){
			if (nsize + _len < _cap) return;
			auto new_cap = nsize + _len + 1;
			pointer new_string = _alloc.allocate(new_cap);
			if (_p_str){
				memcpy_s(new_string, new_cap, _p_str, sizeof(value_type)*_cap);
				_alloc.deallocate(_p_str, _cap);
			}
			_p_str = new_string;	//length don't have changed;
			_cap = new_cap;
		}

		size_t capacity(){ return _cap; }

		void resize(size_type nsize){
			auto new_len = _len < nsize ? _len : nsize;
			pointer new_string = _alloc.allocate(nsize + 1);
			if (_p_str){
				memcpy_s(new_string, nsize, _p_str, sizeof(value_type)*new_len);
				new_string[new_len] = 0;
				_alloc.deallocate(_p_str, _cap);
			}
			_p_str = new_string;
			_len = new_len;	//update length
			_cap = nsize + 1;
		}

		void clear(){
			_destruct();
		}

		void shrink_to_fit(){
			resize(_len);
		}

		self& append(const self& str){
			reserve(str._len);
			_copy(str._p_str, _p_str + _len, str._len);
			_len += str._len;
		}

		self& operator+=(const self& str){
			append(str);
		}

		void push_back(value_type c){
			reserve(1);
			_p_str[_len] = c;
			_p_str[++_len] = '\0';
		}

		self& assign(const self& s){
			_construct(s._p_str, s._len);
			return *this;
		}

		self& assign(const self& s, size_t npos, size_t nlen){
			_construct(s._p_str + npos, nlen);
			return *this;
		}

		self& assign(pointer p){
			_construct(p, _length(p));
			return *this;
		}

		self& assign(pointer p, size_t nlen){
			_construct(p, nlen);
			return *this;
		}

		self& assign(std::initializer_list<value_type> s){
			_construct(s.begin(), s.size());
			return *this;
		}

		template<typename InpurtIterator>
		void assign(InpurtIterator begin, InpurtIterator end){
			_construct(begin, end);
		}

		self substr(size_type start, size_type length) const {
			return self(_p_str + start, length);
		}

	private:
		void _construct(const_pointer p, size_type len){
			_destruct();
			_len = len;
			_cap = _len + 1;
			_p_str = _alloc.allocate(_cap);
			_copy(p, _p_str, len);
			_p_str[len] = 0;
		}

		template<typename InpurtIterator>
		void _construct(InpurtIterator begin, InpurtIterator end){
			_destruct();
			_len = distance(begin, end);
			_cap = _len + 1;
			_p_str = _alloc.allocate(_cap);
			_copy(begin, end, _p_str);
		}

		void _destruct(){
			if (_p_str){
				_alloc.deallocate(_p_str, _cap);
				_cap = _len = 0;
				_p_str = nullptr;
			}
		}

		static size_type _length(const_pointer p){
			size_type ret = 0;
			const_pointer b = p;
			while (*b++) ret++;
			return ret;
		}

		static void _copy(const_pointer p_Src, pointer p_Des, size_type nLen){
			do {*p_Des++ = *p_Src++;} while(--nLen);
			*p_Des = 0;
		}

		template<typename InputIterator>
		static void _copy(InputIterator begin, InputIterator end, pointer p_Des){
			do {*p_Des++ = *begin++; } while (begin != end);
			*p_Des = 0;
		}

		template<typename T>
		friend std::ostream& operator<<(std::ostream& os, basic_string<T>& str){return os<<str._p_str;}

		static const size_t npos = -1;
	};

	using string = basic_string<char>;
	using wstring = basic_string<wchar_t>;
	using u16string = basic_string<char16_t>;
	using u32string = basic_string<char32_t>;


}





#endif	//ZTL_STRING_HPP
