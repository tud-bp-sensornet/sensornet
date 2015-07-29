#include "contiki.h"
#include "net/rime.h"

//the malloc of <stdlib.h> has strange behavior and leads to an illegal read. the malloc of https://github.com/contiki/contiki-2.x/blob/master/backyard/core/lib/malloc.c was used instead.
#include "malloc.h"
#include <string.h>

#include "graph.h"

//Static memory containing the saved node and edge information of the graph created by the k-hop-algorithm
MEMB(node_memory, p_node_t, MAX_NODES);
MEMB(edge_memory, p_edge_t, MAX_EDGES);

//Arrays containing pointers to the node and edge information saved in MEMB
//The free spaces (NULL pointers) will always be located after the filled spaces
static p_node_t** node_memory_array;
static p_edge_t** edge_memory_array;

//Counters for how many nodes and edges are currently saved
static uint8_t node_count;
static uint8_t edge_count;

/**
 * \brief Needs to be called at the start of main once to initialize the memory
 */
void init_graph()
{
	memb_init(&node_memory);
	memb_init(&edge_memory);
	node_memory_array = (p_node_t**) malloc(sizeof(p_node_t*) * MAX_NODES);
	edge_memory_array = (p_edge_t**) malloc(sizeof(p_edge_t*) * MAX_EDGES);
	node_count = 0;
	edge_count = 0;
}

/**************************************************************/
/**************************Node********************************/
/**************************************************************/

/**
 * \brief Adds a new node to the memory
 * \param node The node to be added to the memory
 *
 * Adds a new node to the memory, if the node isn't saved already and memory could be allocated.
 * If the node is already in memory, the known information will be overwritten with the new information.
 */
