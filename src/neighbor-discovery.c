#include "neighbor-discovery.h"
#include "contiki.h"
#include "graph.h"
#include "graph-operations.h"
#include "serialize.h"
#include "pbroadcast.h"
#include "random.h"
#include "routing.h"
#include "positions.h"

#ifndef __NEIGHBOR_DISCOVERY_DEBUG__
#define __NEIGHBOR_DISCOVERY_DEBUG__ 0
#endif

#if __NEIGHBOR_DISCOVERY_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#if __NEIGHBOR_DISCOVERY_DEBUG__
void debug_output_current_graph()
{
	int i;
	uint8_t count;

	p_node_t **nodes = get_all_nodes(&count);

	PRINTF("[neighbor-discovery.c] Nodes (%d): ", count);

	for (i = 0; i < count; i++)
	{
		PRINTF("(%d.%d)", nodes[i]->addr.u8[0], nodes[i]->addr.u8[1]);
	}

	p_edge_t **edges = get_all_edges(&count);

	PRINTF(" Edges (%d): ", count);

	for (i = 0; i < count; i++)
	{
		PRINTF("(%d.%d->%d.%d)", edges[i]->src.u8[0], edges[i]->src.u8[1], edges[i]->dst.u8[0], edges[i]->dst.u8[1]);
	}

	PRINTF("\n");
}
#endif

PROCESS(neighbor_discovery_process, "K Hop Neighbor Discovery process");

static struct p_broadcast_conn broadcast;

static void recv(const struct p_broadcast_conn *bc, const rimeaddr_t *sender, const void *data, size_t length)
{
	//Update graph
	deserialize(sender, data, length);
}

static void packet_complete(const void *packet_data, size_t length)
{
	//Broadcast subgraph
	if (p_broadcast_send(&broadcast, packet_data, length) == 0)
	{
		//TODO: Errorhandling
	}
}

PROCESS_THREAD(neighbor_discovery_process, ev, data)
{
	PROCESS_EXITHANDLER(p_broadcast_close(&broadcast));
	PROCESS_BEGIN();

	broadcast.received = recv;

	p_broadcast_open(&broadcast, 129);

	static struct etimer et;
	init_graph();

	//We are root
	p_node_t root = {rimeaddr_node_addr, get_stored_position_of(&rimeaddr_node_addr)};
	add_node(root);

	while (1)
	{
		/* Delay 60-120 seconds */
		etimer_set(&et, CLOCK_SECOND * DISCOVERY_INTERVAL_MIN + random_rand() % (CLOCK_SECOND * DISCOVERY_INTERVAL_MAX));

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		//Purge edges with expired ttl
		purge();

		#if __NEIGHBOR_DISCOVERY_DEBUG__
		debug_output_current_graph();
		#endif

		//Create subgraphs and broadcast them
		serialize(packet_complete);
	}

	PROCESS_END();
}
