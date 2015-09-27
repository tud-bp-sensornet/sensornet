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

PROCESS(simple_process, "Simple process");
AUTOSTART_PROCESSES(&simple_process);

void message_received(const void *packet_data, size_t length)
{
	printf("[firmware.c] Received msg: %s\n", (char*)packet_data);
}

void send_message_after_5_minutes()
{
	printf("[firmware.c] 5 minutes have passed!\n");

	if (linkaddr_node_addr.u8[0] == 50 || linkaddr_node_addr.u8[0] == 2 || linkaddr_node_addr.u8[0] == 9 || 
		linkaddr_node_addr.u8[0] == 36 || linkaddr_node_addr.u8[0] == 31 || linkaddr_node_addr.u8[0] == 19)
	{
		linkaddr_t dst = {{33,0}};

		char msg[25];
		int len = snprintf(msg, 25, "Feuer bei %d.%d!\n", linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
		send_message(msg, len+1, &dst);

		printf("[firmware.c] Sent message of size %d!\n", len);
	}
}

PROCESS_THREAD(simple_process, ev, data)
{
	PROCESS_BEGIN();
	
	cc2420_set_txpower(12);
	init_router(&message_received);

	static struct ctimer ct;
	ctimer_set(&ct, CLOCK_SECOND * 60 * 5, send_message_after_5_minutes, NULL);
	
	process_start(&neighbor_discovery_process, NULL);

	printf("K = %d, MAX_NODES = %d, MAX_EDGES = %d, linkaddr = %d.%d\n", (int) K, (int) MAX_NODES, (int) MAX_EDGES, linkaddr_node_addr.u8[0], linkaddr_node_addr.u8[1]);
	printf("NODE_MEM = %dB, EDGE_MEM = %dB\n", (int)(sizeof(p_node_t) * MAX_NODES), (int)(sizeof(p_edge_t) * MAX_EDGES));
	printf("PACKETBUF_SIZE = %d, TXPOWER = %d, INTERVAL = %d - %d\n", PACKETBUF_SIZE, cc2420_get_txpower(), DISCOVERY_INTERVAL_MIN, DISCOVERY_INTERVAL_MAX);

	PROCESS_END();
}
