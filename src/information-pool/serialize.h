#include "net/rime.h"

#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

void* serialize(p_node_t * p_node, unsigned short k);

void* crawlNode(p_node_t * p_node, unsigned short k, unsigned short current_k, void* currentPointer, void* startPointer);

void* crawlEdge(p_edge_t * p_edge, unsigned short k, unsigned short current_k, void* currentPointer, void* startPointer);


#endif
