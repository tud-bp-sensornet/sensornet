/**
 * \file
 *         Identified best-effort bulk local area broadcast
 * \author
 *         Teriame
 */

#include "contiki-net.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

#include "bulk_broadcast.h"

static const struct packetbuf_attrlist attributes[] =
{
	BROADCAST_ATTRIBUTES PACKETBUF_ATTR_LAST
};

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------------------*/
static void
read_new_datapacket(struct bulk_broadcast_conn *c)
{
	int len = 0;

	if (c->cb->read_chunk) { //TODO when c->data returns correct pointer broadcast will not send, on wrong pointer sending is no problem
		len = c->cb->read_chunk(c, c->currenthdr.chunk * BULK_BROADCAST_DATASIZE, c->data, BULK_BROADCAST_DATASIZE);
	}
	if (len > 0) {
		c->currenthdr.datalen = len;
	} else {
		c->currenthdr.datalen = 0;
	}

	PRINTF("read_new_datapacket len %d\n", len);
}
/*---------------------------------------------------------------------------*/
static void
recv_from_abc(struct abc_conn *bc)
{
	rimeaddr_t sender;
	struct bulk_broadcast_conn *c = (struct bulk_broadcast_conn *)bc;
	struct bulk_broadcast_hdrpacket *p = packetbuf_dataptr();

	PRINTF("received chunk %d\n", p->chunk);
	//TODO test if chunk order is correct, no chunk was lost
	rimeaddr_copy(&sender, packetbuf_addr(PACKETBUF_ADDR_SENDER));
	if (p->datalen < BULK_BROADCAST_DATASIZE) {
		c->cb->write_chunk(c, p->chunk * BULK_BROADCAST_DATASIZE, BULK_BROADCAST_FLAG_LASTCHUNK, (packetbuf_dataptr() + sizeof(struct bulk_broadcast_hdrpacket)), p->datalen, &sender);
	} else {
		c->cb->write_chunk(c, p->chunk * BULK_BROADCAST_DATASIZE, BULK_BROADCAST_FLAG_NONE, (packetbuf_dataptr() + sizeof(struct bulk_broadcast_hdrpacket)), p->datalen, &sender);
	}
}
/*---------------------------------------------------------------------------*/
static void
sent_by_abc(struct abc_conn *bc, int status, int num_tx)
{
	struct bulk_broadcast_conn *c = (struct bulk_broadcast_conn *)bc;

	//No retransmission, send next chunk
	c->currenthdr.chunk++;
	read_new_datapacket(c);
	if (c->currenthdr.datalen == 0) {
		PRINTF("Finished sending!\n");
	} else {
		PRINTF("Sending data chunk %d next time\n", c->currenthdr.chunk);
		packetbuf_clear();
		packetbuf_set_datalen((uint16_t)(c->currenthdr.datalen + sizeof(struct bulk_broadcast_hdrpacket)));
		packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &rimeaddr_node_addr);
		memcpy(packetbuf_dataptr(), (void*)&c->currenthdr, (size_t)sizeof(struct bulk_broadcast_hdrpacket));
		memcpy(packetbuf_dataptr() + sizeof(struct bulk_broadcast_hdrpacket), *(c->data), c->currenthdr.datalen);
		abc_send(&c->c);
	}
}
/*---------------------------------------------------------------------------*/
static const struct abc_callbacks broadcast_call = {recv_from_abc, sent_by_abc};
/*---------------------------------------------------------------------------*/

void
bulk_broadcast_open(struct bulk_broadcast_conn *c, uint16_t channel, const struct bulk_broadcast_callbacks *cb)
{
	abc_open(&c->c, channel, &broadcast_call);
	c->cb = cb;
	channel_set_attributes(channel, attributes);
}
/*---------------------------------------------------------------------------*/
void
bulk_broadcast_close(struct bulk_broadcast_conn *c)
{
	abc_close(&c->c);
}
/*---------------------------------------------------------------------------*/
int
bulk_broadcast_send(struct bulk_broadcast_conn *c)
{
	c->currenthdr.chunk = 0;
	//Ask for new data
	read_new_datapacket(c);
	//Load data into packetbuf
	packetbuf_clear();
	printf("%d %d\n", ((p_node_t*)*(c->data))->addr.u8[0], ((p_node_t*)*(c->data))->addr.u8[1]);
	packetbuf_set_datalen((uint16_t)(c->currenthdr.datalen + sizeof(struct bulk_broadcast_hdrpacket)));
	packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &rimeaddr_node_addr);
	memcpy(packetbuf_dataptr(), (void*)&c->currenthdr, (size_t)sizeof(struct bulk_broadcast_hdrpacket));
	memcpy(packetbuf_dataptr() + sizeof(struct bulk_broadcast_hdrpacket), *(c->data), c->currenthdr.datalen);
	return abc_send(&c->c);
}
/*---------------------------------------------------------------------------*/
/** @} */
