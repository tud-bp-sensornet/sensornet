#include "contiki.h"
#include "net/rime.h"
#include <stdio.h>
#include <stdlib.h>

#include "serialize.h"
#include "graph.h"

/**
 * serializes the k-view of the known tree
 * p_node: the pointer to the p_node of the node itself
 * k: how deep the tree should be crawled
 * maxNodes: the currently known amount of nodes
 * maxEdges: the currently known amount of edges
 * countNode: uint16_t* which will point to the counted amount of nodes in the serialized k depth of the graph
 * countEdge: uint16_t* which will point to the counted amount of edges in the serialized k depth of the graph
 * return: void* to the start of the allocated memory
 */
void* serialize(p_node_t * p_node, unsigned short k, uint16_t maxNodes, uint16_t maxEdges, uint16_t* countNode, uint16_t* countEdge) {

	//allocate enough memory to fit in all information the node knows
	void* startPointer = malloc(sizeof(p_node_t)*maxNodes+sizeof(p_edge_t)*maxEdges);
	void* currentPointer = startPointer;

	//start counting from 0
	*countNode = 0;
	*countEdge = 0;

	currentPointer = serializeNode(p_node, k, 0, currentPointer, startPointer, countNode, countEdge);

	return startPointer;
}

/**
 * copys p_node into the buffer at the position of currentPointer and calls crawlEdge, which in turn calls crawlNode
 * p_node: the edge to be copied into the buffer
 * k: how deep the tree should be crawled
 * current_k: the distance of p_node to the start node
 * currentPointer: void pointer that keeps track of the current place inside the buffer in which information will be written
 * startPointer: the beginning of buffer. used for calculating relativ position inside the buffer
 * countNode: uint16_t* which will point to the counted amount of nodes in the serialized k depth of the graph
 * countEdge: uint16_t* which will point to the counted amount of edges in the serialized k depth of the graph
 * return: void* to the start of the still free space of the allocated space in serialize
 */
void* serializeNode(p_node_t * p_node, unsigned short k, unsigned short current_k, void* currentPointer, void* startPointer, uint16_t* countNode, uint16_t* countEdge){

	//copy the node to the currentPointer address and move the pointer forward according to the size of the struct
	*((p_node_t*)currentPointer) = *(p_node);
	void* tempptr = currentPointer;
	currentPointer = currentPointer+sizeof(p_node_t);
	*countNode = (*countNode)+1;

	//end the tree if already deep enough
	if(current_k < k){
		//stop if it has no outgoing edges
		if(p_node->edges != NULL){
			//set the relative pointer for edges and go further into the tree with k+1
			((p_node_t*)tempptr)->edges = (p_edge_t*)(currentPointer-startPointer);
			currentPointer = serializeEdge(p_node->edges, k, current_k+1, currentPointer, startPointer, countNode, countEdge);
		}
	} else {
		//don't send the edges of the k-level nodes or deserialization doesn't work properly
		((p_node_t*)tempptr)->edges = NULL;
	}

	return currentPointer;
}

/*
 * copys p_edge into the buffer at the position of currentPointer and calls crawlNode, which in turn calls crawlEdge
 * p_edge: the edge to be copied into the buffer
 * k: how deep the tree should be crawled
 * current_k: the distance of p_node to the start node
 * currentPointer: void pointer that keeps track of the current place inside the buffer in which information will be written
 * startPointer: the beginning of buffer. used for calculating relativ position inside the buffer
 * countNode: uint16_t* which will point to the counted amount of nodes in the serialized k depth of the graph
 * countEdge: uint16_t* which will point to the counted amount of edges in the serialized k depth of the graph
 * return: void* to the start of the still free space of the allocated space in serialize
 */
void* serializeEdge(p_edge_t * p_edge, unsigned short k, unsigned short current_k, void* currentPointer, void* startPointer, uint16_t* countNode, uint16_t* countEdge){

	//copy the edge to the currentPointer address and move the pointer forward according to the size of the struct
	*((p_edge_t*)currentPointer) = *(p_edge);
	void* tempptr = currentPointer;
	currentPointer = currentPointer+sizeof(p_edge_t);
	*countEdge = (*countEdge)+1;

	//first go over every edge of the node until the list is empty
	if(p_edge->next != NULL){
		((p_edge_t*)tempptr)->next = (p_edge_t*)(currentPointer-startPointer);
		currentPointer = serializeEdge(p_edge->next, k, current_k, currentPointer, startPointer, countNode, countEdge);
	}

	//go into the drain and continue crawling
	((p_edge_t*)tempptr)->drain = (p_node_t*)(currentPointer-startPointer);
	currentPointer = serializeNode(p_edge->drain, k , current_k, currentPointer, startPointer, countNode, countEdge);

	return currentPointer;
}

/**
 * deserializes a correctly serialized graph
 * startPointer: pointer to the start of the serialized data
 * return: the root of the deserialized tree
 */
p_node_t* deserialize(void* startPointer){

	void* currentPointer = startPointer;

	p_node_t *this_node;
	this_node = deserializeNode(startPointer, currentPointer);

	return this_node;
}

/**
 * deserializes the current Node and calls deserializeEdge, which calls deserializeNode again
 * startPointer: the pointer to the beginning of the serialized graph
 * currentPointer: the pointer to the position of the current node inside the serialized graph
 * return: the current node inside the tree
 */
p_node_t* deserializeNode(void* startPointer, void* currentPointer){

	//cast the pointer to this node
	p_node_t *this_node;
	this_node = (p_node_t*)currentPointer;

	//go deeper into the tree if an edge exists (very error prone if the packet is damaged)
	if(this_node->edges != NULL){
		currentPointer = (void*)startPointer+(uint16_t)(this_node)->edges;
		this_node->edges = deserializeEdge(startPointer, currentPointer);
	}

	return this_node;
}

/**
 * deserializes the current Edge and calls deserializeNode, which calls deserializeEdge again
 * startPointer: the pointer to the beginning of the serialized graph
 * currentPointer: the pointer to the position of the current edge inside the serialized graph
 * return: the current edge inside the tree
 */
p_edge_t* deserializeEdge(void* startPointer, void* currentPointer){

	//cast the pointer to this edge
	p_edge_t *this_edge;
	this_edge = (p_edge_t*)currentPointer;

	//go deeper into the tree if an edge exists (very error prone if the packet is damaged)
	if(this_edge->next != NULL){
	currentPointer = (void*)startPointer+(uint16_t)(this_edge)->next;
	this_edge->next = deserializeEdge(startPointer, currentPointer);
	}

	//go deeper into the tree
	currentPointer = (void*)startPointer+(uint16_t)(this_edge)->drain;
	this_edge->drain = deserializeNode(startPointer, currentPointer);

	return this_edge;
}
