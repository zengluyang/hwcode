
#include "route.h"
#include "lib_record.h"
#include <stdio.h>

#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <algorithm>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>

using namespace boost;
using namespace std;

void test_boost();

//你要完成的功能总入口
#define printf
    
typedef adjacency_list < vecS, vecS, directedS,
  no_property, property < edge_weight_t, int > > graph_t;
typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
typedef std::pair<int, int> Edge;

void search_route(char *topo[5000], int edge_num, char *demand)
{
	//test_boost();
    unsigned short result[] = {2, 6, 3};//示例中的一个解

    //------------1 parse input-------------------------
   	std::vector<Edge> edge_vector;
	edge_vector.reserve(edge_num);
   	std::vector<int> weight_vecotr;
	edge_vector.reserve(edge_num);
   	int biggest_node_id=0;
    for(int i=0,linkd_id=0,src_id=0,dest_id=0,cost=0;i<edge_num;i++) {
    	sscanf(topo[i],"%d,%d,%d,%d",&linkd_id,&src_id,&dest_id,&cost);
    	edge_vector.push_back(Edge(src_id,dest_id));
    	weight_vecotr.push_back(cost);
    	if(src_id>biggest_node_id) {
    		biggest_node_id=src_id;
    	}
    	if(dest_id>biggest_node_id) {
    		biggest_node_id=dest_id;
    	}
    	printf("%d,%d,%d,%d\n",linkd_id,src_id,dest_id,cost);    	
    }
    int demand_src_id=0,demand_dst_id=0;
    char must_visit_nodes_buf[128];
    sscanf(demand,"%d,%d",&demand_src_id,&demand_dst_id);
    printf("demand_src_id:%d, demand_dst_id:%d\n",demand_src_id,demand_dst_id);
	int must_visit_nodes[64];
	int must_visit_node_cnt=0;
	int curent_must_visit_node = 0;
	char* split=demand;
	char* token;
	token=strsep(&split,",");
	token=strsep(&split,",");
	token=strsep(&split,",");
	//printf(token);
	split=token;
	while(split!=NULL) {
		token=strsep(&split,"|");
		curent_must_visit_node=atoi(token);
		must_visit_nodes[must_visit_node_cnt++] = curent_must_visit_node;

	}
	for(int i=0;i<must_visit_node_cnt;i++) {
		printf("%d|",must_visit_nodes[i]);
	}
	printf("\n");

	//-----------2 ctor graph----------------------------------------------
	const int num_nodes = biggest_node_id;
	printf("biggest_node_id:%d\n",biggest_node_id);
	graph_t g(edge_vector.begin(), edge_vector.end(), weight_vecotr.begin(), num_nodes);
	property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);
	std::vector<vertex_descriptor> p(num_vertices(g));
	std::vector<int> d(num_vertices(g));


	//-----------3 cacl and sort shortest path------------------------------
	//-----------3.1 cacl shortest path-------------------------------------
	const int shortest_path_cache_num = must_visit_node_cnt+2;

	std::unordered_map<int,std::unordered_map<int,int > >src_dst_cost(num_nodes);
	std::unordered_map<int,std::vector<std::pair<int,int> > >src_dst_cost_ordered(num_nodes);
	std::unordered_map<int,std::unordered_map<int,std::vector<int > > > src_dst_path(num_nodes);

	for(int i=0;i<shortest_path_cache_num;i++) {
		int start_node;
		if(i==shortest_path_cache_num-2) {
			start_node = demand_src_id;
		} else if(i==shortest_path_cache_num-1) {
			start_node = demand_dst_id;
		} else {
			start_node = must_visit_nodes[i];
		}
		vertex_descriptor s = vertex(start_node, g);

		dijkstra_shortest_paths(g, s,
		                        predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).
		                        distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));

		graph_traits < graph_t >::vertex_iterator vi, vend;
		for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
			//printf("%d->%d\n",start_node,*vi);
			if((int)d[*vi]<std::numeric_limits<int>::max()) {
				if(((int)d[*vi]!=0&&start_node!=(int)*vi
					&&std::find(must_visit_nodes,must_visit_nodes+must_visit_node_cnt,(int)*vi)!=must_visit_nodes+must_visit_node_cnt)
				   ||(demand_src_id==start_node&&(int)d[*vi]!=0&&std::find(must_visit_nodes,must_visit_nodes+must_visit_node_cnt,(int)*vi)!=must_visit_nodes+must_visit_node_cnt)
				   ||(demand_dst_id==start_node&&(int)d[*vi]!=0&&std::find(must_visit_nodes,must_visit_nodes+must_visit_node_cnt,(int)*vi)!=must_visit_nodes+must_visit_node_cnt)
				   ||((int)*vi==demand_src_id&&(int)d[*vi]!=0&&start_node!=(int)*vi&&start_node!=demand_dst_id)
				   ||((int)*vi==demand_dst_id&&(int)d[*vi]!=0&&start_node!=(int)*vi&&start_node!=demand_src_id)
						) {
					//printf("55555\n");
					src_dst_cost[start_node][*vi]=(int)d[*vi];
					printf("%d->%d:%d\n",start_node,(int)*vi,(int)d[*vi]);
					int vp=p[*vi];

					if(start_node==18&&(int)*vi==15) {
						printf("start_node==18&&(int)*vi=15\n");
						printf("(demand_src_id==start_node&&(int)d[*vi]!=0): %d\n",(demand_src_id==start_node&&(int)d[*vi]!=0));
						printf("vp:%ld\n",vp);
					}
					//std::cout << "parent "<<vp<<std::endl; //<< vp << ") ";
					//printf("vp:%d start_node:%d *vi%d vp!=start_node:%d vp!=(int)*vi:%d\n",vp,vi,vp!=start_node,vp!=(int)*vi);
					if(vp!=start_node&&vp!=(int)*vi) {
						src_dst_path[start_node][*vi].push_back(start_node);
					}
					std::vector<int> reverse_path;
					reverse_path.reserve(num_nodes);
					while(vp!=start_node&&vp!=(int)*vi) {

						reverse_path.push_back(vp);
						//std::cout<<vp<<" ";
						vp=p[vp];
					}
					for(auto it=reverse_path.rbegin();it!=reverse_path.rend();it++) {
						src_dst_path[start_node][*vi].push_back(*it);
					}
					if(!src_dst_path[start_node][*vi].empty()) {
						//src_dst_path[start_node][*vi].push_back((int)*vi);
					} else {
						if(vp==start_node) {
							src_dst_path[start_node][*vi].push_back(start_node);
							//src_dst_path[start_node][*vi].push_back((int)*vi);
						}
					}
				}
			}

		}
	}

	//------------------- 3.2 sort shortest path-----------------------------
	for(auto srci=src_dst_cost.begin();srci!=src_dst_cost.end();srci++) {
		auto dest_cost = (*srci).second;
		src_dst_cost_ordered[(*srci).first]=std::vector<std::pair<int,int> >();
		auto dest_cost_ordered=src_dst_cost_ordered.find((*srci).first);
		dest_cost_ordered->second.reserve(dest_cost.size());

		for(auto desti=dest_cost.begin();desti!=dest_cost.end();desti++) {
			//printf("UNORDER %d->%d: %d\n",(*srci).first,(*desti).first,desti->second);
			dest_cost_ordered->second.push_back(std::pair<int,int>(desti->second,desti->first));
		}
		std::sort(dest_cost_ordered->second.begin(),dest_cost_ordered->second.end());
		for(auto desti=dest_cost_ordered->second.begin();desti!=dest_cost_ordered->second.end();desti++) {
			printf("ORDERED %d->%d: %d\n",(*srci).first,(*desti).second,desti->first);
		}
		printf("dest_cost.size():%d dest_cost_ordered->second.size():%d\n",dest_cost.size(),dest_cost_ordered->second.size());
		printf("src_dst_cost_ordered[(*srci).first].size():%d\n",src_dst_cost_ordered[(*srci).first].size());
	}	

	printf("----------\n");
	for(auto srci=src_dst_path.cbegin();srci!=src_dst_path.cend();srci++) {
		auto dest_path = (*srci).second;
		for(auto desti=dest_path.cbegin();desti!=dest_path.cend();desti++) {
			auto src_dest_path = (*desti).second;
			printf("%d->%d: ",(*srci).first,(*desti).first);
			for(auto pathi=src_dest_path.cbegin();pathi!=src_dest_path.cend();pathi++) {
				printf("%d,", *pathi);
			}
			printf("\n");
		}
	}

	//-------------------4 dfs-------------------------------------------------

	std::vector<std::vector<int> > rlt_path;
	std::set<int> rlt_pathset;
	std::set<int> cur_pathset;
	std::vector<int> qi;
	int s = demand_src_id;
	int t = demand_dst_id;
	int path_lenth;
	int must_visit_visited_cnt;
