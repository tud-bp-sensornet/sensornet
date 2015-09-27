#include <stdio.h>

#include "contiki.h"
#include "graph.h"
#include "graph-operations.h"
#include "serialize.h"
#include "pbroadcast.h"
#include "random.h"
#include "neighbor-discovery.h"
#include "cc2420.h"

PROCESS(simple_process, "Simple process");
AUTOSTART_PROCESSES(&simple_process);

void disable_some_nodes()
{
	if (rimeaddr_node_addr.u8[0] % 2 == 0)
	{
		printf("[firmware.c] --- Mote ID is even --> Turning off neighbor-discovery! ---\n");
		process_exit(&neighbor_discovery_process);
	}
}

PROCESS_THREAD(simple_process, ev, data)
{
	PROCESS_BEGIN();
	
	cc2420_set_txpower(12);

	// Timer is set to 10 minutes by default. Test runtime should be ~20 minutes.
	static struct ctimer ct;
	ctimer_set(&ct, CLOCK_SECOND * 60 * 10, disable_some_nodes, NULL);
	
	process_start(&neighbor_discovery_process, NULL);

	printf("K = %d, MAX_NODES = %d, MAX_EDGES = %d, RIMEADDR = %d.%d\n", (int) K, (int) MAX_NODES, (int) MAX_EDGES, rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1]);
	printf("NODE_MEM = %dB, EDGE_MEM = %dB\n", (int)(sizeof(p_node_t) * MAX_NODES), (int)(sizeof(p_edge_t) * MAX_EDGES));
	printf("PACKETBUF_SIZE = %d, TXPOWER = %d, INTERVAL = %d - %d\n", PACKETBUF_SIZE, cc2420_get_txpower(), DISCOVERY_INTERVAL_MIN, DISCOVERY_INTERVAL_MAX);

	PROCESS_END();
}
