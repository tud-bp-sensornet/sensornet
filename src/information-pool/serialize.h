#include <stdlib.h>
#include "net/rime.h"
#include "graph.h"

#ifndef __SERIALIZE_H__
#define __SERIALIZE_H__

void* serialize(const p_node_t *node, uint16_t k, size_t *length);
p_node_t* deserialize(void *basePointer);

#endif
