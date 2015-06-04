#include "net/rime.h"
#include "contiki.h"
#include <stdbool.h>

#ifndef __P_GRAPH_H__
#define __P_GRAPH_H__

typedef struct p_edge_t p_edge_t;
typedef struct p_node_t p_node_t;

#define K 3		//The K of the K-Hop algorithm. Will only update the local tree to a depth of K, so serializing and sending more than K-1 is wasted effort
//static struct p_node_t root_data;
//static struct p_node_t *root;	//The root of the local node information tree

struct p_node_t {
	rimeaddr_t addr;
	unsigned long last_seen;

	//additional node data goes here
	//...

	//points to the first directed edge
	p_edge_t* edges;
} *root, root_data;

struct p_edge_t {
	p_node_t* drain;

	//additional edge data goes here
	//...

	p_edge_t* next;
};

void iterateUpdate(p_node_t * p_node);

p_node_t* iterateUpdateNode(p_node_t * p_node, unsigned short current_k);

p_edge_t* iterateUpdateEdge(p_edge_t * p_edge, unsigned short current_k, bool alloc_required);

void updateNeighbour(p_node_t * p_node);

void updateEdgeList(p_edge_t * edgelist_elem, p_edge_t * new_edge);

p_node_t* handleExistingNode(p_node_t * old_p_node, p_node_t * new_p_node, unsigned short current_k);

void handleExistingNode_Edgelist(p_edge_t * edgelist_elem_MEMB, p_edge_t * edgelist_elem_UPDATE,  p_node_t * source_node, unsigned short current_k);

p_node_t* findNodeAddr(rimeaddr_t addr);

p_node_t* findNodeAddr_Node(p_node_t * p_node, rimeaddr_t addr, unsigned short current_k);

p_node_t* findNodeAddr_Edge(p_edge_t * p_edge, rimeaddr_t addr, unsigned short current_k);

bool edgeAlreadyExists(p_edge_t * p_edge, p_node_t * source_node);

bool edgeAlreadyExists_EdgeList(p_edge_t * p_edge, p_edge_t * edgelist_elem);

//p_node_t* findNodePos(PosX, PosY);

#endif
