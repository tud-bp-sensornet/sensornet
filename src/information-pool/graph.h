#include "net/rime.h"
#include "contiki.h"

#ifndef __P_GRAPH_H__
#define __P_GRAPH_H__

#define K 3		// The K of the K-Hop algorithm.
// Will only update the local tree to a depth of K,
// so serializing and sending more than K-1 is wasted effort

typedef struct p_edge_t p_edge_t;
typedef struct p_node_t p_node_t;
typedef struct p_graph_t p_graph_t;

struct p_node_t {
	rimeaddr_t addr;

	// additional node data goes here
	// ...

	//points to the first directed edge
	p_edge_t* edges;
} *root, root_data;

struct p_edge_t {
	p_node_t* drain;
	uint8_t ttl;

	// additional edge data goes here
	// ...

	p_edge_t* next;
};

struct p_graph_t {
	p_node_t* root;
	uint8_t num_edges;
	uint8_t num_nodes;
};

p_node_t* findNodeByAddr(p_node_t* root, rimeaddr_t addr);

void updateGraph(p_graph_t* graph, p_node_t * other);

#endif
