/**
 * \file simple-application.c
 * The simple application starts neighbor-discovery and sends a routing package on buttonpress.
 * \author tud-bp-sensornet
 */

#include "contiki.h"
#include "graph.h"
#include "graph-operations.h"
#include "serialize.h"
#include "pbroadcast.h"
#include "random.h"
#include "routing.h"
#include "positions.h"
#include "neighbor-discovery.h"

#include "dev/button-sensor.h"

//\cond
PROCESS(simple_application_process, "Simple application process");
AUTOSTART_PROCESSES(&simple_application_process);
//\endcond

/**
 * \brief The destination linkaddr_t to send routing packages to.
*/
linkaddr_t node_destination = {{0x04}};

//\cond
PROCESS_THREAD(simple_application_process, ev, data)
{
	process_start(&neighbor_discovery_process, NULL);

	PROCESS_EXITHANDLER(close_router());

	PROCESS_BEGIN();
	SENSORS_ACTIVATE(button_sensor);

	init_router(NULL);

	//When button is pressed send message using router
	while (1)
	{
		PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event && data == &button_sensor);

		send_message("Fire!", 5, &node_destination);
	}

	PROCESS_END();
}
//\endcond
