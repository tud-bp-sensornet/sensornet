#include <stdio.h>

#include "contiki.h"
#include "graph.h"
#include "graph-operations.h"
#include "serialize.h"
#include "pbroadcast.h"
#include "random.h"
#include "routing.h"
#include "positions.h"

#include "dev/button-sensor.h"

#ifndef __ROUTING_THREAD_DEBUG__
#define __ROUTING_THREAD_DEBUG__ 0
#endif

#if __ROUTING_THREAD_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
PROCESS(neighbor_discovery_process, "K-Hop process");
PROCESS(routing_process, "Routing process");
AUTOSTART_PROCESSES(&neighbor_discovery_process, &routing_process);
/*---------------------------------------------------------------------------*/
static struct p_broadcast_conn broadcast;
static struct unicast_conn uc;
/*---------------------------------------------------------------------------*/
static void
recv(const struct p_broadcast_conn *bc, const rimeaddr_t *sender, const void *data, size_t length)
{
	//Update graph
	deserialize(sender, data, length);
}
static void
recv_uc(struct unicast_conn *c, const rimeaddr_t *from)
{
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
			unicast_send(&uc, &receiver);
		}
	}
	else
	{
		PRINTF("Got it!\n");
		//Do something with the message
	}
}
/*---------------------------------------------------------------------------*/
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
/*---------------------------------------------------------------------------*/
static void packet_complete(const void *packet_data, size_t length)
{
	//Broadcast subgraph
	if (p_broadcast_send(&broadcast, packet_data, length) == 0)
	{
		//TODO: Errorhandling
	}
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(neighbor_discovery_process, ev, data)
{
	PROCESS_EXITHANDLER(p_broadcast_close(&broadcast));
	PROCESS_BEGIN();

	broadcast.received = recv;

	p_broadcast_open(&broadcast, 129);

	static struct etimer et;
	init_graph();

	//We are root
	p_node_t root = {rimeaddr_node_addr};
	add_node(root);

	while (1)
	{
		/* Delay 60-120 seconds */
		etimer_set(&et, CLOCK_SECOND * 60 + random_rand() % (CLOCK_SECOND * 120));

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		//Purge edges with expired ttl
		purge();

		//Create subgraphs and broadcast them
		serialize(packet_complete);
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(routing_process, ev, data)
{
	PROCESS_EXITHANDLER(unicast_close(&uc));
	PROCESS_BEGIN();
	SENSORS_ACTIVATE(button_sensor);

	unicast_open(&uc, 146, &unicast_callbacks);

	//When button is pressed send unicast
	while (1)
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

		packetbuf_copyfrom("Fire!", 5);
		rimeaddr_t receiver = get_nearest_neighbour();
		if (!rimeaddr_cmp(&receiver, &rimeaddr_null))
		{
			PRINTF("Will send initial unicast to: %d\n", receiver.u8[0]);
			unicast_send(&uc, &receiver);
		}
		else
		{
			PRINTF("No neighbour to send to\n");
		}
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
