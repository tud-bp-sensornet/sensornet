#include "contiki.h"
#include "graph.h"
#include "graph-operations.h"
#include "serialize.h"
#include "pbroadcast.h"
#include "random.h"
#include "routing.h"
#include "positions.h"
#include "neighbor-discovery.h"

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

PROCESS(routing_process, "Routing process");
AUTOSTART_PROCESSES(&routing_process);

static struct unicast_conn uc;

static void recv_uc(struct unicast_conn *c, const rimeaddr_t *from)
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

static const struct unicast_callbacks unicast_callbacks = {recv_uc};

PROCESS_THREAD(routing_process, ev, data)
{
	process_start(&neighbor_discovery_process, NULL);

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