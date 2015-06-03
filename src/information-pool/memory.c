#include "contiki.h"

#include <stdio.h>
#include <stdlib.h>

#include "graph.h"

#define MAX_NODES 50
#define MAX_EDGES 50

MEMB(node_memory, p_node_t, MAX_NODES);
MEMB(edge_memory, p_edge_t, MAX_EDGES);

void init_mem()
{
	memb_init(&node_memory);
	memb_init(&edge_memory);

}

void* get_node_memory()
{
	return &node_memory.mem;
}

void* get_edge_memory()
{
	return edge_memory.mem;
}

p_node_t* get_node()
{
	return (p_node_t*) memb_alloc(&node_memory);
}

p_edge_t* get_edge()
{
	return (p_edge_t*) memb_alloc(&edge_memory);
}

