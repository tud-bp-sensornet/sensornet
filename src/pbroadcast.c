/**
 * \file           Identified broadcast Rime layer ontop of anonymous broadcast (abc)
 * \author         tud-bp-sensornet
 */

#include <string.h>
#include "contiki-net.h"
#include "crc16.h"
#include "pbroadcast.h"

#ifndef __BROADCAST_DEBUG__
#define __BROADCAST_DEBUG__ 0
#endif

#if __BROADCAST_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static uint16_t packet_hash(const void *data, size_t length)
{
	return crc16_data(data, length, 0) & 0xFFFF;
}

static const struct packetbuf_attrlist attributes[] =
{
	BROADCAST_ATTRIBUTES PACKETBUF_ATTR_LAST
};

static void recv_from_abc(struct abc_conn *bc)
{

	if (bc == NULL)
	{
		PRINTF("[Broadcast] Error: ABC Connection is NULL\n");
		return;
	}

	rimeaddr_t sender;
	struct p_broadcast_conn *c = (struct p_broadcast_conn *)bc;

	rimeaddr_copy(&sender, packetbuf_addr(PACKETBUF_ADDR_SENDER));

	uint16_t actual_hash = packet_hash(packetbuf_dataptr(), packetbuf_datalen() - sizeof(uint16_t));
	uint16_t transmitted_hash = *(uint16_t *)(packetbuf_dataptr() + (uintptr_t)(packetbuf_datalen() - sizeof(uint16_t)));

	if (actual_hash != transmitted_hash)
	{
		PRINTF("[Broadcast] Incorrect hash (expected %d, but is %d); discarding received packet.\n", transmitted_hash, actual_hash);
		return;
	}

	if (c->received)
	{
		c->received(c, &sender, packetbuf_dataptr(), packetbuf_datalen() - sizeof(uint16_t));
	}
}

static const struct abc_callbacks callbacks = {recv_from_abc};

void p_broadcast_open(struct p_broadcast_conn *c, uint16_t channel)
{

	if (c == NULL)
	{
		PRINTF("[Broadcast] Error: Broadcast Connection is NULL\n");
		return;
	}

	abc_open(&(c->abc), channel, &callbacks);
	channel_set_attributes(channel, attributes);
}

void p_broadcast_close(struct p_broadcast_conn *c)
{

	if (c == NULL)
	{
		PRINTF("[Broadcast] Error: Broadcast Connection is NULL\n");
		return;
	}

	abc_close(&(c->abc));
}

int p_broadcast_send(struct p_broadcast_conn *c, const void *data, size_t length)
{

	if (length > MAX_BROADCAST_PAYLOAD_SIZE)
	{
		PRINTF("[Broadcast] Error: Broadcast packet is too long.\n");
		return 0;
	}

	if (c == NULL)
	{
		PRINTF("[Broadcast] Error: Broadcast Connection is NULL\n");
		return 0;
	}

	packetbuf_clear();
	packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &rimeaddr_node_addr);
	packetbuf_set_datalen(length + sizeof(uint16_t));

	// copy payload into packet
	memcpy(packetbuf_dataptr(), data, length);

	// copy hash to end of packet
	uint16_t hash = packet_hash(data, length);
	PRINTF("[Broadcast] hash: %02X datalen: %d\n", hash, packetbuf_datalen());
	memcpy(packetbuf_dataptr() + (uintptr_t)length, &hash, sizeof(uint16_t));

	int status = abc_send(&(c->abc));

	if (status == 0)
	{
		PRINTF("[Broadcast] Broadcast could not be sent.\n");
	}

	return status;
}
/*---------------------------------------------------------------------------*/
