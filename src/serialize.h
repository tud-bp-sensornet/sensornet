/**
 * \file                       Header file for a serializer and deserializer
 * \author                     tud-bp-sensornet
 */

#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

#include <stdlib.h>

#include "rimeaddr.h"

/**
 * \brief                       Serializes the graph (local view) into several (sub) graphs (up to K-1 hops)
 * \param packet_complete       A function pointer which will be called if a (sub) graph was serialized
 * \param packet_data           A pointer to the memory where the (sub) graph is stored
 * \param length                The length of the serialized (sub) graph
 * \param max_packet_length     Maximum length of a packet produces by this function. Note that this length must
 *                              be between the length of a minimal subgraph (two nodes, one edge) and PACKETBUF_SIZE.
 *
 *                              This function serializes our local view graph into several (sub) graphs.
 *                              It will not serialize nodes and eges in more than K-1 hops distance and
 *                              not the edges between K-1 nodes.
 *                              The called funtion behind the function pointer must not free the memory
 *                              pointed by packet_data.
 *                              Serialized (sub) graphs are stored with this layout:
 *                              p_node_t src, (p_edge_t src_drain, p_node_t drain)*
 *                              If a src has too many edges another packet will be serialized containing
 *                              the remaining edges and drains.
 *                              If minimal package length (2 Nodes and one Edge) is bigger than PACKETBUF_SIZE
 *                              , K == 0, no memory space could be allocated or the graph is malformed the
 *                              function will return and the function behind the function pointer will
 *                              not get called.
 *
 */
void serialize(void (*packet_complete)(const void *packet_data, size_t length), size_t max_packet_length);

/**
 * \brief                       Deserializes the serialized (sub) graph and updates the graph (local view)
 * \param sender                A pointer to the sender of the serialized package
 * \param packet                A pointer to the memory where the serialized (sub) graph is stored
 * \param length                The length of the serialized (sub) graph
 *
 *                              This function deserializes the (sub) graph and updates our local view graph.
 *                              This function will not free the memory pointed by packet nor change the sender.
 *                              Function will create a new edge to the neighbour we got the informations from.
 *
 */
void deserialize(const rimeaddr_t *sender, const void *packet, size_t length);

#endif /* __SERIALIZE_H__ */
