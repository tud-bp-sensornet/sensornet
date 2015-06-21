#include "contiki.h"

#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "memory.h"

MEMB(node_memory, p_node_t, MAX_NODES);
MEMB(edge_memory, p_edge_t, MAX_EDGES);

void init_mem()
{
	memb_init(&node_memory);
	memb_init(&edge_memory);
}

p_node_t* new_node(uint8_t * num_nodes)
{
	*num_nodes += 1; //new node in the graph
	return (p_node_t*) memb_alloc(&node_memory);
}

p_edge_t* new_edge(uint8_t * num_edges)
{
	*num_edges += 1; //new edge in the graph
	return (p_edge_t*) memb_alloc(&edge_memory);
}

void free_node(p_node_t * p_node, uint8_t * num_nodes){
	*num_nodes -= 1; //removed node from graph
	memb_free (&node_memory, p_node);
}

void free_edge(p_edge_t * p_edge, uint8_t * num_edges){
	*num_edges -= 1; //removed edge from graph
	memb_free (&edge_memory, p_edge);
}
