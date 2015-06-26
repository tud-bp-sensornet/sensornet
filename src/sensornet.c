#include "contiki.h"

#include <stdio.h>
/*---------------------------------------------------------------------------*/
PROCESS(simple_process, "Simple process");
AUTOSTART_PROCESSES(&simple_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(simple_process, ev, data)
{
PROCESS_BEGIN();

printf("K = %d, MAX_NODES = %d, MAX_EDGES = %d\n", (int) K, (int) MAX_NODES, (int) MAX_EDGES);\

PROCESS_END();
}
/*---------------------------------------------------------------------------*/;