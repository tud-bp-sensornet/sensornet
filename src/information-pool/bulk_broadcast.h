#ifndef __BULK_BROADCAST_H__
#define __BULK_BROADCAST_H__

#include <stdbool.h>
#include "sys/timer.h"
#include "net/rime/abc.h"
#include "net/rime/rimeaddr.h"
#include "buffer.h"

#define BULK_BROADCAST_ATTRIBUTES  { PACKETBUF_ADDR_SENDER, PACKETBUF_ADDRSIZE }, \
                        ABC_ATTRIBUTES

#define BULK_BROADCAST_DATASIZE 64

#define BULK_BROADCAST_RECEIVE_TIMEOUT (CLOCK_SECOND * 20)

typedef enum {
	BULK_BROADCAST_FLAG_ANNOUNCE,
	BULK_BROADCAST_FLAG_CHUNK,
	BULK_BROADCAST_FLAG_LASTCHUNK,
} bulk_broadcast_flag;

/*---------------------------------------------------------------------------*/

struct bulk_broadcast_conn;

struct bulk_broadcast_callbacks {
	void (* broadcast_received)(const struct bulk_broadcast_conn *bc, const rimeaddr_t *sender, void *data, size_t length);
};

struct bulk_broadcast_header {
	bulk_broadcast_flag flag;
	uint16_t chunk_id; // uint8_t won't make the header smaller for some reason; remaining byte would be padded with random data
	size_t data_length;
};

struct bulk_broadcast_receive_task {
	bool active;
	rimeaddr_t current_sender;
	uint8_t received_chunk_count;
	size_t announced_size;
	void* buffer;
	struct timer timeout;
};

struct bulk_broadcast_send_task {
	bool active;
	void* data;
	size_t bytes;
	uint8_t current_chunk_id;
};

struct bulk_broadcast_conn {
	struct abc_conn abc;
	const struct bulk_broadcast_callbacks *callbacks;
	struct bulk_broadcast_receive_task receiver;
	struct bulk_broadcast_send_task sender;
};

/*---------------------------------------------------------------------------*/

void bulk_broadcast_open(struct bulk_broadcast_conn *c, uint16_t channel, const struct bulk_broadcast_callbacks *u);

void bulk_broadcast_close(struct bulk_broadcast_conn *c);

int bulk_broadcast_send(struct bulk_broadcast_conn *c, void* data, size_t data_length);

#endif /* __BULK_BROADCAST_H__ */
