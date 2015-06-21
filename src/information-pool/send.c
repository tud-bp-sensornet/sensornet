#include "contiki.h"
#include "net/rime.h"
#include "random.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "graph.h"
#include "serialize.h"
#include "memory.h"
#include "bulk_broadcast.h"

static p_graph_t* graph;

void broadcast_received(const struct bulk_broadcast_conn *bc, const rimeaddr_t *sender, void *data, size_t length)
{
	int i;
	printf("broadcast received from %d.%d (size %d): ", sender->u8[0], sender->u8[1], (int)length);
	for (i = 0; i < length; i++)
	{
		printf("%02X ", ((char*)data)[i] & 0xFF);
	}
	printf("\n");

	p_node_t* foreign_root = deserialize(data);

	printf("updating with foreign node, root: %d.%d\n", foreign_root->addr.u8[0], foreign_root->addr.u8[1]);

	updateGraph(graph, foreign_root);

	free(data);
}

/*---------------------------------------------------------------------------*/
static const struct bulk_broadcast_callbacks bulk_broadcast_call = {broadcast_received};
static struct bulk_broadcast_conn bulk_broadcast;
/*---------------------------------------------------------------------------*/

PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
PROCESS_THREAD(example_broadcast_process, ev, data)
{
	static struct etimer et;

	PROCESS_EXITHANDLER(bulk_broadcast_close(&bulk_broadcast);)

	PROCESS_BEGIN();

	bulk_broadcast_open(&bulk_broadcast, 128, &bulk_broadcast_call);

	init_mem();

	graph = malloc(sizeof *graph);
	
	//We are root
	graph->num_edges = 0;
	graph->num_nodes = 0;
	graph->root = new_node(&(graph->num_nodes));
	graph->root->addr = rimeaddr_node_addr;
	graph->root->edges = NULL;
	
	while (1)
	{
		/* Delay 10-20 seconds */
		etimer_set(&et, CLOCK_SECOND * 10 + random_rand() % (CLOCK_SECOND * 20));

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		size_t bytes;
		void* serialized = serialize(graph, K-1, &bytes);

		printf("Start sending size: %d, edges: %d, nodes: %d\n", bytes, graph->num_edges, graph->num_nodes);

		bulk_broadcast_send(&bulk_broadcast, serialized, bytes);

		free(serialized);
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/** @} */
