#include "net/rime.h"
#include "graph.h"

#ifndef __MEMORY_H__
#define __MEMORY_H__

void init_mem(void);

void* get_node_memory(void);

void* get_edge_memory(void);

p_node_t* get_node(void);

p_edge_t* get_edge(void);

#endif
