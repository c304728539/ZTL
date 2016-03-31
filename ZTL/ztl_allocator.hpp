#ifndef ZTL_ALLOCATOR_HPP
#define ZTL_ALLOCATOR_HPP

#include"ztl_base.hpp"
#include"ztl_template.hpp"
#include"ztl_algorithm.hpp"
//#include<xmemory0>
//#include<Windows.h>
//#include<atomic>
#include<mutex>

namespace ztl{

#define _VOLATILE volatile

	class malloc_alloc{
		typedef void(*malloc_handler_func)();
	private:
		static void* e_malloc(size_t n);
		static void* e_realloc(void* p, size_t n);

		static malloc_handler_func _malloc_handler;
	public:


		static void* allocate(size_t n){
			void* ret = operator new(n);		//malloc(n);
			if (0 == ret) e_malloc(n);
			return ret;
		}

		static void deallocate(void* p, size_t n){
			operator delete(p,n);
		}

		static void deallocate(void* p){
			operator delete(p);
		}

		static void* reallocate(void* p, size_t n_old, size_t n_new){
			void* ret = realloc(p, n_new);
			if (0 == ret) e_realloc(p, n_new);
			return ret;
		}

		static malloc_handler_func set_malloc_handler(malloc_handler_func f)
		{
			malloc_handler_func old_handler = _malloc_handler;
			_malloc_handler = f;
			return old_handler;
		}

		static void* allocate_aligned(size_t size, size_t align /*= 8*/)
		{
			void* raw_malloc_ptr;		
			void* aligned_ptr;		//will be returned

			if (align & (align - 1))	//return if align is not a number power of 2 
			{
				errno = EINVAL;
				return nullptr;
			}

			if (0 == size)
			{
				return nullptr;
			}

			if (align < 2 * sizeof(void*))
			{
				align = 2 * sizeof(void*);
			}

			raw_malloc_ptr = allocate(size + align);
			if (!raw_malloc_ptr)
			{
				return nullptr;
			}

			//Align  We have at least sizeof (void *) space below malloc'd ptr. 
			aligned_ptr = reinterpret_cast<void*>((reinterpret_cast<size_t>(raw_malloc_ptr) + align) & ~(static_cast<size_t>(align) - 1));

			(reinterpret_cast<void**>(aligned_ptr))[-1] = raw_malloc_ptr;

			return aligned_ptr;
		}

		static void deallocate_aligned(void * aligned_ptr)
		{
			if (aligned_ptr)
			{
				deallocate((reinterpret_cast<void**>(aligned_ptr)[-1]));
			}
		}

		static bool isAligned(void* data, int alignment)
		{
			//<Hacker's Delight>  
			return ((uintptr_t)data & (alignment - 1)) == 0;
		}

	};

	malloc_alloc::malloc_handler_func malloc_alloc::_malloc_handler = 0;

	void* malloc_alloc::e_malloc(size_t n){
		void* ret;
		while (1){
			if (0 == _malloc_handler) throw std::bad_alloc();
			_malloc_handler();
			ret = operator new(n);		//malloc(n);			/*dangerous£¡*/
			if (ret) return ret;
		}
	}

	void* malloc_alloc::e_realloc(void* p, size_t n){
		void* ret;
		while (1){
			if (0 == _malloc_handler) throw std::bad_alloc();
			_malloc_handler();
			ret = realloc(p, n);						/*dangerous£¡*/
			if (ret) return ret;
		}
	}


	template<bool _thread, int _nint>
	class alloc{
		enum{ _ALIGN = 8 };
		enum{ _MAX_BYTES = 128 };
		enum{ _FREELISTNUM = _MAX_BYTES / _ALIGN };
	private:
		static std::mutex Mutex;

		static size_t CEIL(const size_t _nbyte){
			return (((_nbyte)+_ALIGN - 1)&~(_ALIGN - 1));
		}

		union __block
		{
			__block* _free_list_link;
			char data[1];
		};

		static __block* _VOLATILE _free_list[_FREELISTNUM];

