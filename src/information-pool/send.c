/**
 * \file
 *         Testing the bulk broadcast layer in Rime..
 * \author
 *         tud-bp-sensornet
 */

#include "contiki.h"
#include "net/rime.h"
#include "random.h"

#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "serialize.h"
#include "memory.h"


/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/

static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{

	printf("Got paket: %d size, from %d\n", packetbuf_datalen(), from->u8[0]);

	p_node_t* newroot = deserialize(packetbuf_dataptr());

	iterateUpdate(newroot);
}

/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(example_broadcast_process, ev, data)
{

	static struct etimer et;

	//TODO: Returns system time, all motes have to be started simultaneously
	clock_init();

	PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

	PROCESS_BEGIN();

	broadcast_open(&broadcast, 129, &broadcast_call);

	init_mem();

	//We are root
	root = get_node();
	root->addr = rimeaddr_node_addr;
	root->last_seen = clock_seconds();
	root->edges = NULL;

	while (1) {

		/* Delay 2-4 seconds */
		etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		//clear packet buffer and set packet size
		packetbuf_clear ();

		uint16_t nodes = 1;
		uint16_t edges = 0;

		//TODO: Keep track over our tree size
		void * serializationptr = serialize(root, 3, 10, 10, &nodes, &edges);

		packetbuf_set_datalen (sizeof(p_node_t) * nodes + sizeof(p_edge_t) * edges);
		packetbuf_copyfrom(serializationptr, sizeof(p_node_t) * nodes + sizeof(p_edge_t) * edges);

		broadcast_send(&broadcast);

		free(serializationptr);
	}

	PROCESS_END();
}


