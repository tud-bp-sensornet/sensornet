/**
 * \file
 *         Header file for identified best-effort bulk local area broadcast
 * \author
 *         Teriame
 */

#ifndef __BULK_BROADCAST_H__
#define __BULK_BROADCAST_H__

#include "net/rime/abc.h"
#include "net/rime/rimeaddr.h"

#define BULK_BROADCAST_ATTRIBUTES  { PACKETBUF_ADDR_SENDER, PACKETBUF_ADDRSIZE }, \
                        ABC_ATTRIBUTES

#define BULK_BROADCAST_DATASIZE 64

enum {
  BULK_BROADCAST_FLAG_NONE,
  BULK_BROADCAST_FLAG_LASTCHUNK,
};

/*---------------------------------------------------------------------------*/
struct bulk_broadcast_conn;

struct bulk_broadcast_callbacks {
  void (* write_chunk)(struct bulk_broadcast_conn *c, int offset, int flag, void *data, int len, const rimeaddr_t *sender);
  int (* read_chunk)(struct bulk_broadcast_conn *c, int offset, void *to, int maxsize);
};

struct bulk_broadcast_hdrpacket {
  uint16_t chunk;
  uint8_t datalen;
};

struct bulk_broadcast_datapacket {
  void* data;
};

struct bulk_broadcast_conn {
  struct abc_conn c;
  const struct bulk_broadcast_callbacks *cb;
  struct bulk_broadcast_datapacket current;
  struct bulk_broadcast_hdrpacket currenthdr;
};
/*---------------------------------------------------------------------------*/
void bulk_broadcast_open(struct bulk_broadcast_conn *c, uint16_t channel, const struct bulk_broadcast_callbacks *u);

void bulk_broadcast_close(struct bulk_broadcast_conn *c);

int bulk_broadcast_send(struct bulk_broadcast_conn *c);

#endif /* __BULK_BROADCAST_H__ */
/** @} */
/** @} */