		static size_t _freelist_index(const size_t _nbyte){
			return (CEIL(_nbyte)>>3) - 1;
		}

		static void* refill(size_t n);
		static char* chunk_alloc(size_t _nbyte, size_t& nblocks);
		static char* start_alloc;
		static char* end_alloc;

	public:
		static void* allocate(size_t n);
		static void deallocate(void* p, size_t n);
		static void* reallocate(void* p, size_t n_old, size_t n_new);
	private:
		static void* _allocate_aux(size_t n, __true_type);
		static void* _allocate_aux(size_t n, __false_type);
		static void _deallocate_aux(void* p, size_t n, __true_type);
		static void _deallocate_aux(void* p, size_t n, __false_type);
	};
	template<bool _thread, int _nint>
	std::mutex alloc<_thread, _nint>::Mutex;
	template<bool _thread, int _nint>
	char* alloc<_thread, _nint>::start_alloc = 0;

	template<bool _thread, int _nint>
	char* alloc<_thread, _nint>::end_alloc = 0;

	template<bool _thread, int _nint>
	typename alloc<_thread, _nint>::__block* _VOLATILE alloc<_thread, _nint>::_free_list[_FREELISTNUM]
		= { 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, };

	template<bool _thread, int _nint>
	inline void* alloc<_thread, _nint>::allocate(size_t n)
	{
		typedef typename IfThenElse<_thread, __true_type, __false_type>::Result R;
		return _allocate_aux(n, R());
	}

	template<bool _thread, int _nint>
	void* alloc<_thread, _nint>::_allocate_aux(size_t n, __true_type)
	{
		Mutex.lock();
		void* ret=_allocate_aux(n, __false_type());
		Mutex.unlock();
		return ret;
	}


	template<bool _thread, int _nint>
	void* alloc<_thread, _nint>::_allocate_aux(size_t n, __false_type)
	{
		__block* _VOLATILE * my_free_list;
		__block* ret;
		if (n > _MAX_BYTES) return malloc_alloc::allocate(n);
		my_free_list = _free_list + _freelist_index(n);
		ret = *my_free_list;
		if (0 == ret) return refill(CEIL(n));
		*my_free_list = ret->_free_list_link;
		return ret;
	}

	template<bool _thread, int _nint>
	void alloc<_thread, _nint>::deallocate(void* p, size_t n){
		typedef typename IfThenElse<_thread, __true_type, __false_type>::Result R;
		_deallocate_aux(p, n, R());
	}
	template<bool _thread, int _nint>
	void alloc<_thread, _nint>::_deallocate_aux(void* p, size_t n, __true_type) {
		Mutex.lock();
		_deallocate_aux(p, n, __false_type());
		Mutex.unlock();
	}
	
	template<bool _thread, int _nint>
	void alloc<_thread, _nint>::_deallocate_aux(void* p, size_t n, __false_type) {
		if (p == 0) return;
		__block* _VOLATILE * my_free_list;
		__block* b = (__block*)p;
		if (n > _MAX_BYTES) return malloc_alloc::deallocate(p, n);
		my_free_list = _free_list + _freelist_index(n);
		b->_free_list_link = *my_free_list;
		*my_free_list = b;
	}


	template<bool _thread, int _nint>
	void* alloc<_thread, _nint>::reallocate(void* p, size_t n_old, size_t n_new) {
		void* ret = allocate(n_new);
		copy<char*,char*>((char*)p, (char*)(p + ztl_min(n_old,n_new)), (char*)ret);
		deallocate(p, n_old);
		return ret;
	}


	template<bool _thread, int _nint>
	void* alloc<_thread, _nint>::refill(size_t n){
		size_t nblock = 20;
		char* chunk = chunk_alloc(n, nblock);
		__block* _VOLATILE* my_free_list;
		__block* ret;
		__block* current_block, *next_block;
		size_t i;
		if (1 == nblock) return chunk;
		my_free_list = _free_list + _freelist_index(n);		//CEIL repeat!
		ret = (__block*)chunk;
		*my_free_list = next_block = (__block*)(chunk + n);
		for (i = 1;; i++){
			current_block = next_block;
			next_block = (__block*)((char*)next_block + n);
			if (nblock - 1 == i){
				current_block->_free_list_link = 0;
				break;
			}
			else
				current_block->_free_list_link = next_block;
		}
		return ret;
	}

