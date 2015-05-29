#include "contiki.h"
#include "net/rime.h"
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"

/**
 * Start iterating over the deserialized update and adjust the memory with the new information. frees the deserialized space
 * p_node: start of the deserialized space
 * node_memory: pointer to the MEMB memory for nodes
 * edge_memory: pointer to the MEMB memory for edges
 * this_mote_node: pointer to the node information of the k-0 node in MEMB
 */
void iterateUpdate(p_node_t * p_node, void* node_memory, void* edge_memory, p_node_t * this_mote_node){

	p_node_t *this_node = iterateUpdateNode(p_node, node_memory, edge_memory, this_mote_node);

	updateNeighbour(this_node, node_memory, edge_memory, this_mote_node);

	//free the allocated space from the deserialization
	free((void*) p_node);
}

/**
 * iterate over the nodes of the update and adjust the MEMB when necessary
 * p_node: pointer to the current node in the deserialized update
 * node_memory: pointer to the MEMB memory for nodes
 * edge_memory: pointer to the MEMB memory for edges
 * this_mote_node: pointer to the node information of the k-0 node in MEMB
 * return: the address of the node inside MEMB
 */
p_node_t* iterateUpdateNode(p_node_t * p_node, void* node_memory, void* edge_memory, p_node_t * this_mote_node){
	p_node_t *this_node;

	p_node->edges = iterateUpdateEdge(p_node->edges, *p_node, node_memory, edge_memory, this_mote_node);

	p_node_t *search_node = findNodeAddr (this_mote_node, p_node->addr);
	if(search_node != NULL){
		this_node = handleExistingNode(search_node, p_node);
	} else {
		//this_node = memb_alloc(&node_memory);
		*this_node = *p_node;
	}

	return this_node;
}

/**
 * iterate over the edges of the update and adjust the MEMB when necessary
 * p_edge: pointer to the current node in the deserialized update
 * source_node: pointer to the source of the edge
 * node_memory: pointer to the MEMB memory for nodes
 * edge_memory: pointer to the MEMB memory for edges
 * this_mote_node: pointer to the node information of the k-0 node in MEMB
 * return: the address of the edge inside MEMB
 */
p_edge_t* iterateUpdateEdge(p_edge_t * p_edge, p_node_t source_node, void* node_memory, void* edge_memory, p_node_t * this_mote_node){
	p_edge_t *this_edge;
	//this_edge = memb_alloc(&edge_memory);

	//Needs to handle already existing edges to not allocate unnecessary space

	this_edge->drain = iterateUpdateNode(p_edge->drain, node_memory, edge_memory, this_mote_node);
	this_edge->next = iterateUpdateEdge(p_edge->next, source_node, node_memory, edge_memory, this_mote_node);

	return this_edge;
}

/**
 * the direct neighbours of this_mote_node need to be handled differently to create new edges and update last_seen
 * p_node: pointer to the current node in the deserialized update
 * node_memory: pointer to the MEMB memory for nodes
 * edge_memory: pointer to the MEMB memory for edges
 * this_mote_node: pointer to the node information of the k-0 node in MEMB
 */
void updateNeighbour(p_node_t * p_node, void* node_memory, void* edge_memory, p_node_t * this_mote_node){
	p_edge_t *new_edge;
	//new_edge = memb_alloc(&edge_memory);

	new_edge->drain = p_node;
	new_edge->next = NULL;
	//p_node->last_seen = ??;

	if(this_mote_node->edges == NULL){
		this_mote_node->edges = new_edge;
	} else {
		//if the edge does not already exist...
		updateEdgeList(this_mote_node->edges, new_edge);
	}
}

/**
 * put the edge at the end of the list
 * mote_p_edge: the current edge
 * new_edge: the edge to be inserted
 */
void updateEdgeList(p_edge_t * mote_p_edge, p_edge_t * new_edge){
	if(mote_p_edge->next == NULL){
		mote_p_edge->next = new_edge;
	} else {
		updateEdgeList(mote_p_edge->next, new_edge);
	}
}

/**
 * if a node already exists then the update can't be just thrown into MEMB
 * this function handles this problem
 */
p_node_t* handleExistingNode(p_node_t * old_p_node, p_node_t * new_p_node){
	p_node_t *return_node;

	return return_node;
}

/**
 * finds and returns the node which has the specified rimeaddr if it exists in MEMB. if it doesn't exist, return NULL
 */
p_node_t* findNodeAddr(p_node_t * this_mote_node, rimeaddr_t addr){
	p_node_t *this_node;

	return this_node;
}

/**
 * handle edges already in MEMB
 */
p_edge_t* edgeAlreadyExists(p_edge_t * p_edge, p_node_t * source_node, p_node_t * this_mote_node){
	p_edge_t *return_edge;

	return return_edge;
}
