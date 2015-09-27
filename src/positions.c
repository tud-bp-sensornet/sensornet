/**
 * \file positions.c
 * Holds the positions of all nodes.
 * \author tud-bp-sensornet
 */

#include "positions.h"
#include "contiki.h"

/**
 * \brief This array represents the x and y positions of the nodes in 2D
 */
position_t pos[NODE_POSITION_ARRAY_LENGTH] = {NODE_POSITION_ARRAY};

/*---------------------------------------------------------------------------*/
position_t get_stored_position_of(const linkaddr_t *addr)
{
	int16_t linkaddr_as_int = (addr->u8[1] << 8) + (addr->u8[0]);
	return pos[linkaddr_as_int - (POSITION_ARRAY_RIMEADDR_OFFSET)];
}
/*---------------------------------------------------------------------------*/
