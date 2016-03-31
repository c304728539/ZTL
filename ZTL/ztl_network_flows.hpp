#ifndef ZTL_NETWORK_FLOW
#define ZTL_NETWORK_FLOW

#include"ztl_graph.hpp"

namespace ztl{

	template<typename T>
	using __network_flows_node = __weighted_graph_node_base<T>;

	
	struct __network_flows_edge :public __graph_edge_base{
		size_type c_volume;
		size_type f_stream =0;

		__network_flows_edge(size_type volume = 0, no_type no = MAX_SIZE_T) :__graph_edge_base(no), c_volume(volume){}
	
	//	bool operator==(const __network_flows_edge& rhs){
	//		return __graph_edge_base::operator==(rhs);
	//	}
	
	};

	template<typename T, template<typename> class Alloc>
	using __network_flows_iterator = __graph_iterator_base<T, __network_flows_node, network_flows, Alloc>;

	template<typename T, template<typename> class Alloc = Allocator>
	class network_flows
		:public __graph_base<T, __network_flows_iterator, __network_flows_node, __network_flows_edge, network_flows<T, Alloc>, Alloc>{
		using base = __graph_base<T, __network_flows_iterator, __network_flows_node, __network_flows_edge, network_flows<T, Alloc>, Alloc>;
	public:
		using value_type = typename base::value_type;
		using iterator = typename base::iterator;
		using const_iterator = typename base::const_iterator;
		using node_type = typename base::node_type;
		using edge_type = typename base::edge_type;
		using no_type = typename base::no_type;
		using size_type = typename base::size_type;
		using path_type = typename base::path_type;
	private:
		typedef weighted_directional_graph<T, Alloc> residual_graph_type;
	public:
		network_flows(){}
		virtual ~network_flows(){}

		using base::insert;
		using base::erase;
		using base::clear;


	private:
		no_type s_node = MAX_SIZE_T, t_node = MAX_SIZE_T;
	public:
		void set_s_node(iterator it_s){ if (!check_no_invalid(it_s.get_No())) s_node = it_s.get_No(); }

		void set_t_node(iterator it_t){ if (!check_no_invalid(it_t.get_No())) t_node = it_t.get_No(); }


		residual_graph_type make_residual_graph(){
			residual_graph_type ret(this->node_map);
			for (size_type list_no = 0; list_no != this->adjacency_list.size(); ++list_no)
			for (auto& edge_it : *this->adjacency_list[list_no]){
				if (edge_it.f_stream)
					ret.insert_edge(edge_it.node_No, list_no, edge_it.f_stream);
				size_type left = edge_it.c_volume - edge_it.f_stream;
				if (left)
					ret.insert_edge(list_no, edge_it.node_No, left);
			}
			return move(ret);
		}

		size_type get_stream() const {
			size_type ret_stream = 0;
			for (auto edge_it : *this->adjacency_list[s_node])
				ret_stream += edge_it.f_stream;
			return ret_stream;
		}

		bool stream_init(){
			if (check_no_invalid(s_node)
				|| check_no_invalid(t_node)) return false;

			for (auto list_it : this->adjacency_list)
			for (auto& edge_list : *list_it)
				edge_list.f_stream = 0;

			return true;
		}

		void add_stream(const path_type& path, size_type stream_add){
			auto to = path.cbegin();
			auto from = to++;
			for (; to != path.cend(); ++to, ++from){
				auto r = this->adjacency_list[*from]->find({ 0, *to });
				if (r != this->adjacency_list[*from]->end())
					r->f_stream += stream_add;
				else{
					r = this->adjacency_list[*to]->find({ 0, *from });
					r->f_stream -= stream_add;
				}
			}
		}

		pair<size_type,bool> ford_fulkerson(){
			if (!stream_init())
				return make_pair<size_type, bool>(0, false);


			residual_graph_type residual_graph = make_residual_graph();
			pair<path_type, bool> path = residual_graph.path_DFS(s_node, t_node);
			for (; path.second; residual_graph = make_residual_graph(), path = residual_graph.path_DFS(s_node, t_node)){
				size_type stream_add = residual_graph.min_weight(path.first);
				add_stream(path.first, stream_add);
			}
			return make_pair(get_stream(), true);
		}

		pair<size_type, bool> edmonds_karp(){
			if (!stream_init())
				return make_pair<size_type, bool>(0, false);

			residual_graph_type residual_graph = make_residual_graph();
			pair<path_type, bool> path = residual_graph.path_BFS(s_node, t_node);
			for (; path.second; residual_graph = make_residual_graph(), path = residual_graph.path_BFS(s_node, t_node)){
				size_type stream_add = residual_graph.min_weight(path.first);
				add_stream(path.first, stream_add);
			}
			return make_pair(get_stream(), true);
		}

		void insert_edge(iterator it_from, iterator it_to, size_type volume){
			no_type no_from = it_from.get_No(), no_to = it_to.get_No();
			if (check_no_invalid(no_from)
				|| check_no_invalid(no_to)) return;
			auto plist = this->adjacency_list[no_from];
			auto r = plist->find(no_to);
			if (r != plist->end()) return;

			this->adjacency_list[no_from]->push_front({ volume, no_to });
		}

		void erase_edge(iterator it_from, iterator it_to, int weight){
			no_type no_from = it_from.get_No(), no_to = it_to.get_No();
			if (check_no_invalid(no_from)
				|| check_no_invalid(no_to)) return;
			auto& plist = this->adjacency_list[no_from];
			auto r = plist->find(no_to);

			if (r != plist->end()){
				auto erase_it = this->adjacency_list[no_from]->find_before(no_to);
				this->adjacency_list[no_from]->erase_after(erase_it);
			}
		}


	};

}
#endif
