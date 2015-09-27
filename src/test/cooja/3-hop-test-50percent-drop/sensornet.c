#include <stdio.h>

#include "contiki.h"
#include "graph.h"
#include "graph-operations.h"
#include "serialize.h"
#include "pbroadcast.h"
#include "random.h"
#include "neighbor-discovery.h"

PROCESS(simple_process, "Simple process");
AUTOSTART_PROCESSES(&simple_process);

/**
 * Prints the graph by printing Node informations and Edge informations while iterating it
 * Testcase:Node:%d,%d\n	with the linkaddr.u8[0] and the depth
 * Testcase:Edge:%d,%d\n	with the linkaddr.u8[0] of the source and the linkaddr.u8[0] of the drain
 */
static void debug_k_hop_timer_event(void *ptr)
{
	printf("Print Graph for Node %d\n", linkaddr_node_addr.u8[0]);
	uint8_t count = 0;

	uint8_t i = 0;
	printf("Saved Nodes:%d\n", get_node_count());
	printf("Saved Edges:%d\n", get_edge_count());

	//print nodes
	p_hop_t *hops = get_hop_counts(&count);
	printf("Count Node hops:%d\n", count);
	//also print root
	printf("Testcase:Node:%d,%d\n", linkaddr_node_addr.u8[0], 0);
	for (i = 0; i < count; i++)
	{
		printf("Testcase:Node:%d,%d\n", hops[i].addr.u8[0], hops[i].hop_count);
	}
	free(hops);

	//print edges
	p_edge_t **edge_array = get_all_edges(&count);
	printf("Count Edge:%d\n", count);
	for (i = 0; i < count; i++)
	{
		printf("Testcase:Edge:%d,%d\n", edge_array[i]->src.u8[0], edge_array[i]->dst.u8[0]);
	}
}

PROCESS_THREAD(simple_process, ev, data)
{
	PROCESS_BEGIN();
	
	//After 60 seconds, call debug_k_hop_timer_event
	static struct ctimer ct;
	ctimer_set(&ct, CLOCK_SECOND * 60, debug_k_hop_timer_event, NULL);
	
	process_start(&neighbor_discovery_process, NULL);

	printf("K = %d, MAX_NODES = %d, MAX_EDGES = %d\n", (int) K, (int) MAX_NODES, (int) MAX_EDGES);

	PROCESS_END();
}
