#include <stdio.h>

#include "contiki.h"
#include "graph.h"
#include "graph-operations.h"
#include "serialize.h"
#include "pbroadcast.h"
#include "random.h"
#include "neighbor-discovery.h"
#include "cc2420.h"
#include "routing.h"

#if __NEIGHBOR_DISCOVERY_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

PROCESS(simple_process, "Simple process");
AUTOSTART_PROCESSES(&simple_process);

void message_received(const void *packet_data, size_t length)
{
	printf("[firmware.c] Received msg: %s\n", (char*)packet_data);
}

void send_message_after_3_minutes()
{
	printf("[firmware.c] 3 minutes have passed!\n");

	// If we're node 38, send msg to 33!
	if (rimeaddr_node_addr.u8[0] == 38)
	{
		rimeaddr_t dst = {{33,0}};
		send_message("Feuer!", 7, &dst);
		printf("[firmware.c] Sent message!\n");
	}
}

PROCESS_THREAD(simple_process, ev, data)
{
	PROCESS_BEGIN();
	
	cc2420_set_txpower(8);
	init_router(&message_received);

	static struct ctimer ct;
	ctimer_set(&ct, CLOCK_SECOND * 60 * 3, send_message_after_3_minutes, NULL);
	
	process_start(&neighbor_discovery_process, NULL);

	printf("K = %d, MAX_NODES = %d, MAX_EDGES = %d, RIMEADDR = %d.%d\n", (int) K, (int) MAX_NODES, (int) MAX_EDGES, rimeaddr_node_addr.u8[0], rimeaddr_node_addr.u8[1]);
	printf("NODE_MEM = %dB, EDGE_MEM = %dB\n", (int)(sizeof(p_node_t) * MAX_NODES), (int)(sizeof(p_edge_t) * MAX_EDGES));
	printf("PACKETBUF_SIZE = %d, TXPOWER = %d, INTERVAL = %d - %d\n", PACKETBUF_SIZE, cc2420_get_txpower(), DISCOVERY_INTERVAL_MIN, DISCOVERY_INTERVAL_MAX);

	PROCESS_END();
}