void add_node(const p_node_t node)
{
	//Exit the function early, if the node already exists and the contents got updated with the information of the new node
	p_node_t *search_node = find_node(&(node.addr));
	if (search_node != NULL)
	{
        memcpy(search_node, &node, sizeof(p_node_t));
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
 * \brief Removes the node with the specified rimeaddr from the memory and frees the memory used by the node
 * \param addr Pointer to the rimeaddr
 *
 * Removes the node with the specified rimeaddr from the memory and frees the memory used by the node.
 * If a parameter is NULL, the function immediately returns and does nothing.
 */
void remove_node(const rimeaddr_t *addr)
{
	//Don't do anything if parameters are NULL
	if (addr == NULL)
	{
		return;
	}
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
 * \brief Returns an array of pointers to the saved nodes in memory. The array should neither be freed, nor changed outside of graph.c.
 * \param count A pointer to an uint8 value that will be set to the current amount of saved nodes
 * \retval The pointer to the array of p_node_t*
 *
 * Returns an array of pointers to the saved nodes in memory. The array should neither be freed, nor changed outside of graph.c.
 * The free spaces (NULL pointers) will always be located after the filled spaces.
 * If a parameter is NULL, the function immediately returns NULL.
 */
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

/**
 * \brief Returns a pointer to the node with the specified rimeaddr. If the node does not exist, NULL is returned.
 * \param addr Pointer to the rimeaddr
 * \retval Pointer to the node
 *
 * Returns a pointer to the node with the specified rimeaddr. If the node does not exist, NULL is returned.
 * If a parameter is NULL, the function immediately returns NULL.
 */
p_node_t *find_node(const rimeaddr_t *addr)
{
	//Don't do anything if parameters are NULL
	if (addr == NULL)
	{
		return NULL;
	}
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
 * \brief Returns how many nodes are currently saved
 * \retval The amount of nodes
 */
uint8_t get_node_count()
{
	return node_count;
}

/**************************************************************/
/**************************Edge********************************/
/**************************************************************/


/**
 * \brief Adds a new edge to the memory
 * \param edge The edge to be added to the memory
 *
 * Adds a new edge to the memory, if the edge isn't saved already and memory could be allocated.
 * If the edge is already in memory, the known information will be overwritten with the new information.
 */
void add_edge(const p_edge_t edge)
{
	//Exit the function early, if the edge already exists and the contents got updated with the information of the new edge
	p_edge_t *search_edge = find_edge(&(edge.src), &(edge.dst));
	if (search_edge != NULL)
	{
		memcpy(search_edge, &edge, sizeof(p_edge_t));
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
 * \brief Removes the edge with the two specified rimeaddr from the memory and frees the memory used by the edge
 * \param src Pointer to the rimeaddr of the source node
 * \param dst Pointer to the rimeaddr of the destination node
 *
 * Removes the edge with the two specified rimeaddr from the memory and frees the memory used by the edge.
 * If a parameter is NULL, the function immediately returns and does nothing.
 */
void remove_edge(const rimeaddr_t *src, const rimeaddr_t *dst)
{
	//Don't do anything if parameters are NULL
	if (src == NULL || dst == NULL)
	{
		return;
	}
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
 * \brief Returns an array of pointers to the saved edges in memory. The array should neither be freed, nor changed outside of graph.c.
 * \param count A pointer to an uint8 value that will be set to the current amount of saved edges
 * \retval The pointer to the array of p_edge_t*
 *
 * Returns an array of pointers to the saved edges in memory. The array should neither be freed, nor changed outside of graph.c.
 * The free spaces (NULL pointers) will always be located after the filled spaces.
 * If a parameter is NULL, the function immediately returns NULL.
 */
p_edge_t **get_all_edges(uint8_t *count){
	//Don't do anything if parameters are NULL
	if (count == NULL)
	{
		return NULL;
	}
	*count = edge_count;
	return edge_memory_array;
}

/**
 * \brief Returns an array of pointers to the outgoing edges of the node with the specified rimeaddr. Must be freed.
 * \param src_addr Pointer to the rimeaddr of the node
 * \param count A pointer to an uint8 value that will be set to the amount of outgoing edges
 * \retval Pointer to an array of pointers to edges. Returns NULL if there are no edges.
 *
 * Returns an array of pointers to the outgoing edges of the node with the specified rimeaddr.
 * If no outgoing edges for the node were found, NULL is returned. In this case no free is needed.
 * WARNING: The returned array has to be freed after the function was called.
 * If a parameter is NULL, the function immediately returns NULL.
 */
p_edge_t **get_outgoing_edges(const rimeaddr_t *src_addr, uint8_t *count)
{
	//Don't do anything if parameters are NULL
	if (src_addr == NULL || count == NULL)
	{
		return NULL;
	}

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
	p_edge_t** outgoing_edge_array = (p_edge_t**) malloc(2 * (*count));
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
 * \brief Returns an array of pointers to the ingoing edges of the node with the specified rimeaddr. Must be freed.
 * \param dst_addr Pointer to the rimeaddr of the node
 * \param count A pointer to an uint8 value that will be set to the amount of ingoing edges
 * \retval Pointer to an array of pointers to edges. Returns NULL if there are no edges.
 *
 * Returns an array of pointers to the ingoing edges of the node with the specified rimeaddr
 * If no ingoing edges for the node were found, NULL is returned. In this case no free is needed.
 * WARNING: The returned array has to be freed after the function was called.
 * If a parameter is NULL, the function immediately returns NULL.
 */
p_edge_t **get_ingoing_edges(const rimeaddr_t *dst_addr, uint8_t *count)
{
	//Don't do anything if parameters are NULL
	if (dst_addr == NULL || count == NULL)
	{
		return NULL;
	}

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

	//insert the ingoing edges into the array to be returned
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
 * \brief Returns a pointer to the edge with the source and destination nodes that have the two specified rimeaddr. If it does not exist in the memory, return NULL
 * \param src Pointer to the rimeaddr of the source node
 * \param dst Pointer to the rimeaddr of the destination node
 * \retval A pointer to the edge if it was found, NULL if it wasn't.
 *
 * Returns a pointer to the edge with the source and destination nodes that have the two specified rimeaddr. If it does not exist in the memory, return NULL.
 * If a parameter is NULL, the function immediately returns NULL.
 */
p_edge_t *find_edge(const rimeaddr_t *src, const rimeaddr_t *dst)
{
	//Don't do anything if parameters are NULL
	if (src == NULL || dst == NULL)
	{
		return NULL;
	}

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
 * \brief Returns how many edges are currently saved
 * \retval The amount of edges
 */
uint8_t get_edge_count()
{
	return edge_count;
}
