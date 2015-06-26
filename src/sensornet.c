#include "contiki.h"
#include "broadcast.h"
#include "graph.h"
#include "serialze.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(simple_process, "Simple process");
AUTOSTART_PROCESSES(&simple_process);
/*---------------------------------------------------------------------------*/
static void
recv(const struct p_broadcast_conn *bc, const rimeaddr_t *sender, void *data, size_t length)
{
	//Update graph
	deserialize(sender, data, length);
}
/*---------------------------------------------------------------------------*/
static struct p_broadcast_conn broadcast;
broadcast->received = recv;
/*---------------------------------------------------------------------------*/
static void
packet_complete(const void *packet_data, size_t length)
{
	//Broadcast subgraph
	if(p_broadcast_send(broadcast, packet_data, length) == 0){
		//TODO: Errorhandling
	}
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(simple_process, ev, data)
{
	PROCESS_EXITHANDLER(p_broadcast_close(&broadcast);)
	PROCESS_BEGIN();

	p_broadcast_open(&broadcast, 129);
	static struct etimer et;

	//We are root
	p_node_t root = {rimeaddr_node_addr};
	add_node(root);

	printf("K = %d, MAX_NODES = %d, MAX_EDGES = %d\n", (int) K, (int) MAX_NODES, (int) MAX_EDGES);

	while (1) {
		/* Delay 2-4 seconds */
		etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 4));

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		//Create subgraphs and broadcast them
		serialize(packet_complete);
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/