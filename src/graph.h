#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "contiki.h"
#include "net/rime.h"

//needs to be called once at the start of main to initialize space in the memory
void init_graph();

typedef struct {
    rimeaddr_t addr;
    //possibly additional attributes, like position
} p_node_t;

void add_node(const p_node_t node);

void remove_node(const rimeaddr_t *addr);

//returns array of pointers to all nodes, count will contain number of nodes
p_node_t **get_all_nodes(uint8_t * count);

//returns the count of all nodes
uint8_t get_node_count();

//returns the node with given rimeaddr_t*, or null if not found
p_node_t *find_node(const rimeaddr_t *addr);

typedef struct {
    rimeaddr_t src;
    rimeaddr_t dst;
    uint8_t ttl;
} p_edge_t;

void add_edge(const p_edge_t edge);

void remove_edge(const rimeaddr_t *src, const rimeaddr_t *dst);

//returns array of pointers to all edges, count will contain number of edges
p_edge_t **get_all_edges(uint8_t * count);

p_edge_t **get_outgoing_edges(const rimeaddr_t *src_addr, uint8_t *count);

p_edge_t **get_ingoing_edges(const rimeaddr_t *dst_addr, uint8_t *count);

//returns the edge with given src and destination, or null if not found
p_edge_t *find_edge(const rimeaddr_t *src, const rimeaddr_t *dst);

//returns the count of all edges
uint8_t get_edge_count();

#endif
