/**
 * \file                       Header file for the routing helper functions
 * \author                     tud-bp-sensornet
 */

#ifndef __ROUTING_H__
#define __ROUTING_H__

#include "rimeaddr.h"

/**
 * \brief           Returns the nearest neighbour of root to the destination node
 * \retval          The rimeaddr_t of the nearest neighbour
 *
 *                  This function returns the rimeaddr_t of the nearest
 *                  neighbour of root to the destination node calculated
 *                  with euclidean distance.
 *
 * 	                Returns rimeaddr_null when no neighbour is found.
 */
rimeaddr_t get_nearest_neighbour();

#endif /* __ROUTER_H__ */
