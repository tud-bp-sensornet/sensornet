#include "contiki.h"
#include "net/rime.h"
#include "random.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bulk_broadcast.h"

void broadcast_received(const struct bulk_broadcast_conn *bc, const rimeaddr_t *sender, void *data, size_t length)
{
	int i;
	printf("broadcast received from %d.%d (size %d): ", sender->u8[0], sender->u8[1], (int)length);
	for (i = 0; i < length; i++)
	{
		printf("%02X ", ((char*)data)[i] & 0xFF);
	}
	printf("\n");

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
	
	while (1) {

		/* Delay 10-20 seconds */
		etimer_set(&et, CLOCK_SECOND * 10 + random_rand() % (CLOCK_SECOND * 20));

		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		size_t blocksize = 256;
		void* big_block = malloc(blocksize);
		int i;
		for (i = 0; i < blocksize; i++)
		{
			*((char*)big_block + i) = i;
		}
		
		bulk_broadcast_send(&bulk_broadcast, big_block, blocksize);

		free(big_block);
	}

	PROCESS_END();
}
