#include <stdio.h>

#include "contiki.h"
#include "graph.h"
#include "graph-operations.h"
#include "serialize.h"
#include "pbroadcast.h"
#include "random.h"

#ifndef __K_HOP_DEBUG__
#define __K_HOP_DEBUG__ 0
#endif

/*---------------------------------------------------------------------------*/
PROCESS(simple_process, "Simple process");
AUTOSTART_PROCESSES(&simple_process);
/*---------------------------------------------------------------------------*/
static void
recv(const struct p_broadcast_conn *bc, const rimeaddr_t *sender, const void *data, size_t length)
{
	//Update graph
	deserialize(sender, data, length);
}
/*---------------------------------------------------------------------------*/
static struct p_broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
#if __K_HOP_DEBUG__
/**
 * Prints the graph by printing Node informations and Edge informations while iterating it
 * Testcase:Node:%d,%d\n	with the rimeaddr.u8[0] and the depth
 * Testcase:Edge:%d,%d\n	with the rimeaddr.u8[0] of the source and the rimeaddr.u8[0] of the drain
 */
static void debug_k_hop_timer_event(void *ptr){
	printf("Print Graph for Node %d\n", rimeaddr_node_addr.u8[0]);
	uint8_t count = 0;

	uint8_t i = 0;
	printf("Saved Nodes:%d\n", get_node_count());
	printf("Saved Edges:%d\n", get_edge_count());

	//print nodes
	p_hop_t *hops = get_hop_counts(&count);
	printf("Count Node hops:%d\n", count);
	//also print root
	printf("Testcase:Node:%d,%d\n", rimeaddr_node_addr.u8[0],0);
	for (i = 0; i < count; i++)
	{
		printf("Testcase:Node:%d,%d\n", hops[i].addr.u8[0],hops[i].hop_count);
	}
	free(hops);

	//print edges
	p_edge_t **edge_array = get_all_edges(&count);
	printf("Count Edge:%d\n", count);
	for (i = 0; i < count; i++)
	{
		printf("Testcase:Edge:%d,%d\n", edge_array[i]->src.u8[0], edge_array[i]->dst.u8[0]);
	}
	free(edge_array);
}
#endif
/*---------------------------------------------------------------------------*/
static void
packet_complete(const void *packet_data, size_t length)
{
	//Broadcast subgraph
	if(p_broadcast_send(&broadcast, packet_data, length) == 0){
		//TODO: Errorhandling
	}
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(simple_process, ev, data)
{
	PROCESS_EXITHANDLER(p_broadcast_close(&broadcast);)
	PROCESS_BEGIN();

	broadcast.received = recv;

	p_broadcast_open(&broadcast, 129);
	static struct etimer et;
	init_graph();
	
#if __K_HOP_DEBUG__
		static struct ctimer ct;
		//After 45 seconds, call debug_k_hop_timer_event
		ctimer_set(&ct, CLOCK_SECOND * 45, debug_k_hop_timer_event, NULL);
#endif

	//We are root
	p_node_t root = {rimeaddr_node_addr};
	add_node(root);

	printf("K = %d, MAX_NODES = %d, MAX_EDGES = %d\n", (int) K, (int) MAX_NODES, (int) MAX_EDGES);

	while (1) {
		/* Delay 2-4 seconds */
		etimer_set(&et, CLOCK_SECOND * 2 + random_rand() % (CLOCK_SECOND * 4));

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		//Purge edges with expired ttl
		purge();

		//Create subgraphs and broadcast them
		serialize(packet_complete);
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
