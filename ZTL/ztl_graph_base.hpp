#ifndef ZTL_GRAPH_BASE_H
#define ZTL_GRAPH_BASE_H

#include"ztl_functional.hpp"
#include"ztl_set.hpp"
#include"ztl_map.hpp"
#include"ztl_vector.hpp"
#include"ztl_forward_list.hpp"
#include"ztl_queue.hpp"
#include"ztl_algorithm.hpp"
#include"ztl_fibonacci_heap.hpp"
#include<memory>
namespace ztl{

	enum __graph_node_color{ GCOLOR_WHITE, GCOLOR_GRAY, GCOLOR_BLACK };

	template<typename T>
	struct __graph_node_base{
		typedef T value_type;
		typedef size_t size_type;
		typedef size_type no_type;
		value_type _element;
		__graph_node_color _color;
		size_type _distance = MAX_SIZE_T;
		no_type _DFS_parent_traversal = MAX_SIZE_T;
		no_type _BFS_parent_traversal = MAX_SIZE_T;
		no_type _single_path_parent = MAX_SIZE_T;
		size_type _discover_time,_finish_time;

		 __graph_node_base(const value_type& x = value_type()) :_element(x){}

	};

	template<typename T, template<typename> class _node_type,
		template<typename, template<typename> class> class _graph_iterator_for,
		template<typename> class Alloc>
	class __graph_iterator_base{
		typedef __graph_iterator_base self;
	public:
		typedef T value_type;
		typedef size_t size_type;
		typedef _graph_iterator_for<T, Alloc> graph_type;
		typedef graph_type* graph_pointer;
		typedef typename graph_type::no_type no_type;
	private:
		size_type node_No;
		graph_pointer pGraph;

	public:
		__graph_iterator_base(no_type _No = MAX_SIZE_T,
			graph_pointer _pGraph = nullptr)
			:node_No(_No), pGraph(_pGraph){}

		no_type get_No(){
			return node_No;
		}

		value_type& operator*(){
			return pGraph->node_map[node_No]._element;
		}

		self& operator++(){
			if (node_No != pGraph->adjacency_list.size())
				++node_No;
			return *this;
		}

		self operator++(int){
			self ret = *this;
			++*this;
			return ret;
		}

		bool operator==(const self& rhs){ return node_No == rhs.node_No && pGraph == rhs.pGraph; }
		bool operator!=(const self& rhs){ return !(*this == rhs); }

		
	};

	//template<>
	struct __graph_edge_base{
		typedef size_t size_type;
		typedef size_type no_type;
		no_type node_No;

		__graph_edge_base(no_type x = MAX_SIZE_T) :node_No(x){}

		bool operator==(const __graph_edge_base& rhs){ return node_No == rhs.node_No; }
	};

	template<typename T,
		template<typename, template<typename> class> class _iterator_type,
		template<typename> class _node_type,
		typename _edge_type,
		typename _graph_type,
		template<typename> class Alloc>
	class __weighted_graph_base;

	template<typename T,
		template<typename, template<typename> class> class _iterator_type,
		template<typename> class _node_type,
		typename _edge_type,
		typename _graph_type,
		template<typename> class Alloc>
	class __graph_base{
	public:	
		//using iterator_type = _iterator_type<T,Alloc>;
		template<typename _T, template<typename> class _Alloc>
		friend class _iterator_type;
		
		friend class __weighted_graph_base<T, _iterator_type, _node_type, _edge_type, _graph_type, Alloc>;

	public:
		
		typedef T value_type;
		typedef value_type* pointer;
		typedef const value_type* const_pointer;
		typedef value_type& reference;
		typedef const value_type& const_reference;

		typedef _iterator_type<value_type,Alloc> iterator;
		typedef const iterator const_iterator;

		typedef size_t size_type;
		typedef size_type no_type;
		typedef forward_list<no_type> path_type;
		typedef _node_type<value_type> node_type;
		typedef pair<no_type, node_type> map_node;
		typedef map<no_type, node_type> node_container;
		typedef _edge_type edge_type;
		typedef forward_list<edge_type> edge_list;
		typedef edge_list* edge_list_pointer;
		typedef vector<edge_list_pointer> adjacency_list_type;
		typedef _graph_type graph_type;
		typedef graph_type* graph_pointer;
		
		
	private:
		bool DFS_valid = false;
		bool BFS_valid = false;

		Alloc<edge_list> _alloc_list;


