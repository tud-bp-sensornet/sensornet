/**
 * \file                       Holds the positions of all nodes
 * \author                     tud-bp-sensornet
 */
#include "contiki.h"

#ifndef __POSITIONS_H__
#define __POSITIONS_H__

/**
 * These arrays represent the x and y positions of the nodes in 2D
 */
extern int16_t pos_x[];
extern int16_t pos_y[];

/**
 * This value holds the destination node
 */
extern uint8_t node_destination;

/**
 * This value holds the number of nodes we have the positions from
 */
extern uint8_t node_amount;

#endif /* __POSITIONS_H__ */
