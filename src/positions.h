/**
 * \file positions.h
 * Holds the positions of all nodes.
 * \author tud-bp-sensornet
 */

#include "contiki.h"
#include "net/rime/rime.h"

#ifndef __POSITIONS_H__
#define __POSITIONS_H__

/**
 * \def NODE_POSITION_ARRAY
 * \brief Array of node positions.
*/
#ifndef NODE_POSITION_ARRAY
#define NODE_POSITION_ARRAY {0,0}
#endif

/**
 * \def NODE_POSITION_ARRAY_LENGTH
 * \brief Length of the array above.
*/
#ifndef NODE_POSITION_ARRAY_LENGTH
#define NODE_POSITION_ARRAY_LENGTH 1
#endif

/**
 * \def POSITION_ARRAY_linkaddr_OFFSET
 * \brief Smallest rime address of the array above.
*/
#ifndef POSITION_ARRAY_linkaddr_OFFSET
#define POSITION_ARRAY_linkaddr_OFFSET 1
#endif

/**
 * \struct position_t
 * \brief Represents a position in a two-dimensional system.
 */
typedef struct
{
	int16_t x; ///< The x position.
	int16_t y; ///< The y position.
} position_t;

/**
 * \brief      Returns the stored position of a node.
 * \param addr linkaddr_t of the node you want to know the position of.
 * \return     The x and y position of the node.
 */
position_t get_stored_position_of(const linkaddr_t *addr);

#endif /* __POSITIONS_H__ */
