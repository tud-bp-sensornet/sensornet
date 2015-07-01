/**
 * \file Header file for the graph operations containing all help functions working on the graph
 * \author tud-bp-sensornet
 */

#ifndef __GRAPH_OPERATIONS_H__
#define __GRAPH_OPERATIONS_H__

#include "graph.h"
#include "contiki.h"

/**
 * This struct represents an association between a node and it's hop count to root (this node)
 * The node is represented with its rimeaddr.
 */
typedef struct {
    rimeaddr_t addr;
    uint8_t hop_count;
} p_hop_t;

/**
 * \brief Returns an array of p_hop_t with the shortest hopcount for every node to root (excluding root)
 * \param count A pointer to an uint8 value that will be set to the current amount of reachable nodes from root
 * \retval The pointer to the array of p_hop_t
 *
 * Returns an array of p_hop_t. The array should be freed, by the caller.
 * If count is a NULL pointer, the function will return NULL.
 * If there is no hop reachable from root, the returned pointer must be freed, too.
 */
p_hop_t *get_hop_counts(uint8_t *count);

/**
 * \brief Deletes all edges with a ttl <= 0 and decrements all remaining edges by the time difference this function was called.
 * \param void
 * \retval void
 *
 * Deleting nodes is TBD
 */
void purge();

#endif /* __GRAPH_OPERATIONS_H__ */
