#include "positions.h"
#include "contiki.h"

/**
 * These arrays represent the x and y positions of the nodes in 2D
 */
int16_t pos_x[6] = {50, 90, 130, 130, 130, 170};
int16_t pos_y[6] = {0, 0, 0, 40, -40, -40};

/**
 * This value holds the destination node
 */
rimeaddr_t node_destination = {{0x04}};

/**
 * This value holds the number of nodes we have the positions from
 */
uint8_t node_amount = 0x06;

position_t get_stored_position_of(const rimeaddr_t *addr)
{
	int16_t x = pos_x[addr->u8[0] - 1];
	int16_t y = pos_y[addr->u8[0] - 1];
	return (position_t){x, y};
}