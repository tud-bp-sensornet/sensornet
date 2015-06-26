#ifndef __BROADCAST_H__
#define __BROADCAST_H__

struct p_broadcast_conn;

struct p_broadcast_conn {
    struct abc_conn abc;

    void (*received)(const struct p_broadcast_conn *bc, const rimeaddr_t *sender, void *data, size_t length);
};

void p_broadcast_open(struct p_broadcast_conn *c, uint16_t channel);

void p_broadcast_close(struct p_broadcast_conn *c);

int p_broadcast_send(struct p_broadcast_conn *c, void *data, size_t length);

#endif