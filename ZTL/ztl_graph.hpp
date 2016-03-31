#ifndef ZTL_GRAPH_H
#define ZTL_GRAPH_H

#include"ztl_graph_base.hpp"

namespace ztl{
	template<typename T, template<typename> class Alloc>
	class network_flows;

	template<typename T, template<typename> class Alloc>
	class graph;

	template<typename T>
	using __graph_node = __graph_node_base<T>;

	using __graph_edge = __graph_edge_base;

	template<typename T, template<typename> class Alloc>
	using __graph_iterator = __graph_iterator_base<T, __graph_node, graph, Alloc>;

	template<typename T, template<typename> class Alloc = Allocator>
	class graph : public __graph_base<T, __graph_iterator, __graph_node, __graph_edge, graph<T,Alloc>, Alloc>{
	public:
		
		friend class network_flows<T, Alloc>;

		typedef __graph_base<T, __graph_iterator, __graph_node, __graph_edge, graph, Alloc> my_graph_base;
		using base = my_graph_base;
		typedef typename my_graph_base::iterator iterator;
		typedef typename my_graph_base::const_iterator const_iterator;
		typedef typename my_graph_base::node_type node_type;
		typedef typename my_graph_base::edge_type edge_type;

		using no_type = typename my_graph_base::no_type;
		using node_container = typename my_graph_base::node_container;

	private:

		void insert_edge(no_type no1, no_type no2){

			if (check_no_invalid(no1)
				|| check_no_invalid(no2)) return;
			auto plist = base::adjacency_list[no1];
			auto r = plist->find(no2);
			if (r == plist->end()) return;

			base::adjacency_list[no1]->push_front(no2);
			base::adjacency_list[no2]->push_front(no1);
		}
	public:

		graph(){}
		
		graph(const graph& rhs) :my_graph_base(rhs){}

		graph(const node_container& map) :my_graph_base(map){}

		graph& operator=(const graph& rhs){
			base::operator=(rhs);
			return *this;
		}

		virtual ~graph(){}

		/*graph common*/
		using my_graph_base::insert;
		using my_graph_base::erase;
		using my_graph_base::clear;
		using my_graph_base::DFS;
		using my_graph_base::BFS;
		using my_graph_base::path_DFS;
		using my_graph_base::path_BFS;

		void insert_edge(iterator it1, no_type it2){
			insert_edge(it1.get_No(), it2.get_No());
		}

		void erase_edge(iterator it1, iterator it2){
			no_type no1 = it1.get_No(), no2 = it2.get_No();
			if (check_no_invalid(no1)
				|| check_no_invalid(no2)) return;
			auto& plist = base::adjacency_list[no1];
			auto r = plist->find(no2);
			
			if (r != plist->end()){
				auto erase_it = base::adjacency_list[no1]->find_before(no2);
				base::adjacency_list[no1]->erase_after(erase_it);
				erase_it = base::adjacency_list[no2]->find_before(no1);
				base::adjacency_list[no2]->erase_after(erase_it);
			}
		}

	};

	template<typename T, template<typename> class Alloc>
	class directional_graph;

	template<typename T>
	using __directional_graph_node = __graph_node_base<T>;

	using __directional_graph_edge = __graph_edge_base;

	template<typename T, template<typename> class Alloc>
	using __directional_graph_iterator = __graph_iterator_base<T, __directional_graph_node, directional_graph, Alloc>;

	template<typename T, template<typename> class Alloc = Allocator>
	class directional_graph : public __graph_base<T, __directional_graph_iterator, __directional_graph_node, __directional_graph_edge, directional_graph<T, Alloc>, Alloc>{
	public:
		typedef __graph_base<T, __directional_graph_iterator, __directional_graph_node, __directional_graph_edge, directional_graph, Alloc> my_graph_base;
		using base = my_graph_base;
		typedef typename my_graph_base::iterator iterator;
		typedef typename my_graph_base::const_iterator const_iterator;
		typedef typename my_graph_base::node_type node_type;
		typedef typename my_graph_base::edge_type edge_type;

