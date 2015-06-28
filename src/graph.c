#include "contiki.h"
#include "net/rime.h"

#include <stdlib.h>
#include <string.h>

#include "graph.h"

//Static memory containing the saved node and egde information of the graph created by the k-hop-algorithm
MEMB(node_memory, p_node_t, MAX_NODES);
MEMB(edge_memory, p_edge_t, MAX_EDGES);

//Arrays containing pointers to the node and edge information saved in MEMB
//The free spaces (NULL pointers) will always be located after the filled spaces
static p_node_t** node_memory_array;
static p_edge_t** edge_memory_array;

//Counters for how many nodes and edges are currently saved
static uint8_t node_count = 0;
static uint8_t edge_count = 0;

/**
 * Needs to be called at the start of main once to initialize the memory
 */
void init_graph()
{
	memb_init(&node_memory);
	memb_init(&edge_memory);
	node_memory_array = (p_node_t**) malloc(sizeof(p_node_t*) * MAX_NODES);
	edge_memory_array = (p_edge_t**) malloc(sizeof(p_edge_t*) * MAX_EDGES);
}

/**************************************************************/
/**************************Node********************************/
/**************************************************************/

/**
 * Adds a new node to the memory
 * node: The node to be added to the memory
 */
void add_node(const p_node_t node)
{
	//Exit the function early, if the node already exists
	if (find_node(&(node.addr)) != NULL)
	{
		return;
	}
	p_node_t *allocated_node = (p_node_t*) memb_alloc(&node_memory);
	//only perform further actions if the node could be allocated correctly
	if (allocated_node != NULL)
	{
		node_count++;
		//add the pointer to the first empty space in node_memory_array
		uint8_t i;
		for (i = 0; i < MAX_NODES; i++)
		{
			if (node_memory_array[i] == NULL)
			{
                //copy content
                node_memory_array[i] = allocated_node;
                memcpy(allocated_node, &node, sizeof(p_node_t));
                //exit loop
				break;
			}
		}
	}
}

/**
 * Removes the node with the specified rimeaddr from the memory and frees the memory used by the node
 * addr: Pointer to the rimeaddr
 */
void remove_node(const rimeaddr_t *addr)
{
	uint8_t i;
	for (i = 0; i < node_count; i++)
	{
		if (rimeaddr_cmp(&(node_memory_array[i]->addr), addr))
		{
			node_count--;
			memb_free (&node_memory, node_memory_array[i]);
			//Make sure that NULL isn't in the middle of the array
			uint8_t j;
			for (j = i; j < node_count; j++)
			{
				node_memory_array[j] = node_memory_array[j+1];
			}
			node_memory_array[node_count] = NULL;
			break;
		}
	}
}

/**
 * Returns an array of pointers to the saved nodes in memory. The array should neither be freed, nor changed outside of graph.c.
 * count: A pointer to an uint8 value that will be set to the current amount of saved nodes
 * return: The pointer to the array of p_node_t*
 */
p_node_t **get_all_nodes(uint8_t *count)
{
	*count = node_count;
	return node_memory_array;
}

/**
 * Returns a pointer to the node with the specified rimeaddr. If the node does not exist, NULL is returned.
 * addr: Pointer to the rimeaddr
 * return: Pointer to the node
 */
p_node_t *find_node(const rimeaddr_t *addr)
{
	uint8_t i;
	for (i = 0; i < node_count; i++)
	{
		if (rimeaddr_cmp(&(node_memory_array[i]->addr), addr))
		{
			return node_memory_array[i];
		}
	}
	return NULL;
}

/**
 * Returns how many nodes are currently saved
 * return: The amount of nodes
 */
uint8_t get_node_count()
{
	return node_count;
}

/**************************************************************/
/**************************Edge********************************/
/**************************************************************/


/**
 * Adds a new edge to the memory
 * edge: The edge to be added to the memory
 */
void add_edge(const p_edge_t edge)
{
	//Exit the function early, if the edge already exists
	if (find_edge(&(edge.src), &(edge.dst)) != NULL)
	{
		return;
	}
	p_edge_t *allocated_edge = (p_edge_t*) memb_alloc(&edge_memory);
	//only perform further actions if the edge could be allocated correctly
	if (allocated_edge != NULL)
	{
		edge_count++;
		//add the pointer to the first empty space in edge_memory_array
		uint8_t i;
		for (i = 0; i < MAX_EDGES; i++)
		{
			if (edge_memory_array[i] == NULL)
			{
                //copy content
                edge_memory_array[i] = allocated_edge;
                memcpy(allocated_edge, &edge, sizeof(p_edge_t));
                //exit loop
				break;
			}
		}
	}
}

/**
 * Removes the edge with the two specified rimeaddr from the memory and frees the memory used by the edge
 * src: Pointer to the rimeaddr of the source node
 * dst: Pointer to the rimeaddr of the destination node
 */
