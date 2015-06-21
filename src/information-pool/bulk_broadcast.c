#define __BULK_BROADCAST_DEBUG__ 0

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sys/timer.h"
#include "contiki-net.h"

#include "graph.h"
#include "bulk_broadcast.h"
#include "buffer.h"

static const struct packetbuf_attrlist attributes[] =
{
	BROADCAST_ATTRIBUTES PACKETBUF_ATTR_LAST
};

#if __BULK_BROADCAST_DEBUG__
void debug_dump_broadcast_packetbuf(size_t data_length)
{
	int i;
	printf("packetbuf: ");
	for (i = 0; i < (sizeof(struct bulk_broadcast_header) + data_length); i++)
	{
		printf("%02X ", ((char*)packetbuf_dataptr())[i] & 0xFF);
		if (i == sizeof(struct bulk_broadcast_header) - 1) printf("~ ");
	}
	printf("\n");
}
#endif

void reset_receiver(struct bulk_broadcast_receive_task *rc)
{
	rc->active = false;
	rc->announced_size = 0;
	free(rc->buffer);
	rc->buffer = NULL;
	rc->current_sender = rimeaddr_null;
	rc->received_chunk_count = 0;
	timer_set(&(rc->timeout), BULK_BROADCAST_RECEIVE_TIMEOUT);
}

int send_bulk_broadcast_packet(struct abc_conn *bc, struct bulk_broadcast_header header, void* data)
{
	packetbuf_clear();
	packetbuf_set_datalen((uint16_t)(sizeof(struct bulk_broadcast_header) + header.data_length));
	packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &rimeaddr_node_addr);

	// copy header and data into packetbuffer
	memcpy(packetbuf_dataptr(), &header, sizeof(struct bulk_broadcast_header));
	uintptr_t chunk_offset = BULK_BROADCAST_DATASIZE * header.chunk_id;
	memcpy(packetbuf_dataptr() + sizeof(struct bulk_broadcast_header), data + chunk_offset, header.data_length);

#if __BULK_BROADCAST_DEBUG__
	debug_dump_broadcast_packetbuf(header.data_length);
#endif

	return abc_send(bc);
}

static void recv_from_abc(struct abc_conn *bc)
{
	rimeaddr_t sender;
	rimeaddr_copy(&sender, packetbuf_addr(PACKETBUF_ADDR_SENDER));

	struct bulk_broadcast_conn *c = (struct bulk_broadcast_conn *)bc;

	struct bulk_broadcast_header *header = packetbuf_dataptr();
	void* data = packetbuf_dataptr() + (uintptr_t) sizeof(struct bulk_broadcast_header);

#if __BULK_BROADCAST_DEBUG__
	printf("received ");
	debug_dump_broadcast_packetbuf(header->data_length);
#endif

	// not currently listening and new data broadcast was announced by neighbor
	if (!c->receiver.active)
	{
		if (header->flag == BULK_BROADCAST_FLAG_ANNOUNCE)
		{
			c->receiver.active = true;
			c->receiver.current_sender = sender;
			c->receiver.received_chunk_count = 0;
			size_t announced_size = *((size_t*)data);
#if __BULK_BROADCAST_DEBUG__
			printf("new broadcast of size %d was announced!\n", (int)announced_size);
#endif
			c->receiver.buffer = malloc(announced_size);
			c->receiver.announced_size = announced_size;
			timer_restart(&(c->receiver.timeout));
		}
	}
	else
	{
		// current sender has disappeared
		if (timer_expired(&(c->receiver.timeout)))
		{
#if __BULK_BROADCAST_DEBUG__
			printf("ERROR: current sender seems to disappeared; resetting receiver\n");
#endif
			reset_receiver(&(c->receiver));
			return;
		}

		// ignore if we are not currently listening to this sender
		if (rimeaddr_cmp(&(c->receiver.current_sender), &sender) == 0)
		{
			return;
		}

		// current sender announces another broadcast before properly finishing the last
		if (header->flag == BULK_BROADCAST_FLAG_ANNOUNCE)
		{
#if __BULK_BROADCAST_DEBUG__
			printf("ERROR: sender unexpectedly announced new broadcast; resetting receiver\n");
#endif
			reset_receiver(&(c->receiver));
			return;
		}


		timer_restart(&(c->receiver.timeout));

		c->receiver.received_chunk_count++;

		// copy chunk to buffer
		uintptr_t offset = header->chunk_id * BULK_BROADCAST_DATASIZE;
		memcpy(c->receiver.buffer + offset, data, header->data_length);

		if (header->flag == BULK_BROADCAST_FLAG_LASTCHUNK)
		{
			// check if all chunks have been received
			if (c->receiver.received_chunk_count * BULK_BROADCAST_DATASIZE < c->receiver.announced_size)
			{
#if __BULK_BROADCAST_DEBUG__
				printf("ERROR: packets have been lost in transmission; discarding buffer\n");
#endif
			}
			else if (c->callbacks->broadcast_received)
			{
				c->callbacks->broadcast_received(c, &sender, c->receiver.buffer, c->receiver.announced_size);
			}

			reset_receiver(&(c->receiver));
		}
	}
}

static void sent_by_abc(struct abc_conn *bc, int status, int num_tx)
{
	struct bulk_broadcast_conn *c = (struct bulk_broadcast_conn *)bc;

	if (!c->sender.active)
	{
#if __BULK_BROADCAST_DEBUG__
		printf("Finished sending!\n");
#endif
		return;
	}

	// calculate data size of next packet
	size_t remaining_bytes = c->sender.bytes - BULK_BROADCAST_DATASIZE * c->sender.current_chunk_id;
	size_t next_packet_size = remaining_bytes > BULK_BROADCAST_DATASIZE ? BULK_BROADCAST_DATASIZE : remaining_bytes;

	struct bulk_broadcast_header header;
	header.chunk_id = c->sender.current_chunk_id;
	header.data_length = next_packet_size;

	if (remaining_bytes <= BULK_BROADCAST_DATASIZE)
	{
		header.flag = BULK_BROADCAST_FLAG_LASTCHUNK;
		c->sender.active = false;
	}
	else
	{
		header.flag = BULK_BROADCAST_FLAG_CHUNK;
	}

	send_bulk_broadcast_packet(&(c->abc), header, c->sender.data);

	c->sender.current_chunk_id++;

	return;
}

static const struct abc_callbacks abc_callbacks = {recv_from_abc, sent_by_abc};

void bulk_broadcast_open(struct bulk_broadcast_conn *c, uint16_t channel, const struct bulk_broadcast_callbacks *cb)
{
	abc_open(&(c->abc), channel, &abc_callbacks);
	c->callbacks = cb;
	channel_set_attributes(channel, attributes);

	reset_receiver(&(c->receiver));
}

void bulk_broadcast_close(struct bulk_broadcast_conn *c)
{
	abc_close(&(c->abc));
}

int bulk_broadcast_send(struct bulk_broadcast_conn *c, void* data, size_t data_length)
{
	c->sender.active = true;
	c->sender.data = data;
	c->sender.bytes = data_length;
	c->sender.current_chunk_id = 0;

	struct bulk_broadcast_header header;
	header.flag = BULK_BROADCAST_FLAG_ANNOUNCE;
	header.chunk_id = 0;
	header.data_length = sizeof(data_length);

	return send_bulk_broadcast_packet(&c->abc, header, &data_length);
}
