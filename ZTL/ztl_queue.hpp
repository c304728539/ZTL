#ifndef ZTL_QUEUE_H
#define ZTL_QUEUE_H

#include"ztl_deque.hpp"
#include"ztl_vector.hpp"

namespace ztl{

	template<typename T, typename Container = deque<T>>
	class queue{

		using value_type = T;
		using container_type = Container;
		using reference = typename container_type::reference;
		using const_reference = typename container_type::const_reference;
		using size_type = size_t;

	public:
		queue(){}
		queue(std::initializer_list<T> list) :_imp(list) {}
		~queue(){ _imp.clear(); }

	private:
		container_type _imp;

	public:
		void _clear(){ _imp.clear(); }

		//reference front() { return _imp[0]; }
		const_reference front() const { return _imp[0]; }

		//reference back() { return _imp[size()-1]; }
		const_reference back() const { return _imp[size()-1]; }

		void push(const value_type& x){ _imp.push_back(x); }

		value_type _pop(){ return _imp.pop_front(); }
		void pop() { _imp.pop_front(); }


		size_type size() const { return _imp.size(); }

		bool empty() const { return _imp.empty(); }

	};

	template<typename T, typename Container = ztl::vector<T, Allocator>, typename Compare = less<typename Container::value_type>>
	class priority_queue {
	public:
		using value_type = T;
		using container_type = Container;
		using reference = typename container_type::reference;
		using const_reference = typename container_type::const_reference;
		using size_type = typename container_type::size_type;

	private:
		container_type _imp;
	public:
		priority_queue(){}

		bool empty() {
			return 0 == size();
		}

		void _clear() {
			_imp.clear();
		}

		size_type size() {
			return _imp.size();
		}

		const_reference top() {
			return _imp[0];
		}

		void push(const value_type& x) {
			size_type i = _imp.size();
			_imp.resize(_imp.size() + 1);
			while (i != 0 && Compare()(_imp[(i - 1) / 2], x))
			{
				_imp[i] = _imp[(i - 1) / 2];
				i = (i - 1) / 2;
			}
			_imp[i] = x;
		}

		template <class... Args>
		void emplace(Args&&... args) {

		}


		void pop() {
			if (0 == size()) return;
			T y = move(_imp[size() - 1]);
			_imp.pop_back();
			size_type i = 0, ci = 1;
			while (ci < size())
			{
				if (ci + 1 < size() && Compare()(_imp[ci], _imp[ci + 1]))
					ci++;
				if (Compare()(y ,_imp[ci]))     
				{
					_imp[i] = _imp[ci];       
					i = ci;                
					ci = (ci<<1) + 1;
				}
				else            
					break;
			}
			_imp[i] = move(y);
		}

		void swap(priority_queue& r) {
			_imp.swap(r._imp);
		}
	};

}

#endif