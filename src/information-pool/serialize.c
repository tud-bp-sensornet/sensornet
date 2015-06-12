#include "contiki.h"
#include "net/rime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "serialize.h"
#include "graph.h"
#include "buffer.h"

void serializeNode(const p_node_t* node, uint16_t k, uint16_t i, struct buffer* buffer);
void serializeEdge(const p_edge_t* edge, uint16_t k, uint16_t i, struct buffer* buffer);

p_node_t* deserializeNode(void* basePointer, void* currentPointer);
p_edge_t* deserializeEdge(void* basePointer, void* currentPointer);

/**
 * Serializes the k-view of a graph.
 * node: the pointer to the root node of the graph
 * hop_count: how deep the tree should be crawled
 * bytes: points to a value where the length of the serialized data will be stored (in bytes)
 * return: void* to the start of the allocated memory
 */
void* serialize(p_graph_t* graph, uint8_t hop_count, size_t* bytes)
{
	size_t estimated_buffer_size = sizeof(p_node_t) * graph->num_nodes + sizeof(p_edge_t) * graph->num_edges;

	struct buffer *buffer = new_buffer(estimated_buffer_size);

	serializeNode(graph->root, hop_count, 0, buffer);

	void* serialized = buffer->data;
	*bytes = buffer->offset;

	delete_buffer(buffer);

	return serialized;
}

/**
 * copies node into the buffer at the current offset and calls serializeEdge, which in turn calls serializeNode
 * node: the edge to be copied into the buffer
 * k: how deep the tree should be crawled
 * i: the distance of node to the start node (current depth)
 * buffer: buffer used for storing the serialized data
 */
void serializeNode(const p_node_t* node, uint16_t k, uint16_t i, struct buffer *buffer)
{
	//copy the node into the buffer and move the offset forward according to the size of the struct
	reserve_space(buffer, sizeof(p_node_t));
	memcpy(buffer->data + buffer->offset, node, sizeof(p_node_t));
	p_node_t* currentNode = buffer->data + buffer->offset;
	buffer->offset += sizeof(p_node_t);

	//end the tree if already deep enough
	if(i < k)
	{
		//stop if it has no outgoing edges
		if(node->edges != NULL)
		{
			//set the relative pointer for edges and go further into the tree with k+1
			currentNode->edges = (p_edge_t*)(buffer->offset);
			serializeEdge(node->edges, k, i+1, buffer);
		}
	}
	else
	{
		//don't send the edges of the k-level nodes or deserialization doesn't work properly
		currentNode->edges = NULL;
	}
}

/*
 * copies edge into the buffer at the current offset and calls serializeNode, which in turn calls serializeEdge
 * edge: the edge to be copied into the buffer
 * k: how deep the tree should be crawled
 * i: the distance of node to the start node (current depth)
 * buffer: buffer used for storing the serialized data
 */
void serializeEdge(const p_edge_t* edge, uint16_t k, uint16_t i, struct buffer *buffer)
{
	//copy the edge to the currentPointer address and move the pointer forward according to the size of the struct
	reserve_space(buffer, sizeof(p_edge_t));
	memcpy(buffer->data + buffer->offset, edge, sizeof(p_edge_t));
	p_edge_t* currentEdge = buffer->data + buffer->offset;
	buffer->offset += sizeof(p_edge_t);

	//first go over every edge of the node until the list is empty
	if(edge->next != NULL)
	{
		currentEdge->next = (p_edge_t*)(buffer->offset);
		serializeEdge(edge->next, k, i, buffer);
	}

	//go into the drain and continue crawling
	currentEdge->drain = (p_node_t*)(buffer->offset);
	serializeNode(edge->drain, k , i, buffer);
}

/**
 * Deserializes a correctly serialized graph.
 * basePointer: pointer to the start of the serialized data
 * return: the root of the deserialized tree
 */
p_node_t* deserialize(void* basePointer)
{
	return deserializeNode(basePointer, basePointer);
}

/**
 * deserializes the current node and calls deserializeEdge, which calls deserializeNode again
 * basePointer: the pointer to the beginning of the serialized graph
 * currentPointer: the pointer to the position of the current node inside the serialized graph
 * return: the current node inside the tree
 */
p_node_t* deserializeNode(void* basePointer, void* currentPointer)
{
	//cast the pointer to this node
	p_node_t* currentNode = currentPointer;

	//go deeper into the tree if an edge exists (very error prone if the packet is damaged)
	if(currentNode->edges != NULL)
	{
		currentPointer = basePointer + (uintptr_t)(currentNode->edges);
		currentNode->edges = deserializeEdge(basePointer, currentPointer);
	}

	return currentNode;
}

/**
 * deserializes the current Edge and calls deserializeNode, which calls deserializeEdge again
 * basePointer: the pointer to the beginning of the serialized graph
 * currentPointer: the pointer to the position of the current edge inside the serialized graph
 * return: the current edge inside the tree
 */
p_edge_t* deserializeEdge(void* basePointer, void* currentPointer)
{
	//cast the pointer to this edge
	p_edge_t* currentEdge = currentPointer;

	//go deeper into the tree if an edge exists (very error prone if the packet is damaged)
	if(currentEdge->next != NULL)
	{
		currentPointer = basePointer + (uintptr_t)(currentEdge->next);
		currentEdge->next = deserializeEdge(basePointer, currentPointer);
	}

	//go deeper into the tree
	currentPointer = basePointer + (uintptr_t)(currentEdge->drain);
	currentEdge->drain = deserializeNode(basePointer, currentPointer);

	return currentEdge;
}
