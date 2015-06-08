#include "net/rime.h"
#include "graph.h"

#ifndef __MEMORY_H__
#define __MEMORY_H__

#define MAX_NODES 50
#define MAX_EDGES 50

void init_mem(void);

p_node_t* new_node(void);
p_edge_t* new_edge(void);

void free_node(p_node_t * p_node);
void free_edge(p_edge_t * p_edge);

#endif
