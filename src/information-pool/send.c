#include "contiki.h"
#include "net/rime.h"
#include "random.h"

#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "serialize.h"
#include "memory.h"
#include "bulk_broadcast.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static void * serializationptr;

static p_graph_t* graph;

static void * recv_memory;

static size_t bytes; //TODO

/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/

static void
broadcast_chunk_recv(struct bulk_broadcast_conn *c, int offset, int flag, void *data, int datalen, const rimeaddr_t *sender)
{
	void* offset_ptr = recv_memory + offset;
	memcpy(offset_ptr, data, datalen);

	PRINTF("Got paket: %d size from %d\n", datalen, sender->u8[0]);

	//TODO: handle multiple senders
	if (flag == BULK_BROADCAST_FLAG_LASTCHUNK)
	{
		PRINTF("got all pakets, root is: %d.%d\n", ((p_node_t*)recv_memory)->addr.u8[0], ((p_node_t*)recv_memory)->addr.u8[1]);
		
		p_node_t* newroot = deserialize(recv_memory);

		PRINTF("got info: node: %d\n", newroot->addr.u8[0]);

		updateGraph(graph, newroot);
		
		//free the allocated space from the deserialization
		free((void*) newroot);
	}
}

static int
broadcast_chunk_read(struct bulk_broadcast_conn *c, int offset, void **to, int maxsize)
{
	//return pointer to start
	*to = serializationptr + offset;
	//return length of data
	int size = bytes;
	return (size - offset) > maxsize ? maxsize : (size - offset);
	//TODO //free(serializationptr);
}

/*---------------------------------------------------------------------------*/
static const struct bulk_broadcast_callbacks bulk_broadcast_call = {broadcast_chunk_recv, broadcast_chunk_read};
static struct bulk_broadcast_conn bulk_broadcast;
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(example_broadcast_process, ev, data)
{
	static struct etimer et;

	PROCESS_EXITHANDLER(bulk_broadcast_close(&bulk_broadcast);)

	PROCESS_BEGIN();

	bulk_broadcast_open(&bulk_broadcast, 128, &bulk_broadcast_call);

	init_mem();

	//TODO: Worst case, use serialize retuned size
	recv_memory = malloc(sizeof(p_node_t) * MAX_NODES + sizeof(p_edge_t) * MAX_EDGES);

	graph = malloc(sizeof *graph);
	
	//We are root
	graph->num_edges = 0;
	graph->num_nodes = 0;
	graph->root = new_node(&(graph->num_nodes));
	graph->root->addr = rimeaddr_node_addr;
	graph->root->edges = NULL;
	
	while (1) {

		/* Delay 10-20 seconds */
		etimer_set(&et, CLOCK_SECOND * 10 + random_rand() % (CLOCK_SECOND * 20));

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		//clear packet buffer
		packetbuf_clear();

		serializationptr = serialize(graph, K-1, &bytes);

		PRINTF("Start sending size: %d edges: %d nodes: %d\n", bytes, graph->num_edges, graph->num_nodes);
		
		bulk_broadcast_send(&bulk_broadcast);
		free(serializationptr);
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/** @} */
