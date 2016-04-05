
#include "route.h"
#include "lib_record.h"
#include <stdio.h>

#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>

using namespace boost;
using namespace std;


void test_boost();

//你要完成的功能总入口

    
typedef adjacency_list < vecS, vecS, directedS,
  no_property, property < edge_weight_t, int > > graph_t;
typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
typedef std::pair<int, int> Edge;

void search_route(char *topo[5000], int edge_num, char *demand)
{
	//test_boost();
    unsigned short result[] = {2, 6, 3};//示例中的一个解




   	std::vector<Edge> edge_vector(edge_num);
   	std::vector<int> weight_vecotr(edge_num);
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
    printf("%d,%d,",demand_src_id,demand_dst_id);
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

	const int num_nodes = biggest_node_id;
	printf("biggest_node_id:%d\n",biggest_node_id);
	graph_t g(edge_vector.begin(), edge_vector.end(), weight_vecotr.begin(), num_nodes);
	property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);
	std::vector<vertex_descriptor> p(num_vertices(g));
	std::vector<int> d(num_vertices(g));

	const int shortest_path_cache_num = must_visit_node_cnt+2;

	std::unordered_map<int,std::unordered_map<int,int > >src_dst_cost(num_nodes);
	std::unordered_map<int,std::unordered_map<int,std::vector<int> > > src_dst_path(num_nodes);
	// for(int i=0;i<num_nodes;i++) {
	// 	src_dst_cost.push_back(std::vector<int>(num_nodes,0));
	// }

	// for(int i=0;i<num_nodes;i++) {
	// 	std::vector<std::vector<int> > dest_path(num_nodes);
	// 	for(int j=0;j<num_nodes;j++) {
	// 		dest_path.push_back(std::vector<int>(num_nodes,0));
	// 	}
	// 	src_dst_path.push_back(dest_path);
	// }


	for(int i=0;i<must_visit_node_cnt;i++) {
		int start_node = must_visit_nodes[i];
		vertex_descriptor s = vertex(start_node, g);

		dijkstra_shortest_paths(g, s,
		                        predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).
		                        distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));

		graph_traits < graph_t >::vertex_iterator vi, vend;
		for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
			printf("%d->%d\n",start_node,*vi);
			src_dst_cost[start_node][*vi]=(int)d[*vi];
			std::cout << "distance(" << *vi << ") = " << d[*vi]<< ", ";
			int vp=p[*vi];
			std::cout << "parent "; //<< vp << ") ";
			//printf("vp:%d start_node:%d *vi%d vp!=start_node:%d vp!=(int)*vi:%d\n",vp,vi,vp!=start_node,vp!=(int)*vi);
			while(vp!=start_node&&vp!=(int)*vi) {
				src_dst_path[start_node][*vi].push_back(vp);
				std::cout<<vp<<" ";
				vp=p[vp];
			}
			std::cout<< std::endl;
		}
		std::cout << std::endl;		
	}

	for(auto srci=src_dst_cost.cbegin();srci!=src_dst_cost.cend();srci++) {
		auto dest_cost = (*srci).second;
		for(auto desti=dest_cost.cbegin();desti!=dest_cost.cend();desti++) {
			printf("%d->%d: %d\n",(*srci).first,(*desti).first,desti->second);
		}
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


    for (int i = 0; i < 3; i++)
        record_result(result[i]);
}
