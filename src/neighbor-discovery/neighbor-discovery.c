#include "contiki.h"
#include "net/rime.h"

#include <stdio.h>

#include "network-view.h"

#define BROADCAST_INTERVAL 30

PROCESS(neighbor_discovery_process, "Neighbor discovery process");
AUTOSTART_PROCESSES(&neighbor_discovery_process);

PROCESS_THREAD(neighbor_discovery_process, ev, data)
{
    static struct etimer periodic_timer;

    PROCESS_BEGIN();

    init_network_view();
    debug_print_nodes();

    while (1)
    {
        etimer_reset(&periodic_timer);
        etimer_set(&periodic_timer, CLOCK_SECOND * BROADCAST_INTERVAL);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

        debug_print_nodes();
    }

    PROCESS_END();
}
