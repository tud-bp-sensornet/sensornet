/**
 * \file graph.c
 * The Information-Pool containing all node and edge information of the graph built by the k-hop algorithm.
 * \author tud-bp-sensornet
 */

#include <stdlib.h>
#include <string.h>

#include "graph.h"

/**
 * \def __GRAPH_DEBUG__
 * \brief Set to 1 to activate debug output.
*/
#ifndef __GRAPH_DEBUG__
#define __GRAPH_DEBUG__ 0
#endif

//\cond
#if __GRAPH_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
//\endcond

// Static memory containing the saved node and edge information of the graph created by the k-hop-algorithm
//\cond
MEMB(node_memory, p_node_t, MAX_NODES);
MEMB(edge_memory, p_edge_t, MAX_EDGES);
//\endcond

// Arrays containing pointers to the node and edge information saved in MEMB
// The free spaces (NULL pointers) will always be located after the filled spaces
static p_node_t *node_memory_array[MAX_NODES];
static p_edge_t *edge_memory_array[MAX_EDGES];

// Counters for how many nodes and edges are currently saved
static uint8_t node_count;
static uint8_t edge_count;

/*---------------------------------------------------------------------------*/
void init_graph()
{
	PRINTF("[graph.c] Initializing graph.\n");

	memb_init(&node_memory);
	memb_init(&edge_memory);

	node_count = 0;
	edge_count = 0;
}
/*---------------------------------------------------------------------------*/
void add_node(const p_node_t node)
{
	PRINTF("[graph.c] Adding node %d.%d (%d/%d)!\n", node.addr.u8[0], node.addr.u8[1], node.pos.x, node.pos.y);

	// Update edge if it already exists in graph
	p_node_t *search_node = find_node(&(node.addr));
	if (search_node != NULL)
	{
		memcpy(search_node, &node, sizeof(p_node_t));
		PRINTF("[graph.c] ^ Updated node %d.%d.\n", node.addr.u8[0], node.addr.u8[1]);
		return;
	}
#if __GRAPH_DEBUG__
	else
	{
		PRINTF("[graph.c] ^ Node %d.%d is not yet in the graph.\n", node.addr.u8[0], node.addr.u8[1]);
	}
#endif

	// Exit if there is no space for more nodes
	if (node_count >= MAX_NODES)
	{
		PRINTF("[graph.c] ^ Node was not added due to lack of space.\n");
		return;
	}

	// Add node to graph
	p_node_t *allocated_node = (p_node_t *) memb_alloc(&node_memory);
	if (allocated_node != NULL)
	{
		node_count++;
		// Add the pointer to the first empty space in node_memory_array
		uint8_t i;
		for (i = 0; i < MAX_NODES; i++)
		{
			if (node_memory_array[i] == NULL)
			{
				node_memory_array[i] = allocated_node;
				memcpy(allocated_node, &node, sizeof(p_node_t));
				PRINTF("[graph.c] ^ Added node %d.%d.\n", node.addr.u8[0], node.addr.u8[1]);

				break;
			}
		}
	}
#if __GRAPH_DEBUG__
	else
	{
		PRINTF("[graph.c] ^ Node %d.%d could not be allocated.\n", node.addr.u8[0], node.addr.u8[1]);
	}
#endif
}
/*---------------------------------------------------------------------------*/
void remove_node(const linkaddr_t *addr)
{
	// Don't do anything if parameters are NULL
	if (addr == NULL)
	{
		return;
	}

	uint8_t i;
	for (i = 0; i < node_count; i++)
	{
		if (linkaddr_cmp(&(node_memory_array[i]->addr), addr))
		{
			node_count--;
			memb_free (&node_memory, node_memory_array[i]);
			// Make sure that NULL isn't in the middle of the array
			uint8_t j;
			for (j = i; j < node_count; j++)
			{
				node_memory_array[j] = node_memory_array[j + 1];
			}
			node_memory_array[node_count] = NULL;
			PRINTF("[graph.c] Removed node %d.%d.\n", addr->u8[0], addr->u8[1]);
			break;
		}
	}
}
/*---------------------------------------------------------------------------*/
p_node_t **get_all_nodes(uint8_t *count)
{
	//Don't do anything if parameters are NULL
	if (count == NULL)
	{
		return NULL;
	}

	*count = node_count;

	return node_memory_array;
}
/*---------------------------------------------------------------------------*/
p_node_t *find_node(const linkaddr_t *addr)
{
	// Don't do anything if parameters are NULL
	if (addr == NULL)
	{
		return NULL;
	}

	uint8_t i;
	for (i = 0; i < node_count; i++)
	{
		if (linkaddr_cmp(&(node_memory_array[i]->addr), addr))
		{
			return node_memory_array[i];
		}
	}

	return NULL;
}
/*---------------------------------------------------------------------------*/
uint8_t get_node_count()
{
	return node_count;
}
/*---------------------------------------------------------------------------*/
void add_edge(const p_edge_t edge)
{
	PRINTF("[graph.c] Adding edge %d.%d -> %d.%d (ttl: %u, rssi: %d, lqi: %d).\n",
	       edge.src.u8[0], edge.src.u8[1], edge.dst.u8[0], edge.dst.u8[1], edge.ttl, edge.rssi, edge.lqi);

	// Update edge if it already exists in graph
	p_edge_t *search_edge = find_edge(&(edge.src), &(edge.dst));
	if (search_edge != NULL)
	{
		if (search_edge->ttl > edge.ttl)
		{
			PRINTF("[graph.c] ^ Edge exists, but transmitted info is older --> no update.\n");
			return;
		}

		memcpy(search_edge, &edge, sizeof(p_edge_t));
		PRINTF("[graph.c] ^ Updated edge %d.%d -> %d.%d.\n",
		       edge.src.u8[0], edge.src.u8[1], edge.dst.u8[0], edge.dst.u8[1]);
		return;
	}

	// Exit if there is no space for more edges
	if (edge_count >= MAX_EDGES)
	{
		PRINTF("[graph.c] ^ Edge was not added due to lack of space.\n");
		return;
	}

	// Exit if src or dst is not in our node memory
	if (find_node(&(edge.src)) == NULL || find_node(&(edge.dst)) == NULL)
	{
		PRINTF("[graph.c] ^ Edge was not added due to lack of dst or src in node memory.\n");
		return;
	}

	// Add edge to graph
	p_edge_t *allocated_edge = (p_edge_t *) memb_alloc(&edge_memory);
	if (allocated_edge != NULL)
	{
		edge_count++;

		// Add the pointer to the first empty space in edge_memory_array
		uint8_t i;
		for (i = 0; i < MAX_EDGES; i++)
		{
			if (edge_memory_array[i] == NULL)
			{
				edge_memory_array[i] = allocated_edge;
				memcpy(allocated_edge, &edge, sizeof(p_edge_t));
				PRINTF("[graph.c] ^ Allocated edge %d.%d -> %d.%d.\n", edge.src.u8[0], edge.src.u8[1], edge.dst.u8[0], edge.dst.u8[1]);

				break;
			}
		}
	}
#if __GRAPH_DEBUG__
	else
	{
		PRINTF("[graph.c] ^ Edge %d.%d -> %d.%d could not be allocated.\n", edge.src.u8[0], edge.src.u8[1], edge.dst.u8[0], edge.dst.u8[1]);
	}
#endif
}
/*---------------------------------------------------------------------------*/
void remove_edge(const linkaddr_t *src, const linkaddr_t *dst)
{
	// Don't do anything if parameters are NULL
	if (src == NULL || dst == NULL)
	{
		return;
	}

	uint8_t i;
	for (i = 0; i < edge_count; i++)
	{
		if ((linkaddr_cmp(&(edge_memory_array[i]->src), src)) && (linkaddr_cmp(&(edge_memory_array[i]->dst), dst)))
		{
			edge_count--;
			memb_free (&edge_memory, edge_memory_array[i]);

			// Make sure that NULL isn't in the middle of the array
			uint8_t j;
			for (j = i; j < edge_count; j++)
			{
				edge_memory_array[j] = edge_memory_array[j + 1];
			}

			edge_memory_array[edge_count] = NULL;

			PRINTF("[graph.c] Removed edge %d.%d -> %d.%d.\n", src->u8[0], src->u8[1], dst->u8[0], dst->u8[1]);
			break;
		}
	}
}
/*---------------------------------------------------------------------------*/
p_edge_t **get_all_edges(uint8_t *count)
{
	// Don't do anything if parameters are NULL
	if (count == NULL)
	{
		return NULL;
	}

	*count = edge_count;

	return edge_memory_array;
}
/*---------------------------------------------------------------------------*/
p_edge_t **get_outgoing_edges(const linkaddr_t *src_addr, uint8_t *count)
{
	// Don't do anything if parameters are NULL
	if (src_addr == NULL || count == NULL)
	{
		return NULL;
	}

	// Count how many outgoing edges the node has
	*count = 0;
	uint8_t i;
	for (i = 0; i < edge_count; i++)
	{
		if (linkaddr_cmp(&(edge_memory_array[i]->src), src_addr))
		{
			*count = (*count) + 1;
		}
	}

	// If there aren't any edges to be returned, return NULL and end the function
	if ((*count) == 0)
	{
		return NULL;
	}

	// Allocate memory to be returned
	p_edge_t **outgoing_edge_array = (p_edge_t **) malloc(sizeof(p_edge_t *) * (*count));
	if (outgoing_edge_array == NULL)
	{
		PRINTF("[graph.c] Failed allocating outgoing edge array.\n");
		*count = 0;
		return NULL;
	}

	uint8_t current_num = 0;

	// Insert the outgoing edges into the array to be returned
	for (i = 0; i < edge_count; i++)
	{
		if (linkaddr_cmp(&(edge_memory_array[i]->src), src_addr))
		{
			outgoing_edge_array[current_num] = edge_memory_array[i];
			current_num++;
		}
	}

	return outgoing_edge_array;
}
/*---------------------------------------------------------------------------*/
p_edge_t **get_ingoing_edges(const linkaddr_t *dst_addr, uint8_t *count)
{
	// Don't do anything if parameters are NULL
	if (dst_addr == NULL || count == NULL)
	{
		return NULL;
	}

	// Count how many ingoing edges the node has
	*count = 0;
	uint8_t i;
	for (i = 0; i < edge_count; i++)
	{
		if (linkaddr_cmp(&(edge_memory_array[i]->dst), dst_addr))
		{
			*count = (*count) + 1;
		}
	}

	// If there aren't any edges to be returned, return NULL and end the function
	if ((*count) == 0)
	{
		return NULL;
	}

	// Allocate memory to be returned
	p_edge_t **ingoing_edge_array = (p_edge_t **) malloc(sizeof(p_edge_t *) * (*count));
	if (ingoing_edge_array == NULL)
	{
		PRINTF("[graph.c] Failed allocating ingoing edge array.\n");
		*count = 0;
		return NULL;
	}

	uint8_t current_num = 0;

	// Insert the ingoing edges into the array to be returned
	for (i = 0; i < edge_count; i++)
	{
		if (linkaddr_cmp(&(edge_memory_array[i]->dst), dst_addr))
		{
			ingoing_edge_array[current_num] = edge_memory_array[i];
			current_num++;
		}
	}

	return ingoing_edge_array;
}
/*---------------------------------------------------------------------------*/
p_edge_t *find_edge(const linkaddr_t *src, const linkaddr_t *dst)
{
	// Don't do anything if parameters are NULL
	if (src == NULL || dst == NULL)
	{
		return NULL;
	}

	uint8_t i;
	for (i = 0; i < edge_count; i++)
	{
		if ((linkaddr_cmp(&(edge_memory_array[i]->src), src)) && (linkaddr_cmp(&(edge_memory_array[i]->dst), dst)))
		{
			return edge_memory_array[i];
		}
	}

	return NULL;
}
/*---------------------------------------------------------------------------*/
uint8_t get_edge_count()
{
	return edge_count;
}
/*---------------------------------------------------------------------------*/
