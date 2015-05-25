#include "net/rime.h"

#ifndef __P_GRAPH_H__
#define __P_GRAPH_H__


typedef struct p_edge_t p_edge_t;
typedef struct p_node_t p_node_t;

struct p_node_t {
	rimeaddr_t addr;
	unsigned short last_seen;

	//additional node data goes here
	//...

	//points to the first directed edge
	p_edge_t* edges;
};

struct p_edge_t {
	p_node_t* drain;

	//additional edge data goes here
	//...

	p_edge_t* next;
};

#endif
