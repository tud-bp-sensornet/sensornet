/**
 * \file graph-operations.h
 * Header file for the graph operations containing all help functions working on the graph.
 * \author tud-bp-sensornet
 */

#ifndef __GRAPH_OPERATIONS_H__
#define __GRAPH_OPERATIONS_H__

#include "graph.h"
#include "contiki.h"

/**
 * \struct p_hop_t
 * \brief This struct represents an association between a node and it's hop count to root (this node).
 *
 * The node is represented with its rimeaddr_t.
 */
typedef struct
{
	rimeaddr_t addr; ///< The rimeaddr-t of the node.
	uint8_t hop_count; ///< The hop count to root.
} p_hop_t;

/**
 * \brief Returns an array of p_hop_t with the shortest hopcount for every node to root (excluding root) traversing the graph as if edges are not directed.
 * \param count A pointer to an uint8 value that will be set to the current amount of reachable nodes (ignoring edge direction) from root.
 * \return The pointer to the array of p_hop_t.
 *
 * Returns an array of p_hop_t. The array should be freed, by the caller.
 * Array layout:
 * Nodes are sorted by its hopcount to root.
 * (node with 1 hop)+, (node with 2 hops)+, ..., (node with n-1 hops)+, (node with n hops)+
 * with n is the longest shortest path from root.
 * If count is a NULL pointer or the graph has no edges or nodes, the function will return NULL.
 * If there is no hop reachable from root, the returned pointer must be freed, too.
 * If no memory for the array could be allocated, the function will return NULL.
 */
p_hop_t *get_hop_counts(uint8_t *count);

/**
 * \brief Deletes all edges with a ttl <= 0 and decrements all remaining edges by the time difference this function was last called.
 *
 * As ttl is in minutes, purge will accumulate differences of seconds until >= 1 minute has passed before decrementing the ttl.
 * Do not call this function with a frequency of < 1 second.
 * Must be called at minimum once before a clock overflow happens.
 * Multiple clock overflows without calling purge will result in undefined behaviour when purge is called.
 * Will delete a node iff node has no ingoing and outgoing edges.
 * Root will never be deleted.
 */
void purge();

#endif /* __GRAPH_OPERATIONS_H__ */