		using no_type = typename base::no_type;
		using node_container = typename base::node_container;
	public:

		directional_graph(){}

		directional_graph(const directional_graph& rhs) :my_graph_base(rhs){}

		directional_graph(const node_container& map) :my_graph_base(map){}

		directional_graph& operator=(const directional_graph& rhs){
			my_graph_base::operator=(rhs);
			return *this;
		}

		virtual ~directional_graph(){}

		void vir_func() override {}
		/*graph common*/
		using my_graph_base::insert;
		using my_graph_base::erase;
		using my_graph_base::clear;
		using my_graph_base::DFS;
		using my_graph_base::BFS;
		using my_graph_base::path_DFS;
		using my_graph_base::path_BFS;


		void insert_edge(iterator it_from, iterator it_to){
			no_type no_from = it_from.get_No(), no_to = it_to.get_No();
			if (check_no_invalid(no_from)
				|| check_no_invalid(no_to)) return;
			auto plist = base::adjacency_list[no_from];
			auto r = plist->find(no_to);
			if (r == plist->end()) return;

			base::adjacency_list[no_from]->push_front(no_to);
		}

		void erase_edge(iterator it_from, iterator it_to){
			no_type no_from = it_from.get_No(), no_to = it_to.get_No();
			if (check_no_invalid(no_from)
				|| check_no_invalid(no_to)) return;
			auto& plist = base::adjacency_list[no_from];
			auto r = plist->find(no_to);

			if (r != plist->end()){
				auto erase_it = base::adjacency_list[no_from]->find_before(no_to);
				base::adjacency_list[no_from]->erase_after(erase_it);
			}
		}
	};

	template<typename T, template<typename> class Alloc>
	class weighted_graph;

	template<typename T>
	using __weighted_graph_node = __weighted_graph_node_base<T>;

	using __weighted_graph_edge = __weighted_graph_edge_base;

	template<typename T, template<typename> class Alloc>
	using __weighted_graph_iterator = __graph_iterator_base<T, __weighted_graph_node, weighted_graph, Alloc>;

	template<typename T, template<typename> class Alloc = Allocator>
	class weighted_graph :public __weighted_graph_base<T, __weighted_graph_iterator, __weighted_graph_node, __weighted_graph_edge, weighted_graph<T, Alloc>, Alloc>
	{
	public:
		typedef __weighted_graph_base<T, __weighted_graph_iterator, __weighted_graph_node, __weighted_graph_edge, weighted_graph<T, Alloc>, Alloc> my_graph_base;
		using base = my_graph_base;
		typedef typename my_graph_base::iterator iterator;
		typedef typename my_graph_base::const_iterator const_iterator;
		typedef typename my_graph_base::node_type node_type;
		typedef typename my_graph_base::edge_type edge_type;
		using no_type = typename base::no_type;
		using node_container = typename base::node_container;

	public:
		weighted_graph(){}
		
		weighted_graph(const weighted_graph& rhs) :my_graph_base(rhs){}

		weighted_graph(const node_container& map) :my_graph_base(map){}
		
		weighted_graph& operator=(const weighted_graph& rhs){
			my_graph_base::operator=(rhs);
			return *this;
		}

		virtual ~weighted_graph(){}

	public:
		/*graph common*/
		using my_graph_base::insert;
		using my_graph_base::erase;
		using my_graph_base::clear;
		using my_graph_base::DFS;
		using my_graph_base::BFS;
		using my_graph_base::path_DFS;
		using my_graph_base::path_BFS;
		/*weighted_graph*/
		using my_graph_base::MST_kruskal;
		using my_graph_base::MST_prim;
		using my_graph_base::single_source_bellmax_ford;
		using my_graph_base::single_source_dijkstra;
		using my_graph_base::floyd_warshall;

		void insert_edge(iterator it1, iterator it2, int weight){
			no_type no1 = it1.get_No(), no2 = it2.get_No();
			if (base::check_no_invalid(no1)
				|| base::check_no_invalid(no2)) return;
			auto plist = base::adjacency_list[no1];
			auto r = find(plist->begin(), plist->end(), edge_type(weight, no2));
			if (r != plist->end()) return;

			base::adjacency_list[no1]->push_front({ weight, no2 });
			base::adjacency_list[no2]->push_front({ weight, no1 });
		}