	template<bool _thread, int _nint>
	char* alloc<_thread, _nint>::chunk_alloc(size_t _nbyte, size_t& nblock){
		char* ret;
		size_t total_bytes = _nbyte*nblock;
		
		size_t bytes_left = end_alloc - start_alloc;
		if (total_bytes <= bytes_left){
			ret = start_alloc;
			start_alloc += total_bytes;
			return ret;
		}
		else if (bytes_left>=_nbyte){
			nblock = bytes_left / _nbyte;
			total_bytes = _nbyte*nblock;
			ret = start_alloc;
			start_alloc += total_bytes;

			return ret;
		}
		else{
			size_t bytes_to_get = 2 * total_bytes + 256;
			if (bytes_left > 0){
				__block* _VOLATILE * my_free_list = _free_list + _freelist_index(bytes_left);
				((__block*)start_alloc)->_free_list_link = *my_free_list;
				*my_free_list = (__block*)start_alloc;
			}
			start_alloc = reinterpret_cast<char*>(malloc_alloc::allocate_aligned(bytes_to_get, _ALIGN)); // malloc_alloc::allocate(bytes_to_get));		//(char*)malloc(bytes_to_get);
			if (0 == start_alloc){
				__block* _VOLATILE * my_free_list, *p;
				for (auto i = _nbyte; i <= _MAX_BYTES; i += _ALIGN){
					my_free_list = _free_list + _freelist_index(i);
					p = *my_free_list;
					if (p!=0){
						*my_free_list = p->_free_list_link;
						start_alloc = reinterpret_cast<char*>(p);
						end_alloc = start_alloc + i;
						return chunk_alloc(_nbyte, nblock);
					}
				}
				end_alloc = 0;
				start_alloc = reinterpret_cast<char*>(malloc_alloc::allocate_aligned(bytes_to_get, _ALIGN));  //malloc_alloc::allocate(bytes_to_get));
			}
			end_alloc = start_alloc + bytes_to_get;
			return (chunk_alloc(_nbyte, nblock));
		}
	}

	template<bool thread, int nint>
	class alloc_z{
		enum { _ALIGN = 8 };
		enum { _MAX_BYTES = 128 };
		enum { _FREELISTNUM = _MAX_BYTES / _ALIGN };

		static size_t CEIL(const size_t _nbyte) {
			return (((_nbyte)+_ALIGN - 1)&~(_ALIGN - 1));
		}

		static size_t _freelist_index(const size_t _nbyte) {
			return (CEIL(_nbyte) >> 3) - 1;
		}

		union __block
		{
			__block* _free_list_link;
			char data[1];
		};

		class Chunk {
			static char* start_alloc;
			static char* end_alloc;
			static std::mutex mutex_chunk;
		public:
			Chunk() = default;
			Chunk(const Chunk&) = delete;

			void* alloc(size_t _nbyte, size_t& nblock) {
				typedef typename IfThenElse<thread, __true_type, __false_type>::Result R;
				return _alloc_aux(_nbyte, nblock, R());
			}

		private:
			void* _alloc_aux(size_t _nbyte, size_t& nblock, __true_type) {
				mutex_chunk.lock();
				void* ret = _alloc_aux(_nbyte, nblock, __false_type());
				mutex_chunk.unlock();
				return ret;
			}

