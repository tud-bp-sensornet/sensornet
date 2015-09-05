#include "positions.h"
#include "contiki.h"

// Array of node positions
#ifndef NODE_POSITION_ARRAY
#define NODE_POSITION_ARRAY {0,0}
#endif

// Length of the array above
#ifndef NODE_POSITION_ARRAY_LENGTH
#define NODE_POSITION_ARRAY_LENGTH 1
#endif

// Smallest rime address of the array above
#ifndef POSITION_ARRAY_RIMEADDR_OFFSET
#define POSITION_ARRAY_RIMEADDR_OFFSET 1
#endif

/**
 * This array represents the x and y positions of the nodes in 2D
 */
position_t pos[NODE_POSITION_ARRAY_LENGTH] = {NODE_POSITION_ARRAY};

position_t get_stored_position_of(const rimeaddr_t *addr)
{
	int16_t rimeaddr_as_int = (addr->u8[1] << 8) + (addr->u8[0]);
	return pos[rimeaddr_as_int - (POSITION_ARRAY_RIMEADDR_OFFSET)];
}