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

#include "serialize.h"
#include "serialize.c"
#include "graph.h"
#include "memory.c"

static struct p_node_t *root;
static struct p_node_t *neighbour;
static struct p_edge_t *edge;

/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/

static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{

  printf("deserialize! paket is: %d\n", packetbuf_datalen());

  p_node_t* newroot = deserialize(packetbuf_dataptr());
  printf("broadcast message received from %d.%d: %d.%d\n", from->u8[0], from->u8[1], newroot->addr.u8[0], newroot->edges->drain->addr.u8[0]);
}

/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(example_broadcast_process, ev, data)
{

  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

  init_mem();

  //Example tree
  root = get_node();
  root->addr.u8[0] = 0x01;

  neighbour = get_node();
  neighbour->addr.u8[0] = 0x02;

  edge = get_edge();
  edge->drain = neighbour;
  edge->next = NULL;

  root->edges = edge;
  neighbour->edges = NULL;

  while (1) {

    /* Delay 2-4 seconds */
    etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    //clear packet buffer and set packet size
    packetbuf_clear ();

    uint16_t * nodes;
    uint16_t * edges;

    void * serializationptr = serialize(root, 1, 2, 1, nodes, edges);

    packetbuf_set_datalen (sizeof(p_node_t) * *nodes + sizeof(p_edge_t) * *edges);
    packetbuf_copyfrom(serializationptr, sizeof(p_node_t) * *nodes + sizeof(p_edge_t) * *edges);

    broadcast_send(&broadcast);

    free(serializationptr);
  }

  PROCESS_END();
}