		void erase_edge(iterator it1, iterator it2, int weight){
			no_type no1 = it1.get_No(), no2 = it2.get_No();
			if (check_no_invalid(no1)
				|| check_no_invalid(no2)) return;
			auto& plist = base::adjacency_list[no1];
			auto r = plist->find(no2);

			if (r != plist->end()){
				auto erase_it = base::adjacency_list[no1]->find_before(no2);
				base::adjacency_list[no1]->erase_after(erase_it);
				erase_it = base::adjacency_list[no2]->find_before(no1);
				base::adjacency_list[no2]->erase_after(erase_it);
			}
		}
	};

	template<typename T, template<typename> class Alloc>
	class weighted_directional_graph;

	template<typename T>
	using __weighted_directional_graph_node = __weighted_graph_node_base<T>;

	using __weighted_directional_graph_edge = __weighted_graph_edge_base;

	template<typename T, template<typename> class Alloc>
	using __weighted_directional_graph_iterator = __graph_iterator_base<T, __weighted_directional_graph_node, weighted_directional_graph, Alloc>;

	template<typename T, template<typename> class Alloc = Allocator>
	class weighted_directional_graph : public __weighted_graph_base<T, __weighted_directional_graph_iterator, __weighted_directional_graph_node, __weighted_directional_graph_edge, weighted_directional_graph<T, Alloc>, Alloc>{
	public:
		typedef __weighted_graph_base<T, __weighted_directional_graph_iterator, __weighted_directional_graph_node, __weighted_directional_graph_edge, weighted_directional_graph, Alloc> my_graph_base;
		using base = my_graph_base;
		typedef typename my_graph_base::iterator iterator;
		typedef typename my_graph_base::const_iterator const_iterator;
		typedef typename my_graph_base::node_type node_type;
		typedef typename my_graph_base::edge_type edge_type;
		using no_type = typename base::no_type;
		using node_container= typename base::node_container;
	public:

		weighted_directional_graph(){}

		weighted_directional_graph(const weighted_directional_graph& rhs) :my_graph_base(rhs){}

		weighted_directional_graph(const node_container& map) :my_graph_base(map){}

		virtual ~weighted_directional_graph(){}

		weighted_directional_graph& operator=(const weighted_directional_graph& rhs){
			my_graph_base::operator=(rhs);
			return *this;
		}

		/*graph common*/
		using my_graph_base::insert;
		using my_graph_base::erase;
		using my_graph_base::clear;
		using my_graph_base::DFS;
		using my_graph_base::BFS;
		using my_graph_base::path_DFS;
		using my_graph_base::path_BFS;
		/*weighted_graph*/
		using my_graph_base::MST_kruskal;
		using my_graph_base::MST_prim;
		using my_graph_base::single_source_bellmax_ford;
		using my_graph_base::single_source_dijkstra;
		using my_graph_base::floyd_warshall;



		void insert_edge(iterator it_from, iterator it_to, int weight){
			no_type no_from = it_from.get_No(), no_to = it_to.get_No();
			if (base::check_no_invalid(no_from)
				|| base::check_no_invalid(no_to)) return;
			auto plist = base::adjacency_list[no_from];
			auto r = find(plist->begin(), plist->end(), edge_type(weight, no_to));
			if (r != plist->end()) return;

			base::adjacency_list[no_from]->push_front({ weight, no_to });
		}

		void erase_edge(iterator it_from, iterator it_to, int weight){
			no_type no_from = it_from.get_No(), no_to = it_to.get_No();
			if (base::check_no_invalid(no_from)
				|| base::check_no_invalid(no_to)) return;
			auto& plist = base::adjacency_list[no_from];
			auto r = plist->find(no_to);

			if (r != plist->end()){
				auto erase_it = base::adjacency_list[no_from]->find_before(no_to);
				base::adjacency_list[no_from]->erase_after(erase_it);
			}
		}
	};



}

#endif
