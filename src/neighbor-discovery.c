/**
 * \file neighbor-discovery.c
 * The neighbor discovery process.
 * \author tud-bp-sensornet
 */

#include "neighbor-discovery.h"
#include "contiki.h"
#include "graph.h"
#include "graph-operations.h"
#include "serialize.h"
#include "pbroadcast.h"
#include "random.h"
#include "routing.h"
#include "positions.h"

/**
 * \def __NEIGHBOR_DISCOVERY_DEBUG__
 * \brief Set to 1 to activate debug output.
*/
#ifndef __NEIGHBOR_DISCOVERY_DEBUG__
#define __NEIGHBOR_DISCOVERY_DEBUG__ 0
#endif

//\cond
#if __NEIGHBOR_DISCOVERY_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
//\endcond

/*---------------------------------------------------------------------------*/
#if __NEIGHBOR_DISCOVERY_DEBUG__
void debug_output_current_graph()
{
	int i;
	uint8_t count;

	p_node_t **nodes = get_all_nodes(&count);

	PRINTF("[neighbor-discovery.c] --- Current Graph ---\n");
	PRINTF("[neighbor-discovery.c] Nodes (%d): ", count);

	for (i = 0; i < count; i++)
	{
		if (i >= 40 && i % 40 == 0) PRINTF("[...] \n[neighbor-discovery.c] ^ ");

		PRINTF("(%d.%d)", nodes[i]->addr.u8[0], nodes[i]->addr.u8[1]);
	}

	PRINTF("\n");

	p_edge_t **edges = get_all_edges(&count);

	PRINTF("[neighbor-discovery.c] Edges (%d): ", count);

	for (i = 0; i < count; i++)
	{
		if (i >= 40 && i % 40 == 0) PRINTF("[...] \n[neighbor-discovery.c] ^ ");

		PRINTF("(%d.%d->%d.%d;%u)", edges[i]->src.u8[0], edges[i]->src.u8[1], edges[i]->dst.u8[0], edges[i]->dst.u8[1], edges[i]->ttl);
	}

	PRINTF("\n");
}
#endif
/*---------------------------------------------------------------------------*/
//\cond
PROCESS(neighbor_discovery_process, "K Hop Neighbor Discovery process");
//\endcond
/*---------------------------------------------------------------------------*/
static struct p_broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
static void recv(const struct p_broadcast_conn *bc, const rimeaddr_t *sender, const void *data, size_t length)
{
	//Update graph
	deserialize(sender, data, length);
}
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
//\cond
PROCESS_THREAD(neighbor_discovery_process, ev, data)
{
	PROCESS_EXITHANDLER(p_broadcast_close(&broadcast));
	PROCESS_BEGIN();

	broadcast.received = recv;

	p_broadcast_open(&broadcast, 10000);

	static struct etimer et;
	init_graph();

	//We are root
	p_node_t root = {rimeaddr_node_addr, get_stored_position_of(&rimeaddr_node_addr)};
	add_node(root);

	while (1)
	{
		/* Delay DISCOVERY_INTERVAL_MIN-DISCOVERY_INTERVAL_MAX seconds */
		etimer_set(&et, CLOCK_SECOND * DISCOVERY_INTERVAL_MIN + random_rand() % (CLOCK_SECOND * DISCOVERY_INTERVAL_MAX-DISCOVERY_INTERVAL_MIN));

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		//Purge edges with expired ttl
		purge();

#if __NEIGHBOR_DISCOVERY_DEBUG__
		debug_output_current_graph();
#endif

		//Create subgraphs and broadcast them
		serialize(packet_complete, MAX_BROADCAST_PAYLOAD_SIZE);
	}

	PROCESS_END();
}
//\endcond
/*---------------------------------------------------------------------------*/