	protected:
		size_type num_node = 0;
		node_container node_map;
		adjacency_list_type adjacency_list;
		edge_list_pointer allocate_list(){
			edge_list_pointer ret = _alloc_list.allocate(1);
			construct(ret);
			return ret;
		}

		void deallocate_list(edge_list_pointer& p){
			_alloc_list.deallocate(p, 1);
			p = nullptr;
		}

		void set_BFS_invalid(){ BFS_valid = false; }
		void set_DFS_invalid(){ DFS_valid = false; }

		bool check_no_invalid(no_type no){ return no >= adjacency_list.size(); }
	
		void DFS_visit(no_type u_no, size_t& time){
			node_map[u_no]._discover_time = ++time;
			node_map[u_no]._color = GCOLOR_GRAY;
			for (auto& edge_it : *adjacency_list[u_no]){
				no_type v_no = edge_it.node_No;
				if (node_map[v_no]._color == GCOLOR_WHITE){
					node_map[v_no]._DFS_parent_traversal = u_no;
					DFS_visit(v_no, time);
				}
			}
			node_map[u_no]._color = GCOLOR_BLACK;
			node_map[u_no]._finish_time = ++time;
		}
	public:
		__graph_base(){}
		__graph_base(const __graph_base& rhs) :node_map(rhs.node_map){
			
			adjacency_list.resize(rhs.node_map.size());
			for (size_type x = 0; x != adjacency_list.size(); ++x){
				adjacency_list[x] = allocate_list();
				*adjacency_list[x] = *rhs.adjacency_list[x];
			}
		}

		__graph_base(const node_container& map):node_map(map){
			adjacency_list.resize(map.size());
			for (auto& x : adjacency_list)
				x = allocate_list();
		}

		__graph_base& operator=(const __graph_base& rhs){
			clear();
			node_map = rhs.node_map;
			adjacency_list.resize(node_map.size());
			for (size_type x=0; x != adjacency_list.size();++x){
				adjacency_list[x] = allocate_list();
				*adjacency_list[x] = *rhs.adjacency_list[x];
			}
			return *this;
		}

		virtual ~__graph_base(){ clear(); }

		void clear(){
			for (auto& list_it : adjacency_list)
				deallocate_list(list_it);
			adjacency_list.clear();
			node_map.clear();
			set_BFS_invalid();
			set_DFS_invalid();
		}

		void BFS(no_type n){
			if (BFS_valid) return;
			if (!adjacency_list[n]) return;		//not exist
			for (no_type i = 0; i < adjacency_list.size(); ++i){
				if (i != n){
					auto& u = node_map[i];
					u._color = GCOLOR_WHITE;
					u._distance = MAX_SIZE_T;
					u._BFS_parent_traversal = MAX_SIZE_T;
				}
			}
			auto& s = node_map[n];
			s._color = GCOLOR_GRAY;
			s._distance = 0;
			s._BFS_parent_traversal = MAX_SIZE_T;
			queue<no_type> no_queue;
			no_queue.push(n);
			while (!no_queue.empty()){
				no_type u = no_queue._pop();
				for (auto& list_it : *adjacency_list[u]){
					no_type v = list_it.node_No;
					auto& noderef = node_map[v];
					if (noderef._color == GCOLOR_WHITE){
						noderef._color = GCOLOR_GRAY;
						noderef._distance = node_map[u]._distance + 1;
						noderef._BFS_parent_traversal = u;
						no_queue.push(v);
					}
				}
				node_map[u]._color = GCOLOR_BLACK;
			}
			BFS_valid = true;
		}

		void DFS(){
			if (DFS_valid) return;

			for (no_type v_no = 0; v_no != adjacency_list.size(); ++v_no){
				node_map[v_no]._color = GCOLOR_WHITE;
				node_map[v_no]._DFS_parent_traversal = MAX_SIZE_T;
			}
			size_t time = 0;
			for (no_type v_no = 0; v_no < adjacency_list.size(); ++v_no)
			if (node_map[v_no]._color == GCOLOR_WHITE)
				DFS_visit(v_no,time);
			DFS_valid = true;
		}

		iterator make_iterator_value(const value_type& value){
			for(auto& pair: node_map)
				if(pair.second._element == value)
					return iterator(pair.first, dynamic_cast<graph_pointer>(this));
			return iterator(MAX_SIZE_T, nullptr);
		}	

