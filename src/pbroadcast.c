/**
 * \file pbroadcast.c
 * Identified broadcast Rime layer ontop of anonymous broadcast (abc).
 * \author tud-bp-sensornet
 */

#include <string.h>
#include "contiki-net.h"
#include "crc16.h"
#include "pbroadcast.h"

/**
 * \def __BROADCAST_DEBUG__
 * \brief Set to 1 to activate debug output.
*/
#ifndef __BROADCAST_DEBUG__
#define __BROADCAST_DEBUG__ 0
#endif

//\cond
#if __BROADCAST_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
void debug_dump_data(void *data, size_t data_length)
{
	int i;
	for (i = 0; i < data_length; i++)
	{
		printf("%02X ", ((char *)packetbuf_dataptr())[i] & 0xFF);
	}
}
#define DEBUG_DUMP_DATA(data, data_length) debug_dump_data(data, data_length)
#else
#define PRINTF(...)
#define DEBUG_DUMP_DATA(data, data_length)
#endif
//\endcond

/*---------------------------------------------------------------------------*/
static uint16_t packet_hash(const void *data, size_t length)
{
	return crc16_data(data, length, 0) & 0xFFFF;
}
/*---------------------------------------------------------------------------*/
static const struct packetbuf_attrlist attributes[] =
{
	BROADCAST_ATTRIBUTES PACKETBUF_ATTR_LAST
};
/*---------------------------------------------------------------------------*/
static void recv_from_abc(struct abc_conn *bc)
{

	if (bc == NULL)
	{
		PRINTF("[pbroadcast.c] Error: ABC Connection is NULL\n");
		return;
	}

	rimeaddr_t sender;
	struct p_broadcast_conn *c = (struct p_broadcast_conn *)bc;

	rimeaddr_copy(&sender, packetbuf_addr(PACKETBUF_ADDR_SENDER));

	uint16_t actual_hash = packet_hash(packetbuf_dataptr(), packetbuf_datalen() - sizeof(uint16_t));
	uint8_t *hash_ptr = packetbuf_dataptr() + (uintptr_t)(packetbuf_datalen() - sizeof(uint16_t));
	uint16_t transmitted_hash = (hash_ptr[1] << 8) + (hash_ptr[0]);

	PRINTF("[pbroadcast.c] Received message of size %d from %d.%d.\n", packetbuf_datalen() - sizeof(uint16_t), sender.u8[0], sender.u8[1]);
	PRINTF("[pbroadcast.c] ^ Data: ");
	DEBUG_DUMP_DATA(packetbuf_dataptr(), packetbuf_datalen());
	PRINTF("\n");

	PRINTF("[pbroadcast.c] ^ Hash: %02X\n", transmitted_hash);

	if (actual_hash != transmitted_hash)
	{
		PRINTF("[pbroadcast.c] ^ Incorrect hash (expected %02X, but is %02X); discarding received packet.\n", transmitted_hash, actual_hash);
		return;
	}

	if (c->received)
	{
		c->received(c, &sender, packetbuf_dataptr(), packetbuf_datalen() - sizeof(uint16_t));
	}
}
/*---------------------------------------------------------------------------*/
static const struct abc_callbacks callbacks = {recv_from_abc};
/*---------------------------------------------------------------------------*/
void p_broadcast_open(struct p_broadcast_conn *c, uint16_t channel)
{
	if (c == NULL)
	{
		PRINTF("[pbroadcast.c] Error: Broadcast Connection is NULL\n");
		return;
	}

	abc_open(&(c->abc), channel, &callbacks);
	channel_set_attributes(channel, attributes);
}
/*---------------------------------------------------------------------------*/
void p_broadcast_close(struct p_broadcast_conn *c)
{

	if (c == NULL)
	{
		PRINTF("[pbroadcast.c] Error: Broadcast Connection is NULL\n");
		return;
	}

	abc_close(&(c->abc));
}
/*---------------------------------------------------------------------------*/
int p_broadcast_send(struct p_broadcast_conn *c, const void *data, size_t length)
{
	PRINTF("[pbroadcast.c] Sending message of size %d\n", (int)length);

	if (length > MAX_BROADCAST_PAYLOAD_SIZE)
	{
		PRINTF("[pbroadcast.c] ^ Error: Broadcast packet is too long.\n");
		return 0;
	}

	if (c == NULL)
	{
		PRINTF("[pbroadcast.c] ^ Error: Broadcast Connection is NULL\n");
		return 0;
	}

	packetbuf_clear();
	packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &rimeaddr_node_addr);
	packetbuf_set_datalen(length + sizeof(uint16_t));

	// copy payload into packet
	memcpy(packetbuf_dataptr(), data, length);

	// copy hash to end of packet
	uint16_t hash = packet_hash(data, length);
	memcpy(packetbuf_dataptr() + (uintptr_t)length, &hash, sizeof(uint16_t));

	PRINTF("[pbroadcast.c] ^ Hash: %02X Datalen: %d\n", hash, packetbuf_datalen());
	PRINTF("[pbroadcast.c] ^ Data: ");
	DEBUG_DUMP_DATA(packetbuf_dataptr(), packetbuf_datalen());
	PRINTF("\n");

	int status = abc_send(&(c->abc));

	if (status == 0)
	{
		PRINTF("[pbroadcast.c] Broadcast could not be sent.\n");
	}

	return status;
}
/*---------------------------------------------------------------------------*/
