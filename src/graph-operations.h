#ifndef __GRAPH_OPERATIONS_H__
#define __GRAPH_OPERATIONS_H__

#include "graph.h"
#include "contiki.h"

//returns the shortest hop count from a node to another node
//by traversing the graph
//if from or to are not in the graph, -1 is returned.
int8_t get_hop_count(const rimeaddr_t* from, const rimeaddr_t* to);

#endif
