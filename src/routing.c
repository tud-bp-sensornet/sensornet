/**
 * \file                       Routing helper functions - Used by the routing algorithm for path finding
 * \author                     tud-bp-sensornet
 */

#define __ROUTING_DEBUG__ 0

#include "graph.h"
#include "routing.h"
#include "positions.h"

#if __ROUTING_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
rimeaddr_t get_nearest_neighbour()
{

	//Get all edges we know
	uint8_t edge_count = 0;
	p_edge_t **edges = get_all_edges(&edge_count);

	//Iterate over all outgoing edges from root
	uint8_t i;
	rimeaddr_t nearest_neighbour = rimeaddr_null;
	int16_t neighbour_distance = 0x7FFF; //32767 maximum value

	position_t dst_pos = get_stored_position_of(&node_destination);

	for (i = 0; i < edge_count; i++)
	{
		if (rimeaddr_cmp(&(edges[i]->src), &rimeaddr_node_addr))
		{
			position_t neighbor_pos = get_stored_position_of(&(edges[i]->dst));

			//Calculate euklidean distance
			//No need for sqrt, we do not need the exact distance
			int16_t distance = (dst_pos.x - neighbor_pos.x) * (dst_pos.x - neighbor_pos.x) + (dst_pos.y - neighbor_pos.y) * (dst_pos.y - neighbor_pos.y);
			PRINTF("Debug: Distance from %d to %d is: %d\n", node_destination, edges[i]->dst.u8[0], distance);

			if (neighbour_distance > distance)
			{
				nearest_neighbour = edges[i]->dst;
				neighbour_distance = distance;
			}
		}
	}

	PRINTF("Debug: Nearest is: %d\n", nearest_neighbour.u8[0]);
	return nearest_neighbour;
}
/*---------------------------------------------------------------------------*/
