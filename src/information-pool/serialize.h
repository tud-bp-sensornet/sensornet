#include "net/rime.h"
#include "graph.h"

#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

void* serialize(p_node_t * p_node, unsigned short k, uint16_t maxNodes, uint16_t maxEdges, uint16_t* countNode, uint16_t* countEdge);

void* serializeNode(p_node_t * p_node, unsigned short k, unsigned short current_k, void* currentPointer, void* startPointer, uint16_t* countNode, uint16_t* countEdge);

void* serializeEdge(p_edge_t * p_edge, unsigned short k, unsigned short current_k, void* currentPointer, void* startPointer, uint16_t* countNode, uint16_t* countEdge);

p_node_t* deserialize(void* startPointer);

p_node_t* deserializeNode(void* startPointer, void* currentPointer);

p_edge_t* deserializeEdge(void* startPointer, void* currentPointer);


#endif