		iterator insert(const value_type& x){
			no_type new_no;
			new_no = adjacency_list.size();
			adjacency_list.push_back(allocate_list());
			node_map[new_no] = node_type(x);
			++num_node;
			return iterator(new_no, dynamic_cast<graph_pointer>(this));
		}

		void erase(iterator& x){
			no_type erase_no = x.node_No;
			size_type last = adjacency_list.size() - 1;
			if (erase_no > last) return;
			//size_type last = adjacency_list.size() - 1;
			node_map[erase_no] = node_map[last];
			auto it = node_map.maximum();
			node_map.erase(it);

			destory(adjacency_list[erase_no]);
			deallocate_list(adjacency_list[erase_no]);

			for(auto& plist :adjacency_list)
				for (auto it_list = plist->befor_begin(); it_list != plist->end(); ++it){
					auto it_next = it_list;
					++it_next;
					if (it_next->node_No == erase_no)
						plist->erase_after(it_list);
					else if (it_next->node_No == last)
						it_next->Node_No = erase_no;
				}
			--num_node;
		}

		pair<path_type, bool> path_DFS(no_type no_from, no_type no_to){
			path_type path_ret;
			DFS();
			path_ret.push_front(no_to);
			
			while (no_to != no_from){
				auto parent = node_map[no_to]._DFS_parent_traversal;
				if (parent != MAX_SIZE_T)
				{
					path_ret.push_front(parent);
					no_to = parent;
				}
				else
					return make_pair(path_ret, false);
			}
			return make_pair(path_ret, true);
		}


		pair<path_type, bool> path_BFS(no_type no_from, no_type no_to){
			path_type path_ret;
			BFS(no_from);
			path_ret.push_front(no_to);

			while (no_to != no_from){
				auto parent = node_map[no_to]._BFS_parent_traversal;
				if (parent != MAX_SIZE_T)
				{
					path_ret.push_front(parent);
					no_to = parent;
				}
				else
					return make_pair(path_ret, false);
			}
			return make_pair(path_ret, true);
		}

	};



	template<typename T>
	struct __weighted_graph_node_base :public __graph_node_base<T>{
	private:
		typedef  __graph_node_base<T> my_base_class;
	public:
		typedef typename my_base_class::no_type no_type;
		typedef typename my_base_class::value_type value_type;


		no_type _MST_parent_traversal = MAX_SIZE_T;

		__weighted_graph_node_base(const value_type& x = value_type()) :my_base_class(x){}

	};

	struct __weighted_graph_edge_base :public __graph_edge_base{
		int _weight;
		__weighted_graph_edge_base(){}
		__weighted_graph_edge_base(int weight, size_type node_no) :__graph_edge_base(node_no), _weight(weight){}

		bool operator==(const __weighted_graph_edge_base& rhs){ return node_No == rhs.node_No /*&& _weight == rhs._weight*/; }
		bool operator<(const __weighted_graph_edge_base& rhs){ return _weight < rhs._weight; }
		bool operator>(const __weighted_graph_edge_base& rhs){ return _weight > rhs._weight; }
	};


	template<typename T,
		template<typename, template<typename> class> class _iterator_type,
		template<typename> class _node_type,
		typename _edge_type,
		typename _graph_type,
		template<typename> class Alloc>
	class __weighted_graph_base : public __graph_base<T, _iterator_type, _node_type, _edge_type, _graph_type, Alloc>{
	public:
		using base = __graph_base<T, _iterator_type, _node_type, _edge_type, _graph_type, Alloc>;

		using size_type = typename base::size_type;
		using no_type = typename base::no_type;
		using iterator = typename base::iterator;
		using adjacency_list_type = typename base::adjacency_list_type;
		using node_container = typename base::node_container;
		using map_node = typename base::map_node;

		struct node_extract{
			_graph_type* pGraph;
			size_type operator()(const map_node& x) const {
				return x.second._distance;
			}
		};
	public:
		struct adjacency_matrix{

			typedef vector<vector<int>> double_vector;
			double_vector matrix;

			adjacency_matrix(size_type n){
				matrix.resize(n);
				for (auto& it : matrix)
					it.resize(n);
			}

