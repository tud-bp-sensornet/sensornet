#include <stddef.h>
#include "net/rime.h"
#include "graph.h"

#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

void* serialize(p_graph_t* graph,
	uint8_t hop_count, // most likely K-1
	size_t* bytes);

p_node_t* deserialize(void* buffer);

#endif
