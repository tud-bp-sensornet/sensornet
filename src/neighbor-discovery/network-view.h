#include "net/rime.h"

#ifndef __NETWORK_VIEW_H__
#define __NETWORK_VIEW_H__

#define MAX_NODES 32

typedef struct {
    struct node_t *next;
    rimeaddr_t node_addr;
    int hop_count;
    int last_update;
} node_t;

void init_network_view();

list_t get_known_nodes();

void debug_print_nodes();

#endif
