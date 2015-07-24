#include "contiki.h"
#include "net/rime.h"
#include "pbroadcast.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void fill_with_data(void *data, size_t length, uint16_t base)
{
	int i;
	for (i = 0; i < length; i++)
	{
		*((char*)data + i) = (i + base) & 0xFF;
	}
}

// Sends a packet without a hash using abc broadcast. Receivers will interpret the last 2 bytes as
// the hash and should therefore discard the packet as it is obviously not correct.
static struct abc_conn abc;
static const struct abc_callbacks callbacks = {NULL};
void broadcast_faulty_message()
{
	abc_open(&abc, 128, &callbacks);
	void *packet = malloc(64);
	fill_with_data(packet, 64, 64);
	memcpy(packetbuf_dataptr(), packet, 64);
	free(packet);
	abc_send(&abc);
	abc_close(&abc);
}

void broadcast_received(const struct p_broadcast_conn *bc, const rimeaddr_t *sender, const void *data, size_t length)
{
	printf("BROADCAST RECEIVED WITH CORRECT HASH, SIZE=%d, FIRST BYTE=%d\n", length, ((char *)data)[0]);

	int i;
	printf("broadcast received from %d.%d (size %d): ", sender->u8[0], sender->u8[1], (int)length);
	for (i = 0; i < length; i++)
	{
		printf("%02X ", ((char*)data)[i] & 0xFF);
	}
	printf("\n");
}

#define BROADCAST_INTERVAL 20

PROCESS(broadcast_test_process, "Broadcast testing process");
AUTOSTART_PROCESSES(&broadcast_test_process);

PROCESS_THREAD(broadcast_test_process, ev, data)
{
	static struct etimer periodic_timer;

	PROCESS_BEGIN();

	static struct p_broadcast_conn c;
	c.received = broadcast_received;
	p_broadcast_open(&c, 128);

	etimer_reset(&periodic_timer);
	etimer_set(&periodic_timer, CLOCK_SECOND * BROADCAST_INTERVAL);
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

	// only the first node sends a packet
	if (rimeaddr_node_addr.u8[0] == 1)
	{
		printf("BEGIN SEND CORRECT PACKET\n");

		void *packet = malloc(64);
		fill_with_data(packet, 64, 32);

		p_broadcast_send(&c, packet, 64);

		free(packet);

		printf("END SEND CORRECT PACKET\n");
	}

	etimer_reset(&periodic_timer);
	etimer_set(&periodic_timer, CLOCK_SECOND * BROADCAST_INTERVAL);
	PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

	// only the first node sends a packet
	if (rimeaddr_node_addr.u8[0] == 1)
	{
		p_broadcast_close(&c);

		printf("BEGIN SEND FAULTY PACKET\n");
		broadcast_faulty_message();

		printf("END SEND FAULTY PACKET\n");
	}

	PROCESS_END();
	
}
