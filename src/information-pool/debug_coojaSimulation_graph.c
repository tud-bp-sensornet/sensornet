#include "contiki.h"
#include "net/rime.h"

#include "graph.h"
#include "graph-operations.h""
#include "debug_coojaSimulation_graph.h"

/**
 * Prints the graph by printing Node informations and Edge informations while iterating it
 * Testcase:Node:%d,%d\n	with the rimeaddr.u8[0] and the depth
 * Testcase:Edge:%d,%d\n	with the rimeaddr.u8[0] of the source and the rimeaddr.u8[0] of the drain
 * graph: the graph that should be printed
 */
void debug_printGraph(){
	printf("Print Graph for Node %d\n", rimeaddr_node_addr);
	uint8_t count = 0;

	//print nodes
	p_hop_t *hops = get_hop_counts(&count);
	uint8_t i;
	for (i = 0; i < count; i++)
	{
		printf("Testcase:Node:%d,%d\n", hops[i].addr.u8[0],hops[i].hop_count);
	}
	free(hops);

	//print edges
	p_edge_t **edge_array = get_all_edges(&count);
	for (i = 0; i < count; i++)
	{
		printf("Testcase:Edge:%d,%d\n", edge_array[i]->src.u8[0], edge_array[i]->dst.u8[0]);
	}
}
