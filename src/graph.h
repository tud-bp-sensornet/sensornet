/**
 * \file graph.h
 * Header file for the Information-Pool containing all node and edge information of the graph built by the k-hop algorithm.
 * \author tud-bp-sensornet
 */

#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "contiki.h"
#include "net/rime/rime.h"
#include "positions.h"

/**
 * \brief Needs to be called at the start of main once to initialize the memory.
 */
void init_graph();

/**
 * \struct p_node_t
 * \brief This struct represents a node which is identified by it's linkaddr_t.
 *
 * The nodes are saved to represent the local view of the sensor to his neighbours.
 * It contains information of the node necessary for the routing protocol.
 */
typedef struct
{
	linkaddr_t addr; ///< The linkaddr_t of the node.
	position_t pos; ///< The position_t of the node.
} p_node_t;

/**
 * \brief Adds a new node to the memory.
 * \param node The node to be added to the memory.
 *
 * Adds a new node to the memory, if the node isn't saved already and memory could be allocated.
 * If the node is already in memory, the known information will be overwritten with the new information.
 */
void add_node(const p_node_t node);

/**
 * \brief Removes the node with the specified linkaddr_t from the memory and frees the memory used by the node.
 * \param addr Pointer to the linkaddr_t.
 *
 * Removes the node with the specified linkaddr_t from the memory and frees the memory used by the node.
 * If a parameter is NULL, the function immediately returns and does nothing.
 */
void remove_node(const linkaddr_t *addr);

/**
 * \brief Returns an array of pointers to the saved nodes in memory. The array should neither be freed, nor changed outside of graph.c.
 * \param count A pointer to an uint8 value that will be set to the current amount of saved nodes.
 * \return The pointer to the array of p_node_t pointer.
 *
 * Returns an array of pointers to the saved nodes in memory. The array should neither be freed, nor changed outside of graph.c.
 * The free spaces (NULL pointers) will always be located after the filled spaces.
 * If a parameter is NULL, the function immediately returns NULL.
 */
p_node_t **get_all_nodes(uint8_t *count);

/**
 * \brief Returns how many nodes are currently saved.
 * \return The amount of nodes.
 */
uint8_t get_node_count();

/**
 * \brief Returns a pointer to the node with the specified linkaddr_t. If the node does not exist, NULL is returned.
 * \param addr Pointer to the linkaddr_t.
 * \return Pointer to the node.
 *
 * Returns a pointer to the node with the specified linkaddr_t. If the node does not exist, NULL is returned.
 * If a parameter is NULL, the function immediately returns NULL.
 */
p_node_t *find_node(const linkaddr_t *addr);

/**
 * \struct p_edge_t
 * \brief This struct represents an edge with direction which is identified by the linkaddr_t of it's source node and it's destionation node.
 *
 * The edges are saved to represent the local view of the sensor to his neighbours.
 * It contains information of the edge necessary for the routing protocol.
 * It contains a ttl (in seconds) that is used to delete an edge if there wasn't an update in a certain amount of time.
 */
typedef struct
{
	linkaddr_t src; ///< The source nodes linkaddr_t.
	linkaddr_t dst; ///< The destination nodes linkaddr_t.
	uint16_t ttl; ///< The TTL in seconds.
	uint16_t rssi; ///< The Receive Signal Strength Indicator.
	uint16_t lqi; ///< The Link Quality Indicator.
} p_edge_t;

/**
 * \brief Adds a new edge to the memory.
 * \param edge The edge to be added to the memory.
 *
 * Adds a new edge to the memory, if the edge isn't saved already and memory could be allocated.
 * If the edge is already in memory, the known information will be overwritten with the new information.
 */
void add_edge(const p_edge_t edge);

/**
 * \brief Removes the edge with the two specified linkaddr_t from the memory and frees the memory used by the edge.
 * \param src Pointer to the linkaddr_t of the source node.
 * \param dst Pointer to the linkaddr_t of the destination node.
 *
 * Removes the edge with the two specified linkaddr_t from the memory and frees the memory used by the edge.
 * If a parameter is NULL, the function immediately returns and does nothing.
 */
void remove_edge(const linkaddr_t *src, const linkaddr_t *dst);

/**
 * \brief Returns an array of pointers to the saved edges in memory. The array should neither be freed, nor changed outside of graph.c.
 * \param count A pointer to an uint8 value that will be set to the current amount of saved edges.
 * \return The pointer to the array of p_edge_t pointer.
 *
 * Returns an array of pointers to the saved edges in memory. The array should neither be freed, nor changed outside of graph.c.
 * The free spaces (NULL pointers) will always be located after the filled spaces.
 * If a parameter is NULL, the function immediately returns NULL.
 */
p_edge_t **get_all_edges(uint8_t *count);

/**
 * \brief Returns an array of pointers to the outgoing edges of the node with the specified linkaddr_t. Must be freed.
 * \param src_addr Pointer to the linkaddr_t of the node.
 * \param count A pointer to an uint8 value that will be set to the amount of outgoing edges.
 * \return Pointer to an array of pointers to edges. Returns NULL if there are no edges.
 *
 * Returns an array of pointers to the outgoing edges of the node with the specified linkaddr_t.
 * If no outgoing edges for the node were found, NULL is returned. In this case no free is needed.
 * WARNING: The returned array has to be freed after the function was called.
 * If a parameter is NULL, the function immediately returns NULL.
 */
p_edge_t **get_outgoing_edges(const linkaddr_t *src_addr, uint8_t *count);

/**
 * \brief Returns an array of pointers to the ingoing edges of the node with the specified linkaddr_t. Must be freed.
 * \param dst_addr Pointer to the linkaddr_t of the node.
 * \param count A pointer to an uint8 value that will be set to the amount of ingoing edges.
 * \return Pointer to an array of pointers to edges. Returns NULL if there are no edges.
 *
 * Returns an array of pointers to the ingoing edges of the node with the specified linkaddr_t.
 * If no ingoing edges for the node were found, NULL is returned. In this case no free is needed.
 * WARNING: The returned array has to be freed after the function was called.
 * If a parameter is NULL, the function immediately returns NULL.
 */
p_edge_t **get_ingoing_edges(const linkaddr_t *dst_addr, uint8_t *count);

/**
 * \brief Returns a pointer to the edge with the source and destination nodes that have the two specified linkaddr_t. If it does not exist in the memory, return NULL.
 * \param src Pointer to the linkaddr_t of the source node.
 * \param dst Pointer to the linkaddr_t of the destination node.
 * \return A pointer to the edge if it was found, NULL if it wasn't.
 *
 * Returns a pointer to the edge with the source and destination nodes that have the two specified linkaddr_t. If it does not exist in the memory, return NULL.
 * If a parameter is NULL, the function immediately returns NULL.
 */
p_edge_t *find_edge(const linkaddr_t *src, const linkaddr_t *dst);

/**
 * \brief Returns how many edges are currently saved.
 * \return The amount of edges.
 */
uint8_t get_edge_count();

#endif /* __GRAPH_H__ */
