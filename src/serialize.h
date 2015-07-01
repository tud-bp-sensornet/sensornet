/**
 * \file                       Header file for a serializer and deserializer
 * \author                     tud-bp-sensornet
 */

#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

/**
 * \brief			            Serializes the graph (local view) into several (sub) graphs (up to K-1 hops)
 * \param packet_complete		A function pointer which will be called if a (sub) graph was serialized
 * \param packet_data           A pointer to the memory where the (sub) graph is stored
 * \param length                The length of the serialized (sub) graph
 *
 *                              This function serializes our local view graph into several (sub) graphs.
 *                              It will not serialize nodes and eges in more than K-1 hops distance.
 *                              The called funtion behind the function pointer must not free the memory
 *                              pointed by packet_data.
 *                              Serialized (sub) graphs are stored with this layout:
 *                              p_node_t src, (p_edge_t src_drain, p_node_t drain)*
 *                              If a src has too many edges another packet will be serialized containing
 *                              the remaining edges and drains.
 */
void serialize(void (*packet_complete)(const void *packet_data, size_t length));

/**
 * \brief			            Deserializes the serialized (sub) graph and updates the graph (local view)
 * \param sender		        A pointer to the sender of the serialized package
 * \param packet                A pointer to the memory where the serialized (sub) graph is stored
 * \param length                The length of the serialized (sub) graph
 *
 *                              This function deserializes the (sub) graph and updates our local view graph.
 *                              This function will not free the memory pointed by packet nor change the sender.
 */
void deserialize(const rimeaddr_t *sender, void *packet, size_t length);

#endif /* __SERIALIZE_H__ */
