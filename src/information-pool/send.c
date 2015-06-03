/**
 * \file
 *         Testing the bulk broadcast layer in Rime..
 * \author
 *         tud-bp-sensornet
 */

#include "contiki.h"
#include "net/rime.h"
#include "random.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "serialize.h"
#include "memory.h"
#include "net/rime/rudolph0.h"

static struct p_node_t *root;
static void * serializationptr;

//Last serialization amount
static uint16_t nodes = 1;
static uint16_t edges = 0;

static void * recv_memory;

/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/

static void
broadcast_chunk_recv(struct rudolph0_conn *c, int offset, int flag, uint8_t *data, int datalen)
{
	void* offset_ptr = recv_memory + (uint8_t) offset;
	memcpy(offset_ptr, (void*)data, (size_t)datalen);

	printf("Got paket: %d size\n", datalen);

	//TODO: handle multiple senders
	if (flag == RUDOLPH0_FLAG_LASTCHUNK)
	{
		printf("got all pakets\n");
		p_node_t* newroot = deserialize(recv_memory);

		iterateUpdate(newroot);
	}
}

static int
broadcast_chunk_read(struct rudolph0_conn *c, int offset, uint8_t *to, int maxsize)
{
	//return pointer to start
	to = serializationptr + offset;
	//return length of data
	int size = (sizeof(p_node_t) * nodes + sizeof(p_edge_t) * edges);
	return (size - offset) > maxsize ? maxsize : (size - offset);
}

/*---------------------------------------------------------------------------*/
static const struct rudolph0_callbacks rudolph0_call = {broadcast_chunk_recv, broadcast_chunk_read};
static struct rudolph0_conn rudolph0;
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(example_broadcast_process, ev, data)
{

	static struct etimer et;

	//TODO: Returns system time, all motes have to be started simultaneously
	clock_init();

	PROCESS_EXITHANDLER(rudolph0_close(&rudolph0);)

	PROCESS_BEGIN();

	rudolph0_open(&rudolph0, 129, &rudolph0_call);

	init_mem();

	recv_memory = (void*) malloc (sizeof(p_node_t) * 50 + sizeof(p_edge_t) * 50);

	//We are root
	root = get_node();
	root->addr = rimeaddr_node_addr;
	root->last_seen = clock_seconds();

	p_node_t* n1 = get_node();
	p_node_t* n2 = get_node();
	p_node_t* n3 = get_node();
	p_node_t* n4 = get_node();
	p_node_t* n5 = get_node();
	p_node_t* n6 = get_node();
	p_node_t* n7 = get_node();
	p_node_t* n8 = get_node();
	p_node_t* n9 = get_node();
	p_node_t* n10 = get_node();
	p_node_t* n11 = get_node();
	p_node_t* n12 = get_node();
	p_node_t* n13 = get_node();

	p_edge_t* e1 = get_edge();
	p_edge_t* e2 = get_edge();
	p_edge_t* e3 = get_edge();
	p_edge_t* e4 = get_edge();
	p_edge_t* e5 = get_edge();
	p_edge_t* e6 = get_edge();
	p_edge_t* e7 = get_edge();
	p_edge_t* e8 = get_edge();
	p_edge_t* e9 = get_edge();
	p_edge_t* e10 = get_edge();
	p_edge_t* e11 = get_edge();
	p_edge_t* e12 = get_edge();
	p_edge_t* e13 = get_edge();

	root->edges = e1;
	e1->drain = n1;
	e2->drain = n2;
	e3->drain = n3;
	e4->drain = n4;
	e5->drain = n5;
	e6->drain = n6;
	e7->drain = n7;
	e8->drain = n8;
	e9->drain = n9;
	e10->drain = n10;
	e11->drain = n11;
	e12->drain = n12;
	e13->drain = n13;

	e1->next = e2;
	e2->next = e3;
	e3->next = e4;
	e4->next = e5;
	e5->next = e6;
	e6->next = e7;
	e7->next = e8;
	e8->next = e9;
	e9->next = e10;
	e10->next = e11;
	e11->next = e12;
	e12->next = e13;
	e13->next = NULL;

	while (1) {

		/* Delay 2-4 seconds */
		etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		//clear packet buffer and set packet size
		packetbuf_clear ();

		//TODO: Keep track over our tree size
		//TODO: Worst case, find better way
		void * serializationptr = serialize(root, 3, 15, 15, &nodes, &edges);

		printf("Start sending size: %d\n", sizeof(p_node_t) * nodes + sizeof(p_edge_t) * edges);

		rudolph0_send(&rudolph0, CLOCK_SECOND / 4);

		//free(serializationptr);

	}

	PROCESS_END();
}

