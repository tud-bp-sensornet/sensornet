/**
 * \file
 *         Testing the bulk broadcast layer in Rime, transmitting a struct containing a list of structs.
 * \author
 *         tud-bp-sensornet
 */

#include "contiki.h"
#include "net/rime.h"
#include "random.h"

#include <stdio.h>

//Example struct 2
typedef struct {
  unsigned short a;
} my_struct2;

//Example list memb
MEMB(node_memory, my_struct2, 100);

//Example struct
typedef struct {
  LIST_STRUCT (a);
  unsigned short b;
} my_struct;

/*---------------------------------------------------------------------------*/
PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{

  my_struct *my;
  my = packetbuf_dataptr();

  my_struct2* number = list_head(my->a);

  printf("broadcast message received from %d.%d: %d.%d\n",
         from->u8[0], from->u8[1], my->b, number->a);

  //for (my_struct2* number = list_head(my->a); number != NULL; number = list_item_next(number)){
  //  printf("%d", number->a);
  //}

}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(example_broadcast_process, ev, data)
{
  static struct etimer et;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

  memb_init(&node_memory);

  while(1) {

    /* Delay 2-4 seconds */
    etimer_set(&et, CLOCK_SECOND * 4 + random_rand() % (CLOCK_SECOND * 4));

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    //clear packet buffer and set packet size
    packetbuf_clear ();
    //packetbuf_set_datalen (sizeof (test) * sizeof (uint8_t));

    my_struct my;// = (my_struct*)memb_alloc(&node_memory);
    LIST_STRUCT_INIT(&my, a);
    list_init(my.a);
    my.b = 1;
    my_struct2 number;// = (my_struct2*)memb_alloc(&node_memory);
    number.a = 1;
    list_add(my.a, &number);

    packetbuf_copyfrom((void *)&my, sizeof(my_struct));

    broadcast_send(&broadcast);
    printf("broadcast message sent\n");
  }

  PROCESS_END();
}
