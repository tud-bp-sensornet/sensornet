/**
 * \file
 * 			Identified broadcast Rime layer ontop of anonymous broadcast (abc)
 * \author
 * 			tud-bp-sensornet
 */

#include "contiki-net.h"
#include <string.h>

static const struct packetbuf_attrlist attributes[] =
  {
    BROADCAST_ATTRIBUTES PACKETBUF_ATTR_LAST
  };

/*---------------------------------------------------------------------------*/
static void
recv_from_abc(struct abc_conn *bc)
{
  rimeaddr_t sender;
  struct p_broadcast_conn *c = (struct p_broadcast_conn *)bc;

  rimeaddr_copy(&sender, packetbuf_addr(PACKETBUF_ADDR_SENDER));
  
  if(c->received) {
    c->received(c, &sender, packetbuf_dataptr(), (size_t)packetbuf_datalen());
  }
}
/*---------------------------------------------------------------------------*/
static const struct abc_callbacks broadcast = {recv_from_abc};
/*---------------------------------------------------------------------------*/
void
p_broadcast_open(struct p_broadcast_conn *c, uint16_t channel)
{
  abc_open(&c->abc, channel, &broadcast);
  channel_set_attributes(channel, attributes);
}
/*---------------------------------------------------------------------------*/
void
p_broadcast_close(struct p_broadcast_conn *c)
{
  abc_close(&c->abc);
}
/*---------------------------------------------------------------------------*/
int
p_broadcast_send(struct p_broadcast_conn *c, void *data, size_t length)
{
  packetbuf_clear ();
  packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &rimeaddr_node_addr);
  if(packetbuf_copyfrom(data, (uint16_t)length) != (int)length){
    //TODO:Errorhandling
  }
  return abc_send(&c->abc);
}
/*---------------------------------------------------------------------------*/