			void* _alloc_aux(size_t _nbyte, size_t& nblock,__false_type) {
				char* ret;
				size_t total_bytes = _nbyte*nblock;
				size_t bytes_left = end_alloc - start_alloc;
				if (total_bytes <= bytes_left) {
					ret = start_alloc;
					start_alloc += total_bytes;
					return ret;
				}
				else if (bytes_left >= _nbyte) {
					nblock = bytes_left / _nbyte;
					total_bytes = _nbyte*nblock;
					ret = start_alloc;
					start_alloc += total_bytes;
					return ret;
				}
				else {
					size_t bytes_to_get = 2 * total_bytes + 256;
					if (bytes_left > 0) {
						alloc_z<thread, nint>::lists[_freelist_index(bytes_left)]->_deallocate_aux(start_alloc,__false_type());
					}
					start_alloc = reinterpret_cast<char*>(malloc_alloc::allocate_aligned(bytes_to_get, _ALIGN));
					if (0 == start_alloc) {
						for (auto i = _nbyte; i <= _MAX_BYTES; i += _ALIGN) {
							if (!alloc_z<thread, nint>::lists[_freelist_index(i)]->empty()) {
								start_alloc = reinterpret_cast<char*>(alloc_z<thread, nint>::lists[_freelist_index(i)]->allocate());
								end_alloc = start_alloc + i;
								return _alloc_aux(_nbyte, nblock, __false_type());
							}
						}
						end_alloc = 0;
						start_alloc = reinterpret_cast<char*>(malloc_alloc::allocate_aligned(bytes_to_get, _ALIGN));  //malloc_alloc::allocate(bytes_to_get));
					}
					end_alloc = start_alloc + bytes_to_get;
					return (_alloc_aux(_nbyte, nblock, __false_type()));
				}
			}
		};

		class free_list_base {
			friend class Chunk;
		public:
			void* allocate() {
				typedef typename IfThenElse<thread, __true_type, __false_type>::Result R;
				return _allocate_aux(R());
			}

			void deallocate(void* p) {
				typedef typename IfThenElse<thread, __true_type, __false_type>::Result R;
				_deallocate_aux(p, R());
			}

		protected:
			virtual void* _allocate_aux(__true_type) = 0;
			virtual void* _allocate_aux(__false_type) = 0;
			virtual void _deallocate_aux(void* p, __true_type) = 0;
			virtual void _deallocate_aux(void* p, __false_type) = 0;

		public:
			bool empty() const {
				return _list == nullptr;
			}

		public:
			static Chunk  _chunk;

		protected:
			__block* _list = nullptr;

			void* refill(size_t n){
				size_t nblock = 20;
				char* chunk = reinterpret_cast<char*>(_chunk.alloc(n, nblock));
				//__block*& my_free_list = _list;
				__block* ret;
				__block* current_block, *next_block;
				if (1 == nblock) return chunk; 
				ret = (__block*)chunk;
				next_block = (__block*)(chunk + n);
				if (_list != nullptr)
					_list->_free_list_link = next_block;
				else
					_list = next_block;
				for (int i = 1;; i++) {
					current_block = next_block;
					next_block = (__block*)((char*)next_block + n);
					if (nblock - 1 == i) {
						current_block->_free_list_link = nullptr;
						break;
					}
					else
						current_block->_free_list_link = next_block;
				}
				return ret;
			}

		};

		template<int n>
		class free_list :public free_list_base {
			using base = free_list_base;
		public:
			free_list() = default;
			free_list(const free_list&) = delete;
		private:
			static std::mutex Mutex;

		protected:
			virtual void* _allocate_aux(__true_type) override {
				Mutex.lock();
				void* ret = _allocate_aux(__false_type());
				Mutex.unlock();
				return ret;
			}

			virtual void* _allocate_aux(__false_type) override {
				__block* ret;
				ret = base::_list;
				if (nullptr == ret)
					return base::refill(n);
				base::_list = ret->_free_list_link;
				return ret;
			}
 
			virtual void _deallocate_aux(void* p, __true_type) override {
				Mutex.lock();
				_deallocate_aux(p, __false_type());
				Mutex.unlock();
			}

			virtual void _deallocate_aux(void* p, __false_type) override {
				static std::mutex mutex_dealloc;
				if (p == 0) return;
				__block* b = (__block*)p;
				mutex_dealloc.lock();
 				b->_free_list_link = base::_list;
				base::_list = b;
				mutex_dealloc.unlock();
			}
		};
	
