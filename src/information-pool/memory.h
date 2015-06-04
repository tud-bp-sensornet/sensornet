#include "net/rime.h"
#include "graph.h"

#define MAX_NODES 50
#define MAX_EDGES 50

#ifndef __MEMORY_H__
#define __MEMORY_H__

void init_mem(void);

void* get_node_memory(void);

void* get_edge_memory(void);

p_node_t* get_node(void);

p_edge_t* get_edge(void);

void free_node_memory(p_node_t * p_node);

void free_edge_memory(p_edge_t * p_edge);

#endif