void remove_edge(const rimeaddr_t *src, const rimeaddr_t *dst)
{
	uint8_t i;
	for (i = 0; i < edge_count; i++)
	{
		if ((rimeaddr_cmp(&(edge_memory_array[i]->src), src)) && (rimeaddr_cmp(&(edge_memory_array[i]->dst), dst)))
		{
			edge_count--;
			memb_free (&edge_memory, edge_memory_array[i]);
			//Make sure that NULL isn't in the middle of the array
			uint8_t j;
			for (j = i; j < edge_count; j++)
			{
				edge_memory_array[j] = edge_memory_array[j+1];
			}
			edge_memory_array[edge_count] = NULL;
			break;
		}
	}
}

/**
 * Returns an array of pointers to the saved edges in memory. The array should neither be freed, nor changed outside of graph.c.
 * count: A pointer to an uint8 value that will be set to the current amount of saved edges
 * return: The pointer to the array of p_edge_t*
 */
p_edge_t **get_all_edges(uint8_t *count){
	*count = edge_count;
	return edge_memory_array;
}

/**
 * Returns an array of pointers to the outgoing edges of the node with the specified rimeaddr
 * WARNING: The returned array has to be freed after the function was called.
 * src_addr: Pointer to the rimeaddr of the node
 * count: A pointer to an uint8 value that will be set to the amount of outgoing edges
 * return: Pointer to an array of pointers to edges
 */
p_edge_t **get_outgoing_edges(const rimeaddr_t *src_addr, uint8_t *count)
{
	//count how many outgoing edges the node has
	*count = 0;
	uint8_t i;
	for (i = 0; i < edge_count; i++)
	{
		if (rimeaddr_cmp(&(edge_memory_array[i]->src), src_addr))
		{
			*count = (*count)+1;
		}
	}

	//if there aren't any edges to be returned, return NULL and end the function
	if ((*count) == 0)
	{
		return NULL;
	}

	//allocate memory to be returned
	p_edge_t** outgoing_edge_array = (p_edge_t**) malloc(sizeof(p_edge_t*) * (*count));
	uint8_t current_num = 0;

	//insert the outgoing edges into the array to be returned
	for (i = 0; i < edge_count; i++)
	{
		if (rimeaddr_cmp(&(edge_memory_array[i]->src), src_addr))
		{
			outgoing_edge_array[current_num] = edge_memory_array[i];
			current_num++;
		}
	}

	return outgoing_edge_array;
}

/**
 * Returns an array of pointers to the ingoing edges of the node with the specified rimeaddr
 * WARNING: The returned array has to be freed after the function was called.
 * dst_addr: Pointer to the rimeaddr of the node
 * count: A pointer to an uint8 value that will be set to the amount of ingoing edges
 * return: Pointer to an array of pointers to edges
 */
p_edge_t **get_ingoing_edges(const rimeaddr_t *dst_addr, uint8_t *count)
{
	//count how many ingoing edges the node has
	*count = 0;
	uint8_t i;
	for (i = 0; i < edge_count; i++)
	{
		if (rimeaddr_cmp(&(edge_memory_array[i]->dst), dst_addr))
		{
			*count = (*count)+1;
		}
	}

	//if there aren't any edges to be returned, return NULL and end the function
	if ((*count) == 0)
	{
		return NULL;
	}

	//allocate memory to be returned
	p_edge_t** ingoing_edge_array = (p_edge_t**) malloc(sizeof(p_edge_t*) * (*count));
	uint8_t current_num = 0;

	//insert the outgoing edges into the array to be returned
	for (i = 0; i < edge_count; i++)
	{
		if (rimeaddr_cmp(&(edge_memory_array[i]->dst), dst_addr))
		{
			ingoing_edge_array[current_num] = edge_memory_array[i];
			current_num++;
		}
	}

	return ingoing_edge_array;
}

/**
 * Returns a pointer to the edge with the source and destination nodes that have the two specified rimeaddr. If it does not exist in the memory, return NULL
 * src: Pointer to the rimeaddr of the source node
 * dst: Pointer to the rimeaddr of the destination node
 * return:
 */
p_edge_t *find_edge(const rimeaddr_t *src, const rimeaddr_t *dst)
{
	uint8_t i;
	for (i = 0; i < edge_count; i++)
	{
		if ((rimeaddr_cmp(&(edge_memory_array[i]->src), src)) && (rimeaddr_cmp(&(edge_memory_array[i]->dst), dst)))
		{
			return edge_memory_array[i];
		}
	}

	return NULL;
}

/**
 * Returns how many edges are currently saved
 * return: The amount of edges
 */
uint8_t get_edge_count()
{
	return edge_count;
}
