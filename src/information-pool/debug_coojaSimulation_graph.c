#include "contiki.h"
#include "net/rime.h"

#include "graph.h"
#include "debug_coojaSimulation_graph.h"

void debug_printGraphNode(p_node_t* p_node, uint16_t i);
void debug_printGraphEdge(p_node_t* source_node, p_edge_t* p_edge, uint16_t i);

/**
 * Prints the graph by printing Node informations and Edge informations while iterating it
 * Testcase:Node:%d,%d\n	with the rimeaddr.u8[0] and the depth
 * Testcase:Edge:%d,%d\n	with the rimeaddr.u8[0] of the source and the rimeaddr.u8[0] of the drain
 * graph: the graph that should be printed
 */
void debug_printGraph(p_graph_t* graph){
	printf("Print Graph for Node %d\n", rimeaddr_node_addr);
	debug_printGraphNode(graph->root,0);
}

/**
 * Prints the current Node
 * Testcase:Node:%d,%d\n	with the rimeaddr.u8[0] and the depth
 * p_node: the current Node
 * i: the current depth of the graph
 */
void debug_printGraphNode(p_node_t* p_node, uint16_t i){
	printf("Testcase:Node:%d,%d\n", p_node->addr.u8[0],i);

	//Only go to the depth of K
	if(i<K){
		//If the node has an edge, iterate it
		if(p_node->edges != NULL){
			debug_printGraphEdge(p_node, p_node->edges, i+1);
		}
	}
}

/**
 * Prints the current Edge
 * Testcase:Edge:%d,%d\n	with the rimeaddr.u8[0] of the source and the rimeaddr.u8[0] of the drain
 * source_node: the source of p_edge
 * p_edge: the current Edge
 * i: the current depth of the graph
 */
void debug_printGraphEdge(p_node_t* source_node, p_edge_t* p_edge, uint16_t i){
	printf("Testcase:Edge:%d,%d\n", source_node->addr.u8[0],p_edge->drain->addr.u8[0]);

	//If the source_node has more edges, iterate them
	if(p_edge->next != NULL){
		debug_printGraphEdge(source_node, p_edge->next, i);
	}
	//Iterate the drain of the Edge
	debug_printGraphNode(p_edge->drain, i);
}
