#include "net/rime.h"
#include "graph.h"

#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

void* serialize(const p_node_t *p_node, uint16_t k, uint16_t maxNodes, uint16_t maxEdges, uint16_t* countNode, uint16_t* countEdge);
p_node_t* deserialize(void *startPointer);

#endif
