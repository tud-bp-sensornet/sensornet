#include "contiki.h"
#include "graph.h"

#include <stdlib.h>
#include <stdio.h>


PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);

PROCESS_THREAD(example_broadcast_process, ev, data) {
	
	PROCESS_BEGIN();
	
	p_node_t *n1, *n2, *n3, *n4;
	
	n1 = (p_node_t*) malloc(sizeof(p_node_t));
	n2 = (p_node_t*) malloc(sizeof(p_node_t));
	n3 = (p_node_t*) malloc(sizeof(p_node_t));
	n4 = (p_node_t*) malloc(sizeof(p_node_t));


	n1->addr.u8[0] = 0xFF;

	printf("%d\n", rimeaddr_node_addr.u8[0]);
	printf("%d:%d\n", n1->addr.u8[0], n1->addr.u8[1]);
	PROCESS_END();
}
