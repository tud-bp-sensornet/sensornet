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

/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{

  p_node_t* newroot = deserialize(packetbuf_dataptr());

  printf("broadcast message received from %d.%d: %d.%d %d.%d\n",
         from->u8[0], from->u8[1], newroot->addr.u8[0], newroot->last_seen, newroot->edges->drain->addr.u8[0], newroot->edges->drain->last_seen);

}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
printf("start");
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  printf("open!");

  broadcast_open(&broadcast, 129, &broadcast_call);

  //Example tree
  p_node_t *root = (p_node_t*) malloc(sizeof(p_node_t));
  root->addr = rimeaddr_node_addr;
  root->last_seen = 0; 

  p_node_t *neighbour = (p_node_t*) malloc(sizeof(p_node_t));
  neighbour->addr.u8[0] = 2;
  neighbour->last_seen = 1;

  p_edge_t *edge = (p_edge_t*) malloc(sizeof(p_edge_t));
  edge->drain = neighbour;
  edge->next = NULL;

  root->edges = edge;
  neighbour->edges = NULL;


  while(1) {

    /* Delay 2-4 seconds */
    etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    //clear packet buffer and set packet size
    packetbuf_clear ();

    packetbuf_set_datalen (sizeof(p_node_t) * 2 + sizeof(p_edge_t));

    uint16_t * nodes;
    uint16_t * edges;

    printf("serialize!");

    void * serializationptr = serialize(root, 1, 2, 1, nodes, edges);

    printf("end serialize");

    packetbuf_copyfrom(serializationptr, sizeof(p_node_t) * 2 + sizeof(p_edge_t));

    printf("send");
    
    broadcast_send(&broadcast);

    free(serializationptr);
  }

  PROCESS_END();
}
