#include "net/rime.h"
#include "contiki.h"

#ifndef __P_GRAPH_H__
#define __P_GRAPH_H__


typedef struct p_edge_t p_edge_t;
typedef struct p_node_t p_node_t;

struct p_node_t {
	rimeaddr_t addr;
	unsigned long last_seen;

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

void iterateUpdate(p_node_t * p_node, void* node_memory, void* edge_memory, p_node_t * this_mote_node);

p_node_t* iterateUpdateNode(p_node_t * p_node, void* node_memory, void* edge_memory, p_node_t * this_mote_node);

p_edge_t* iterateUpdateEdge(p_edge_t * p_edge, p_node_t source_node, void* node_memory, void* edge_memory, p_node_t * this_mote_node);

void updateNeighbour(p_node_t * p_node, void* node_memory, void* edge_memory, p_node_t * this_mote_node);

void updateEdgeList(p_edge_t * mote_p_edge, p_edge_t * new_edge);

p_node_t* handleExistingNode(p_node_t * old_p_node, p_node_t * new_p_node);

p_node_t* findNodeAddr(p_node_t * this_mote_node, rimeaddr_t addr);

void freeEdges(p_edge_t * p_edge, void* node_memory, void* edge_memory);

p_edge_t* edgeAlreadyExists(p_edge_t * p_edge, p_node_t * source_node, p_node_t * this_mote_node);

//p_node_t* findNodePos(p_node_t * this_mote_node, PosX, PosY);

#endif
