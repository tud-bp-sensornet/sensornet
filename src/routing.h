/**
 * \file routing.h
 * Header file for the routing helper functions.
 * \author tud-bp-sensornet
 */

#ifndef __ROUTING_H__
#define __ROUTING_H__

#include <stdlib.h>
#include "contiki.h"
#include "linkaddr.h"

/**
 * \def NEIGHBOR_DISCOVERY_CHANNEL
 * \brief Rime channel routing operates on.
*/
#ifndef ROUTING_CHANNEL
#define ROUTING_CHANNEL 146
#endif

/**
 * \brief Returns the nearest neighbour of root to the provided node.
 * \param dst The node the nearest neighbour should be found to.
 * \return The linkaddr_t of the nearest neighbour.
 *
 * This function returns the linkaddr_t of the nearest
 * neighbour of root to the provided node calculated
 * with (quadratic) euclidean distance.
 *
 * Returns linkaddr_null when no neighbour is found.
 */
linkaddr_t get_nearest_neighbour(linkaddr_t *dst);

/**
 * \brief Initializes the message router.
 * \param message_received Callback function that is called when a message was received that was intended for the current node.
 */
void init_router(void (*message_received)(const void *packet_data, size_t length));

/**
 * \brief Closes the router and cleans up used resources.
 */
void close_router();

/**
 * \brief Sends a message to another node using the routing functionality.
 * \param packet_data Data to be sent.
 * \param length Length in bytes of packet_data.
 * \param dst The final destination node.
 * \return Returns 1 if message was sent successfully, 0 if it wasn't.
 *
 * Note that we can not guarantee that the the message arrives beyond
 * the next hop. Sending will fail if the router was not initialized
 * beforehand, if there is no neighbor to send the message to, or if
 * the position of the target node is unknown.
 */
int8_t send_message(const void *packet_data, size_t length, linkaddr_t *dst);

#endif /* __ROUTING_H__ */
