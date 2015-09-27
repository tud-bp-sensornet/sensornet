#include <stdio.h>

#include "contiki.h"
#include "graph.h"
#include "graph-operations.h"
#include "serialize.h"
#include "pbroadcast.h"
#include "random.h"
#include "neighbor-discovery.h"
#include "cc2420.h"

#if __NEIGHBOR_DISCOVERY_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

PROCESS(simple_process, "Simple process");
AUTOSTART_PROCESSES(&simple_process);

void debug_output_current_graph1()
{
	int i;
	uint8_t count;

	p_node_t **nodes = get_all_nodes(&count);

	PRINTF("[firmware.c] --- Current Graph ---\n");
	PRINTF("[firmware.c] Nodes (%d): ", count);

	for (i = 0; i < count; i++)
	{
		if (i >= 40 && i % 40 == 0) PRINTF("[...] \n[firmware.c] ^ ");

		PRINTF("(%d.%d)", nodes[i]->addr.u8[0], nodes[i]->addr.u8[1]);
	}

	PRINTF("\n");

	p_edge_t **edges = get_all_edges(&count);

	PRINTF("[firmware.c] Edges (%d): ", count);

	for (i = 0; i < count; i++)
	{
		if (i >= 40 && i % 40 == 0) PRINTF("[...] \n[firmware.c] ^ ");

		PRINTF("(%d.%d->%d.%d)", edges[i]->src.u8[0], edges[i]->src.u8[1], edges[i]->dst.u8[0], edges[i]->dst.u8[1]);
	}

	PRINTF("\n");
}

PROCESS_THREAD(simple_process, ev, data)
{
	PROCESS_BEGIN();
	
	cc2420_set_txpower(8);

	static struct ctimer ct;
	ctimer_set(&ct, CLOCK_SECOND * 60, debug_output_current_graph1, NULL);
	
	process_start(&neighbor_discovery_process, NULL);

	printf("K = %d, MAX_NODES = %d, MAX_EDGES = %d, linkaddr = %d.%d\n", (int) K, (int) MAX_NODES, (int) MAX_EDGES, linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
	printf("NODE_MEM = %dB, EDGE_MEM = %dB\n", (int)(sizeof(p_node_t) * MAX_NODES), (int)(sizeof(p_edge_t) * MAX_EDGES));
	printf("PACKETBUF_SIZE = %d, TXPOWER = %d, INTERVAL = %d - %d\n", PACKETBUF_SIZE, cc2420_get_txpower(), DISCOVERY_INTERVAL_MIN, DISCOVERY_INTERVAL_MAX);

	PROCESS_END();
}