			adjacency_matrix(const adjacency_list_type& adj){
				matrix.resize(adj.size());
				for (auto& it : matrix)
					it.resize(adj.size());
				for (auto& it_sub : matrix)
				for (auto& it : it_sub)
					it = INT_MAX/2;
				for (auto i = 0; i != matrix.size();++i)
					matrix[i][i] = 0;

				for (size_type i = 0; i < adj.size(); ++i)
				for (auto& edge_it : *adj[i])
					matrix[i][edge_it.node_No] = edge_it._weight;
			}

			~adjacency_matrix(){
				matrix.clear();
			}

			bool path_valid(iterator it_from, iterator it_to){
				return matrix[it_from.get_No][it_to.get_No] != INT_MAX / 2;
			}

			vector<int>& operator[](size_t n){ return matrix[n]; }
		};

	public:
		typedef typename base::const_iterator const_iterator;
		typedef typename base::node_type node_type;
		typedef typename base::edge_type edge_type;
		using path_type = typename base::path_type;
		using edge_list_pointer = typename base::edge_list_pointer;
	private:
		void initialize_single_source(no_type s){
			for (no_type no = 0; no != base::adjacency_list.size(); ++no){
				base::node_map[no]._distance = MAX_SIZE_T;
				base::node_map[no]._single_path_parent = MAX_SIZE_T;
			}
			base::node_map[s]._distance = 0;
		}

		void relax(no_type u,no_type v,int weight){
			if (base::node_map[v]._distance > base::node_map[u]._distance + weight){
				base::node_map[v]._distance = base::node_map[u]._distance + weight;
				base::node_map[v]._single_path_parent = u;
			}
		}

		
	public:
		//using my_graph_base::__graph_base;	//useless

		__weighted_graph_base(){}

		__weighted_graph_base(const __weighted_graph_base& rhs) :base(rhs){}

		__weighted_graph_base(const node_container& map) :base(map){}

		__weighted_graph_base& operator=(const __weighted_graph_base& rhs){ base::operator=(rhs); return *this; }
		
		~__weighted_graph_base(){}

		using base::insert;
		using base::erase;
		using base::clear;
		using base::DFS;
		using base::BFS;
		using base::path_DFS;
		using base::path_BFS;

		no_type get_node_MST_root(no_type x){
			no_type ret;
			do{
				ret = x;
				x = base::node_map[x]._MST_parent_traversal;
			} while (x != MAX_SIZE_T);
			return ret;
		}
		

		multimap<int, pair<iterator, iterator>> MST_kruskal(){
			multimap<int, pair<iterator, iterator>> ret;
			if (base::num_node == 0) return ret;
			set<no_type> vertex_set;
			multimap<int, pair<no_type, no_type>> edge_map;

			for (auto& node_it : base::node_map)
				node_it.second._MST_parent_traversal = MAX_SIZE_T;

			for (no_type v = 0; v != base::adjacency_list.size(); ++v)
			{
				if (base::adjacency_list[v]){
					vertex_set.insert(v);
					edge_list_pointer lst = base::adjacency_list[v];
					for (auto& edge_it : *lst)
						edge_map.insert(
							make_pair(edge_it._weight, 
								make_pair(v, edge_it.node_No)));
				}
			}

			for (auto& edge_it : edge_map){
				auto fir = get_node_MST_root(edge_it.second.first);
				auto sec = get_node_MST_root(edge_it.second.second);

				if (fir != sec){
					ret.insert(make_pair(edge_it.first,
						make_pair(iterator(edge_it.second.first, dynamic_cast<_graph_type*>(this)),
						iterator(edge_it.second.second, dynamic_cast<_graph_type*>(this)))));
					base::node_map[fir]._MST_parent_traversal = sec;
				}
			}

			return ret;
		}