init:
	int src=s;
	int i=0;
	path_lenth = 0;
	must_visit_visited_cnt=0;
	int next_node;
	std::vector<int> cur_path=std::vector<int>();
	while(true) {
		auto dst_cost_ordered = src_dst_cost_ordered.find(src);
		printf("rlt_path: ");
		for (auto pv = rlt_path.begin(); pv != rlt_path.end(); pv++) {
			for (auto p = pv->begin(); p != pv->end(); p++) {
				printf("%d ", *p);
			}
		}
		printf("\n");
		if (dst_cost_ordered != src_dst_cost_ordered.end()) {
			//printf("dst_cost_ordered->second.size():%d\n",dst_cost_ordered->second.size());
			if(i<dst_cost_ordered->second.size()) {
				next_node = dst_cost_ordered->second[i].second;
			} else {
				next_node = -1;
			}
		} else {
			next_node = -1;
		}
		printf("src:%d, next_node:%d\n", src, next_node);
		auto dst_path = src_dst_path.find(src);
		int dst_path_size;
		dst_path_size = 0;
		if (dst_path != src_dst_path.end()) {
			auto path = dst_path->second.find(next_node);
			dst_path_size = dst_path->second.size();
			//printf("dst_path->second.size():%d\n",dst_path->second.size());
			if (path != dst_path->second.end()) {
				cur_path = path->second;
				//printf("cur_path.size():%d\n",cur_path.size());
			} else {
				cur_path.clear();
			}
		} else {
			cur_path.clear();
		}
		cur_pathset.clear();
		if(!cur_path.empty()) {
			cur_pathset.insert(cur_path.begin()+1, cur_path.end());
			cur_pathset.insert(next_node);
		}
		std::vector<int> common_nodes;
		common_nodes.clear();
		set_intersection(cur_pathset.begin(), cur_pathset.end(), rlt_pathset.begin(), rlt_pathset.end(),
						 std::back_inserter(common_nodes));
		if (!cur_path.empty() && common_nodes.size() == 0) {
			printf("!cur_path.empty()&&common_nodes.size()==0\n");
			rlt_path.push_back(cur_path);
			path_lenth+=cur_path.size();
			rlt_pathset.insert(cur_path.begin(), cur_path.end());
			qi.push_back(i);
			printf("push back i:%d\n",i);
			i = 0;
			if (next_node == t && must_visit_visited_cnt==must_visit_node_cnt) {
				path_lenth++;
				rlt_path.push_back(vector<int>({next_node}));
				break;
			} else {
				if (next_node != -1) {
					must_visit_visited_cnt++;
					src = next_node;
					continue;
				}
			}
		} else if (next_node==-1) {
			if(src==s) {
				break;
			}
			printf("next_node==-1\n");
			auto last_path = rlt_path.back();
			rlt_path.pop_back();
			auto last_i = qi.back();
			qi.pop_back();
			src = last_path.front();
			i = last_i + 1;
			path_lenth-=last_path.size();
			must_visit_visited_cnt--;
			for(auto it=last_path.begin();it!=last_path.end();it++) {
				rlt_pathset.erase(*it);
			}
			continue;
		} else if (i == dst_path_size) {
			//printf("i==dst_path_size\n");
		} else {
			i++;
			printf("intersect inc i:%d\n", i);
			continue;
		}
	}



	//-------------------5 record result---------------------------------------

	if(must_visit_visited_cnt==must_visit_node_cnt) {
		printf("rlt_path success: \n");
		vector<int> path_nodes;
		path_nodes.reserve(num_nodes);
		for (auto pv = rlt_path.begin(); pv != rlt_path.end(); pv++) {
			for (auto p = pv->begin(); p != pv->end(); p++) {
				printf("%d\n",*(p));
				path_nodes.push_back(*p);
			}
		}

		for (auto p = path_nodes.begin(); p != path_nodes.end()-1; p++) {
			printf("%d->%d\n",*(p),*(p+1));
			Edge e = Edge( *(p),*(p+1));
			auto edge_index_it = std::find (edge_vector.begin(),edge_vector.end(),e);
			int pos = std::distance(edge_vector.begin(),edge_index_it);
			printf("pos:%d %d->%d\n", pos,*(p),*(p+1));
			record_result(pos);
		}
		printf("\n");
	} else {
		printf("NA\n");
	}

}

#undef printf