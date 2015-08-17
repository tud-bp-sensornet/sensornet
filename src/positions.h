/**
 * \file                       Holds the positions of all nodes
 * \author                     tud-bp-sensornet
 */
#include "contiki.h"
#include "net/rime.h"

#ifndef __POSITIONS_H__
#define __POSITIONS_H__

/**
 * Represents a position in a two-dimensional system.
 */
typedef struct {
	int16_t x;
	int16_t y;
} position_t;

/**
 * These arrays represent the x and y positions of the nodes in 2D
 */
int16_t pos_x[6];
int16_t pos_y[6];

/**
 * This value holds the destination node
 */
extern rimeaddr_t node_destination;

/**
 * This value holds the number of nodes we have the positions from
 */
extern uint8_t node_amount;

/**
 * \brief Returns the stored position of a node.
 * \param addr Rimeaddr of the node you want to know the position of.
 * \retval The x and y position of the node.
 */
position_t get_stored_position_of(const rimeaddr_t *addr);


#endif /* __POSITIONS_H__ */