		multimap<int, pair<iterator, iterator>> MST_prim(){
			multimap<int, pair<iterator, iterator>> ret;
			if (this->num_node == 0) {std::cout<<"numnode ==0! "<<std::endl; return ret;}
			set<no_type> vertex_set;

			multimap<int, pair<no_type, no_type>> edge_map;

			for (auto& node_it : base::node_map){
				node_it.second._MST_parent_traversal = MAX_SIZE_T;
				vertex_set.insert(node_it.first);
			}

			for (no_type v = 0; v < base::adjacency_list.size(); ++v)
			if (base::adjacency_list[v]){
				vertex_set.erase(v);
				for (auto& edge_it : *base::adjacency_list[v])
					edge_map.insert(
						make_pair(edge_it._weight,
						make_pair(v, edge_it.node_No)));
				break;
			}
			auto edge_it = edge_map.begin();
			while (!vertex_set.empty() && !edge_map.empty()){
				edge_it = edge_map.begin();
				while (vertex_set.find(edge_it->second.second) == vertex_set.end()){
					edge_map.erase(edge_it);
					edge_it = edge_map.begin();
				}
				vertex_set.erase(edge_it->second.second);
				ret.insert(make_pair(edge_it->first,
					make_pair(iterator(edge_it->second.first, dynamic_cast<_graph_type*>(this)),
					iterator(edge_it->second.second, dynamic_cast<_graph_type*>(this)))));
				for (auto& edge_ref : *base::adjacency_list[edge_it->second.second])
					edge_map.insert(
						make_pair(edge_ref._weight,
						make_pair(edge_it->second.second, edge_ref.node_No)));
				//node_map[edge_it->second.second]._MST_parent_traversal = edge_it->second.first;		//useless
				edge_map.erase(edge_it);

			}
			return ret;
		}
		
		path_type single_source_after(iterator destination_it){
			path_type ret;
			no_type destination_no = destination_it.get_No();
			ret.push_front(destination_no);
			while(this->node_map[destination_no]._single_path_parent != MAX_SIZE_T){
				ret.push_front(destination_no = this->node_map[destination_no]._single_path_parent);
			}
			if(0 == ret.size())
				return path_type();
			return ret;
		}

		bool single_source_bellmax_ford(iterator s_it){
			no_type s = s_it.get_No();
			initialize_single_source(s);
			for (no_type i = 1; i != base::node_map.size(); ++i){
				for (auto no = 0; no != base::adjacency_list.size();++no)
					if (base::adjacency_list[no])
					for (auto edge_it : *base::adjacency_list[no])
						relax(no, edge_it.node_No, edge_it._weight);
			}
			//check
			for (auto no = 0; no != base::adjacency_list.size(); ++no)
			if (base::adjacency_list[no])
			for (auto edge_it : *base::adjacency_list[no])
			if (base::node_map[no]._distance > base::node_map[edge_it.node_No]._distance + edge_it._weight)
				return false;
			return true;

		}

		void single_source_dijkstra(iterator s_it){
			no_type s = s_it.get_No();
			initialize_single_source(s);
			set<no_type> v_set;
			fibonacci_heap<map_node, size_type, node_extract> Q;
			for (auto& node_it : base::node_map)
				Q.insert(make_pair(node_it.first, node_it.second));

			while (!Q.empty()){
				auto res = Q.get_min();
				map_node u = res.first;
				Q.delete_min();
				v_set.insert(u.first);
				for (auto edge_it : *base::adjacency_list[u.first]){
					size_type dis = base::node_map[u.first]._distance + edge_it._weight;
					if (base::node_map[edge_it.node_No]._distance > dis){
						auto r = Q.begin();
						for (; r != Q.end();++r)
							if (r->first == edge_it.node_No) break;
						base::node_map[edge_it.node_No]._distance = dis;
						if (r != Q.end())
							Q.decrease_key(r, make_pair(edge_it.node_No, base::node_map[edge_it.node_No]));
						
						base::node_map[edge_it.node_No]._single_path_parent = u.first;
					}
				}
			}

		}

		std::shared_ptr<adjacency_matrix> floyd_warshall(){
			size_type n = base::adjacency_list.size();
			//adjacency_matrix matrix(adjacency_list);//, matrix_n(adjacency_list.size());
			std::shared_ptr<adjacency_matrix> matrix(new adjacency_matrix(base::adjacency_list));
			
			for (size_type k = 0; k != n; ++k/*, matrix = *matrix_n*/)
			for (size_type i = 0; i != n; ++i)
			for (size_type j = 0; j != n; ++j)
				(*matrix)[i][j] = min((*matrix)[i][j], (*matrix)[i][k] + (*matrix)[k][j]);
			return matrix;
		}
//#ifdef _DEBUG
		const node_container& get_node_map(){ return this->node_map; }
//#endif
		/*for network_flow*/
		size_type min_weight(path_type path){
			auto to = path.begin();
			auto from = to;
			++to;
			int ret_min=MAX_INT;
			for (; to != path.end(); ++to, ++from){
				auto r = base::adjacency_list[*from]->find({ 0,*to });
				ret_min = ret_min < r->_weight ? ret_min : r->_weight;
			}
			return static_cast<size_type>(ret_min);
		}
	};




	
}

#endif
