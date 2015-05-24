#include "contiki.h"
#include "net/rime.h"
#include <stdio.h>

#include "serialize.h"

/**
 * serializes the k-view of the known tree
 * p_node: the pointer to the p_node of the node itself
 * k: how deep the tree should be crawled
 */
void* serialize(p_node_t * p_node, unsigned short k) {

	//very bad guess how much memory should be allocated. should be changed tbh
	void* startPointer = malloc(sizeof(p_node_t)*(k+1)*3+sizeof(p_node_t)*(k+1)*3*3);
	void* currentPointer = startPointer;

	currentPointer = crawlNode(p_node, k, 0, currentPointer, startPointer);

	return startPointer;
}

/**
 * copys p_node into the buffer at the position of currentPointer and calls crawlEdge, which in turn calls crawlNode
 * p_node: the edge to be copied into the buffer
 * k: how deep the tree should be crawled
 * current_k: the distance of p_node to the start node
 * currentPointer: void pointer that keeps track of the current place inside the buffer in which information will be written
 * startPointer: the beginning of buffer. used for calculating relativ position inside the buffer
 */
void* crawlNode(p_node_t * p_node, unsigned short k, unsigned short current_k, void* currentPointer, void* startPointer){

	//copy the node to the currentPointer address and move the pointer forward according to the size of the struct
	*((p_node_t*)currentPointer) = *(p_node);
	void* tempptr = currentPointer;
	currentPointer = currentPointer+sizeof(p_node_t);

	//end the tree if already deep enough
	if(current_k < k){
		//stop if it has no outgoing edges
		if(p_node->edges != NULL){
			//set the relativ pointer for edges and go further into the tree with k+1
			((p_node_t*)tempptr)->edges = (p_edge_t*)(currentPointer-startPointer);
			currentPointer = crawlEdge(p_node->edges, k, current_k+1, currentPointer, startPointer);
		}
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
 */
void* crawlEdge(p_edge_t * p_edge, unsigned short k, unsigned short current_k, void* currentPointer, void* startPointer){

	//copy the edge to the currentPointer address and move the pointer forward according to the size of the struct
	*((p_edge_t*)currentPointer) = *(p_edge);
	void* tempptr = currentPointer;
	currentPointer = currentPointer+sizeof(p_edge_t);

	//first go over every edge of the node until the list is empty
	if(p_edge->next != NULL){
		((p_edge_t*)tempptr)->next = (p_edge_t*)(currentPointer-startPointer);
		currentPointer = crawlEdge(p_edge->next, k, current_k, currentPointer, startPointer);
	}

	//go into the drain and continue crawling
	((p_edge_t*)tempptr)->drain = (p_node_t*)(currentPointer-startPointer);
	currentPointer = crawlNode(p_edge->drain, k , current_k, currentPointer, startPointer);

	return currentPointer;
}
