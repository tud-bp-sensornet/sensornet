#define __SERIALIZE_DEBUG__ 0

#include "contiki.h"
#include "net/rime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "serialize.h"
#include "graph.h"
#include "buffer.h"

intptr_t findNodeInBuffer(const struct buffer *buffer, const rimeaddr_t *addr);
intptr_t findNodeInBuffer_Node(const void* base, const p_node_t* current, const rimeaddr_t *addr);
intptr_t findNodeInBuffer_Edge(const void* base, const p_edge_t* current, const rimeaddr_t *addr);

void serializeNode(const p_node_t* node, uint16_t k, uint16_t i, struct buffer* buffer);
void serializeEdge(const p_edge_t* edge, uint16_t k, uint16_t i, struct buffer* buffer);
void updateExistingNodeInBuffer(const p_node_t* node, p_node_t* nodeInBuffer, uint16_t k, uint16_t i, struct buffer *buffer);

p_node_t* deserializeNode(void* basePointer, p_node_t* currentNode);
p_edge_t* deserializeEdge(void* basePointer, p_edge_t* currentEdge);

/**
 * Helper method to find if a certain node is already serialized in the buffer.
 * If it is, return offset to the node. If it isn't, returns -1.
 */
intptr_t findNodeInBuffer(const struct buffer *buffer, const rimeaddr_t *addr)
{
	return findNodeInBuffer_Node(buffer->data, buffer->data, addr);
}

intptr_t findNodeInBuffer_Node(const void* base, const p_node_t* current, const rimeaddr_t *addr)
{
	if (rimeaddr_cmp(&(current->addr), addr) != 0)
	{
		intptr_t offset = (intptr_t)current - (intptr_t)base;
		return offset;
	}

	if (current->edges != NULL)
	{
		return findNodeInBuffer_Edge(base, base + (uintptr_t)(current->edges), addr);
	}

	return -1;
}

intptr_t findNodeInBuffer_Edge(const void* base, const p_edge_t* current, const rimeaddr_t *addr)
{
	if (current->drain != NULL)
	{
		return findNodeInBuffer_Node(base, base + (uintptr_t)(current->drain), addr);
	}

	if (current->next != NULL)
	{
		return findNodeInBuffer_Edge(base, base + (uintptr_t)(current->next), addr);
	}

	return -1;
}

#if __SERIALIZE_DEBUG__
void dump_buffer(struct buffer* buffer)
{
	printf("[Serialize] buffer: ");
	void* data = buffer->data;
	int j;
	for (j = 0; j < buffer->offset; j++)
	{
		printf("%02X ", ((char*)data)[j] & 0xFF);
	}
	printf("\n");
}
#endif

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
#if __SERIALIZE_DEBUG__
	printf("[Serialize] Adding node %p (%d.%d) ...\n", node, node->addr.u8[0], node->addr.u8[1]);
#endif

	// copy the node into the buffer and move the offset forward according to the size of the struct
	reserve_space(buffer, sizeof(p_node_t));
	memcpy(buffer->data + buffer->offset, node, sizeof(p_node_t));

	p_node_t* currentNodeInBuffer = buffer->data + buffer->offset;
	currentNodeInBuffer->edges = NULL;

	buffer->offset += sizeof(p_node_t);

#if __SERIALIZE_DEBUG__
	dump_buffer(buffer);
#endif

	// end the tree if already deep enough
	if (i < k)
	{
		// stop if it has no outgoing edges
		if (node->edges != NULL)
		{
			// set the relative pointer for edges and go further into the tree with k+1
			currentNodeInBuffer->edges = (p_edge_t*)(buffer->offset);
			serializeEdge(node->edges, k, i+1, buffer);
		}
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
	const p_edge_t *pivot = edge;

	while (pivot != NULL)
	{
#if __SERIALIZE_DEBUG__
	printf("[Serialize] Adding edge %p to %d.%d ...\n", pivot, pivot->drain->addr.u8[0], pivot->drain->addr.u8[1]);
#endif
		reserve_space(buffer, sizeof(p_edge_t));
		memcpy(buffer->data + buffer->offset, pivot, sizeof(p_edge_t));

		p_edge_t* currentEdgeInBuffer = buffer->data + buffer->offset;
		currentEdgeInBuffer->drain = NULL;
		currentEdgeInBuffer->next = NULL;

		buffer->offset += sizeof(p_edge_t);

		intptr_t nodeOffset = findNodeInBuffer(buffer, &(pivot->drain->addr));

#if __SERIALIZE_DEBUG__
		dump_buffer(buffer);
		printf("[Serialize] Find node %d.%d in buffer: %x\n", pivot->drain->addr.u8[0], pivot->drain->addr.u8[1], (int)nodeOffset);
#endif

		if (nodeOffset != -1)
		{
			currentEdgeInBuffer->drain = (p_node_t*)nodeOffset;

			// node may already be in buffer, but have missing edges
			if (i < k)
			{
				updateExistingNodeInBuffer(pivot->drain, (p_node_t*)(buffer->data + nodeOffset), k, i, buffer);
			}
		}
		else
		{
			currentEdgeInBuffer->drain = (p_node_t*)buffer->offset;
			serializeNode(pivot->drain, k , i, buffer);
		}

		pivot = pivot->next;

		if (pivot != NULL)
		{
			currentEdgeInBuffer->next = (p_edge_t*)buffer->offset;
		}
	}
}

void updateExistingNodeInBuffer(const p_node_t* node, p_node_t* nodeInBuffer, uint16_t k, uint16_t i, struct buffer *buffer)
{
	if (nodeInBuffer->edges == NULL && node->edges != NULL)
	{
		nodeInBuffer->edges = (p_edge_t*)(buffer->offset);
		serializeEdge(node->edges, k, i+1, buffer);
	}
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
 * currentNode: the pointer to the position of the current node inside the serialized graph
 * return: the current node inside the tree
 */
p_node_t* deserializeNode(void* basePointer, p_node_t* currentNode)
{
	// go deeper into the tree if an edge exists (very error prone if the packet is damaged)
	if (currentNode->edges != NULL)
	{
		currentNode->edges = deserializeEdge(basePointer, basePointer + (uintptr_t)(currentNode->edges));
	}

	return currentNode;
}

/**
 * deserializes the current Edge and calls deserializeNode, which calls deserializeEdge again
 * basePointer: the pointer to the beginning of the serialized graph
 * currentEdge: the pointer to the position of the current edge inside the serialized graph
 * return: the current edge inside the tree
 */
p_edge_t* deserializeEdge(void* basePointer, p_edge_t* currentEdge)
{
	// go deeper into the tree if an edge exists (very error prone if the packet is damaged)
	if (currentEdge->next != NULL)
	{
		currentEdge->next = deserializeEdge(basePointer, basePointer + (uintptr_t)(currentEdge->next));
	}

	// only deserialize node if it occurs further into the buffer; otherwise it has already been deserialized
	// TODO may not be bug free --> test this again
	if ((uintptr_t)basePointer + (uintptr_t)(currentEdge->drain) > (uintptr_t)currentEdge)
	{
		currentEdge->drain = deserializeNode(basePointer, basePointer + (uintptr_t)(currentEdge->drain));
	}
	else
	{
		currentEdge->drain = basePointer + (uintptr_t)(currentEdge->drain);
	}


	return currentEdge;
}
