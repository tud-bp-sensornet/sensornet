/**
 * \file                       Routing helper functions - Used by the routing algorithm for path finding
 * \author                     tud-bp-sensornet
 */

#ifndef __ROUTING_DEBUG__
#define __ROUTING_DEBUG__ 0
#endif

#include "graph.h"
#include "routing.h"
#include "positions.h"
#include "net/rime/stunicast.h"
#include "crc16.h"

#if __ROUTING_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

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
			PRINTF("get_nearest_neighbour: Distance from %d to %d is: %d\n", node_destination.u8[0], edges[i]->dst.u8[0], distance);

			if (neighbour_distance > distance)
			{
				nearest_neighbour = edges[i]->dst;
				neighbour_distance = distance;
			}
		}
	}

	PRINTF("get_nearest_neighbour: Nearest is: %d\n", nearest_neighbour.u8[0]);
	return nearest_neighbour;
}

/*------------------------------------------------*/

static uint16_t packet_hash(const void *data, size_t length)
{
	return crc16_data(data, length, 0) & 0xFFFF;
}

static struct runicast_conn uc;
static void (*router_callback)(const void *packet_data, size_t length);
static uint16_t last_received_packet_hash;

static void recv_uc(struct runicast_conn *c, const rimeaddr_t *from, uint8_t seqno)
{
	// runicast will spam the receiver until it receives an acknowledgement
	// this means that we may receive multiples of the same message in the meantime
	// hash the packet and ignore it if we've already received it
	uint16_t current_hash = packet_hash(packetbuf_dataptr(), packetbuf_datalen());
	if (last_received_packet_hash == current_hash)
	{
		return;
	}
	last_received_packet_hash = current_hash;

	PRINTF("Got unicast from: %d Content: %s\n", from->u8[0], (char *)packetbuf_dataptr());

	//Are we the destination?
	rimeaddr_t dst = rimeaddr_null;
	dst.u8[0] = node_destination.u8[0];
	if (!rimeaddr_cmp(&rimeaddr_node_addr, &dst))
	{
		//Forward message
		packetbuf_copyfrom(packetbuf_dataptr(), packetbuf_datalen());
		rimeaddr_t receiver = get_nearest_neighbour();
		if (!rimeaddr_cmp(&receiver, &rimeaddr_null))
		{
			PRINTF("Will forward to: %d\n", receiver.u8[0]);
			runicast_send(&uc, &receiver, 255);
		}
	}
	else
	{
		PRINTF("Got it!\n");

		if (router_callback != NULL)
		{
			router_callback(packetbuf_dataptr(), packetbuf_datalen());
		}
	}
}

static const struct runicast_callbacks runicast_callbacks = {recv_uc};

void init_router(void (*message_received)(const void *packet_data, size_t length))
{
	runicast_open(&uc, 146, &runicast_callbacks);
}

void close_router()
{
	runicast_close(&uc);
}

int8_t send_message(const void *packet_data, size_t length)
{
	packetbuf_copyfrom(packet_data, length);
	rimeaddr_t receiver = get_nearest_neighbour();

	if (!rimeaddr_cmp(&receiver, &rimeaddr_null))
	{
		PRINTF("Will send initial unicast to: %d\n", receiver.u8[0]);
		runicast_send(&uc, &receiver, 255);
		return 1;
	}
	else
	{
		PRINTF("No neighbour to send to\n");
		return 0;
	}
}