		class free_lists{
			template<size_t n, size_t... idxs>
			void _init_list() {
				lists[n] = new free_list<(n + 1) * 8>();
				_init_list<idxs...>();
			}

			template<typename T = void>
			void _init_list(){
			}
		public:
			template<size_t... idxs>
			free_lists(_idx<idxs...>) {
				_init_list<idxs...>();
			}

			free_lists(const free_lists&) = delete;

			free_list_base* const operator[](size_t n) const { return lists[n]; }

			static free_list_base* lists[16];
		};

		
	public:
		static void* allocate(size_t n){
			if (n > _MAX_BYTES) return malloc_alloc::allocate(n);
			return lists[_freelist_index(n)]->allocate();
		}

		static void deallocate(void* p, size_t n) {
			if (n > _MAX_BYTES) 
				malloc_alloc::deallocate(p, n);
			else
				lists[_freelist_index(n)]->deallocate(p);
		}
	private:
		static free_lists lists;
	};

	template<bool thread, int nint>
	typename alloc_z<thread, nint>::free_lists alloc_z<thread, nint>::lists = make_idx_seq<0, 15, 1>::result();//_idx<0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15>();
	
	template<bool thread, int nint>
	typename alloc_z<thread, nint>::free_list_base* alloc_z<thread, nint>::free_lists::lists[16] = {nullptr};

	template<bool thread, int nint>
	typename alloc_z<thread, nint>::Chunk alloc_z<thread, nint>::free_list_base::_chunk;

	template<bool thread, int nint>
	template<int n>
	std::mutex alloc_z<thread, nint>::free_list<n>::Mutex;
	
	template<bool thread, int nint>
	std::mutex alloc_z<thread, nint>::Chunk::mutex_chunk;

	template<bool thread, int nint>
	char* alloc_z<thread, nint>::Chunk::start_alloc = nullptr;
	
	template<bool thread, int nint>
	char* alloc_z<thread, nint>::Chunk::end_alloc = nullptr;

	template<bool thread, typename T, template<bool,int> class alloc_which>
	class __Allocator_aux{
		typedef alloc_which<thread, 0> default_alloc;
	public:
		typedef T value_type;
		typedef size_t size_type;
		typedef value_type *pointer;
		typedef const value_type *const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;
		typedef ptrdiff_t difference_type;

		template<class _Other>
		struct rebind
		{	
			typedef __Allocator_aux<thread,_Other, alloc_which> other;
		};

	public:
		__Allocator_aux() = default;

		template<class U>
		__Allocator_aux(const __Allocator_aux<thread,U, alloc_which>&)
		{}

 		pointer allocate(size_type _Count){
			return static_cast<pointer>(default_alloc::allocate(sizeof(T)*_Count));
		}

		void deallocate(pointer p, size_type _Count){
			default_alloc::deallocate(p, sizeof(T)*_Count);
		}

		pointer address(reference _Val) const {
			return (&_Val);
		}

		const_pointer address(const_reference _Val) const {
			return (&_Val);
		}

		void construct(pointer p, const_reference x){
			::new ((void *)p) T(x);
		}

		template<class _Objty, class... _Types>
		void construct(_Objty *_Ptr, _Types&&... _Args)
		{	// construct _Objty(_Types...) at _Ptr
			::new ((void *)_Ptr) _Objty(ztl::forward<_Types>(_Args)...);
		}//À´×Ôxmemory0
		
		void destroy(pointer p)
		{
			p->~T();
		}

		size_t max_size() const {
			return ((size_t)(-1) / sizeof (T));
		}
	};


	template<typename T>
	using Allocator = __Allocator_aux<false, T, alloc>;

	template<typename T>
	using Allocator_thread = __Allocator_aux<true, T, alloc>;

	template<typename T>
	using Allocator_z = __Allocator_aux<false, T, alloc_z>;

	template<typename T>
	using Allocator_z_thread = __Allocator_aux<true, T, alloc_z>;


}

#endif	//ZTL_ALLOCATOR_HPP
