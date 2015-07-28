/**
 * \file                       Holds the positions of all nodes
 * \author                     tud-bp-sensornet
 */

#ifndef __POSITIONS_H__
#define __POSITIONS_H__

/**
 * These arrays represent the x and y positions of the nodes in 2D
 */
int16_t pos_x[6] = { 50, 90, 130, 130, 130, 170 };
int16_t pos_y[6] = { 0, 0, 0, 40, -40, -40 };

/**
 * This value holds the destination node
 */
uint8_t node_destination = 0x01;

/**
 * This value holds the number of nodes we have the positions from
 */
uint8_t node_amount = 0x06;

#endif /* __POSITIONS_H__ */
