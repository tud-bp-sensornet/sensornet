#include "net/rime.h"
#include "graph.h"

#ifndef __MEMORY_H__
#define __MEMORY_H__

#define MAX_NODES 50
#define MAX_EDGES 50

void init_mem(void);

p_node_t* new_node(uint8_t * num_nodes);
p_edge_t* new_edge(uint8_t * num_edges);

void free_node(p_node_t * p_node, uint8_t * num_nodes);
void free_edge(p_edge_t * p_edge, uint8_t * num_edges);

#endif
