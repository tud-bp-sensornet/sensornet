/**
 * \file routing.c
 * Routing helper functions - used by the routing algorithm for path finding.
 * \author tud-bp-sensornet
 */

/**
 * \def __ROUTING_DEBUG__
 * \brief Set to 1 to activate debug output.
*/
#ifndef __ROUTING_DEBUG__
#define __ROUTING_DEBUG__ 0
#endif

#include <string.h>

#include "graph.h"
#include "routing.h"
#include "positions.h"
#include "net/rime/stunicast.h"
#include "crc16.h"

//\cond
#if __ROUTING_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
//\endcond

/*---------------------------------------------------------------------------*/
rimeaddr_t get_nearest_neighbour(rimeaddr_t *dst)
{
	//Get all edges we know
	uint8_t edge_count = 0;
	p_edge_t **edges = get_all_edges(&edge_count);

	//Iterate over all outgoing edges from root
	uint8_t i;
	rimeaddr_t nearest_neighbour = rimeaddr_null;
	uint32_t neighbour_distance = 0xFFFFFFFF;

	position_t dst_pos = get_stored_position_of(dst);

	for (i = 0; i < edge_count; i++)
	{
		if (rimeaddr_cmp(&(edges[i]->src), &rimeaddr_node_addr))
		{
			p_node_t *node = find_node(&(edges[i]->dst));

			if (node == NULL)
			{
				PRINTF("[routing.c] get_nearest_neighbour: Could not find node!\n");
				continue;
			}

			position_t neighbor_pos = node->pos;

			//Calculate euklidean distance
			//No need for sqrt, we do not need the exact distance
			int32_t xdiff = dst_pos.x - neighbor_pos.x;
			int32_t ydiff = dst_pos.y - neighbor_pos.y;
			uint32_t xdiff_squared = xdiff * xdiff;
			uint32_t ydiff_squared = ydiff * ydiff;
			uint32_t distance = xdiff_squared + ydiff_squared;

			PRINTF("[routing.c] get_nearest_neighbour: Distance from %d.%d to %d.%d is: %"PRIu32"\n",
			       dst->u8[0], dst->u8[1], edges[i]->dst.u8[0], edges[i]->dst.u8[1], distance);

			if (neighbour_distance > distance)
			{
				nearest_neighbour = edges[i]->dst;
				neighbour_distance = distance;
			}
		}
	}

	PRINTF("[routing.c] get_nearest_neighbour: Nearest is: %d.%d\n", nearest_neighbour.u8[0], nearest_neighbour.u8[1]);
	return nearest_neighbour;
}
/*------------------------------------------------*/
static uint16_t packet_hash(const rimeaddr_t *sender, const void *data, size_t length)
{
	unsigned short hash = crc16_data(data, length, 0) & 0xFFFF;
	hash = crc16_add(sender->u8[0], hash);
	hash = crc16_add(sender->u8[1], hash);
	return hash & 0xFFFF;
}
/*---------------------------------------------------------------------------*/
static struct runicast_conn uc;
static void (*router_callback)(const void *packet_data, size_t length);
static uint16_t last_received_packet_hash;
/*---------------------------------------------------------------------------*/
static void recv_uc(struct runicast_conn *c, const rimeaddr_t *from, uint8_t seqno)
{
	// runicast will spam the receiver until it receives an acknowledgement
	// this means that we may receive multiples of the same message in the meantime
	// hash the packet and ignore it if we've already received it
	uint16_t current_hash = packet_hash(from, packetbuf_dataptr(), packetbuf_datalen());
	if (last_received_packet_hash == current_hash)
	{
		return;
	}
	last_received_packet_hash = current_hash;

	PRINTF("[routing.c] Got unicast from: %d.%d Content: %s\n", from->u8[0], from->u8[1], (char *)(packetbuf_dataptr() + sizeof(rimeaddr_t)));

	//Are we the destination?
	rimeaddr_t dst = *(rimeaddr_t *)packetbuf_dataptr();
	if (!rimeaddr_cmp(&rimeaddr_node_addr, &dst))
	{
		//Forward message
		packetbuf_copyfrom(packetbuf_dataptr(), packetbuf_datalen());
		rimeaddr_t receiver = get_nearest_neighbour(&dst);
		if (!rimeaddr_cmp(&receiver, &rimeaddr_null))
		{
			PRINTF("[routing.c] Will forward to: %d.%d\n", receiver.u8[0], receiver.u8[1]);

			uint8_t tries = 0;
			while (!runicast_send(&uc, &receiver, 255))
			{
				tries++;
				PRINTF("[routing.c] runicast_send failed! Retrying (%d) ...\n", tries);
			}
		}
	}
	else
	{
		PRINTF("[routing.c] Got it!\n");

		if (router_callback != NULL)
		{
			router_callback(packetbuf_dataptr() + sizeof(rimeaddr_t), packetbuf_datalen() - sizeof(rimeaddr_t));
		}
	}
}
/*---------------------------------------------------------------------------*/
static const struct runicast_callbacks runicast_callbacks = {recv_uc};
/*---------------------------------------------------------------------------*/
void init_router(void (*message_received)(const void *packet_data, size_t length))
{
	runicast_open(&uc, 146, &runicast_callbacks);
	router_callback = message_received;
}
/*---------------------------------------------------------------------------*/
void close_router()
{
	runicast_close(&uc);
}
/*---------------------------------------------------------------------------*/
int8_t send_message(const void *packet_data, size_t length, rimeaddr_t *dst)
{
	//Set packetbuf to message length + destination rimeaddr length
	packetbuf_set_datalen(sizeof(rimeaddr_t) + length);
	//Copy rimeaddr_t
	memcpy(packetbuf_dataptr(), dst, sizeof(rimeaddr_t));
	//Copy message
	memcpy(packetbuf_dataptr() + sizeof(rimeaddr_t), packet_data, length);

	rimeaddr_t receiver = get_nearest_neighbour(dst);

	if (!rimeaddr_cmp(&receiver, &rimeaddr_null))
	{
		PRINTF("[routing.c] Will send initial unicast to: %d\n", receiver.u8[0]);
		runicast_send(&uc, &receiver, 255);
		return 1;
	}
	else
	{
		PRINTF("[routing.c] No neighbour to send to\n");
		return 0;
	}
}
/*---------------------------------------------------------------------------*/
