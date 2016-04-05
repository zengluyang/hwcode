
#include "route.h"
#include "lib_record.h"
#include <stdio.h>

#include <boost/config.hpp>
#include <iostream>
#include <fstream>

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
	graph_t g(edge_vector.begin(), edge_vector.end(), weight_vecotr.begin(), num_nodes);
	property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);
	std::vector<vertex_descriptor> p(num_vertices(g));
	std::vector<int> d(num_vertices(g));
	int start_node =2;
	vertex_descriptor s = vertex(start_node, g);

	dijkstra_shortest_paths(g, s,
	                        predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).
	                        distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));

	std::cout << "distances and parents:" << std::endl;
	graph_traits < graph_t >::vertex_iterator vi, vend;
	for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
		std::cout << "distance(" << *vi << ") = " << d[*vi]<< ", ";
		std::cout << "parent(" << *vi << ") = ";
		int vp=p[*vi];
		int i=3;
		while(vp!=start_node&&vp!=*vi) {
			std::cout<<vp<<" ";
			vp=p[vp];
			i--;
		}
		std::cout<< std::endl;
		std::cout<<"p ";
		for(int i=0;i<p.size();i++) {
			std::cout<<p[i]<<" ";
		}
		std::cout<<endl;
	}
	std::cout << std::endl;

    for (int i = 0; i < 3; i++)
        record_result(result[i]);
}


void test_boost() {
	using namespace boost;
	typedef adjacency_list < listS, vecS, directedS,
	  no_property, property < edge_weight_t, int > > graph_t;
	typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
	typedef std::pair<int, int> Edge;

	const int num_nodes = 5;
	enum nodes { A, B, C, D, E };
	char name[] = "ABCDE";
	Edge edge_array[] = { Edge(A, C), Edge(B, B), Edge(B, D), Edge(B, E),
	  Edge(C, B), Edge(C, D), Edge(D, E), Edge(E, A), Edge(E, B)
	};
	int weights[] = { 1, 2, 1, 2, 7, 3, 1, 1, 1 };
	int num_arcs = sizeof(edge_array) / sizeof(Edge);
	graph_t g(edge_array, edge_array + num_arcs, weights, num_nodes);
	property_map<graph_t, edge_weight_t>::type weightmap = get(edge_weight, g);
	std::vector<vertex_descriptor> p(num_vertices(g));
	std::vector<int> d(num_vertices(g));
	vertex_descriptor s = vertex(A, g);

	dijkstra_shortest_paths(g, s,
	                        predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).
	                        distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));

	std::cout << "distances and parents:" << std::endl;
	graph_traits < graph_t >::vertex_iterator vi, vend;
	for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi) {
	  std::cout << "distance(" << name[*vi] << ") = " << d[*vi] << ", ";
	  std::cout << "parent(" << name[*vi] << ") = " << name[p[*vi]] << std::
	    endl;
	}
	std::cout << std::endl;

}