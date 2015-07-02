/**
 * \file                       A serializer and deserializer
 * \author                     tud-bp-sensornet
 */

#include "serialize.h"
#include "graph.h"

void serialize(void (*packet_complete)(const void *packet_data, size_t length))
{

	//On K==0 no information will be exchanged
	//If minimal package length (2 Nodes and one Edge) is bigger than PACKETBUF_SIZE, do nothing.
	if (K == 0 || (sizeof(p_node_t) * 2 + sizeof(p_edge_t)) > PACKETBUF_SIZE)
	{
		return;
	}

	//Single memory to avoid fragementation
	//Memory layout: p_node_t src, (p_edge_t src_drain, p_node_t drain)*
	void *memory = malloc(PACKETBUF_SIZE);
	void *memory_ptr = memory;

	//Get all reachable nodes and their hopcount
	uint8_t reachable_count;
	p_hop_t *hops = get_hop_counts(&reachable_count);

	//Package root (on K >= 1) (reachable in 0 hops)
	memcpy(memory_ptr, find_node(&rimeaddr_node_addr), sizeof(p_node_t));
	memory_ptr = memory_ptr + sizeof(p_node_t);

	//Package all outgoing edges and edge destinations from root (on K >= 2) (reachable in 1 hops)
	if (K >= 2)
	{
		package_and_send_edges(memory_ptr, &rimeaddr_node_addr);
	} else {
		return;
	}

	//Package nodes and their edges reachable in up to K-1 hops
	uint8_t l;
	for (l = 0; l < reachable_count; l++)
	{
		if (hops[l].hop_count <= K - 1)
		{
			//Package this node
			memcpy(memory_ptr, find_node(&(hops[l].addr)), sizeof(p_node_t));
			memory_ptr = memory_ptr + sizeof(p_node_t);
			package_and_send_edges(memory_ptr, &(hops[l].addr));
		}
	}

	if (edge_ptr_ptr != NULL) {
		free(edge_ptr_ptr);
	}

	free(memory);
}

void package_and_send_edges(void *memory_ptr, rimeaddr_t *src)
{
	//Get all outgoing edges from that node
	uint8_t edge_count;
	p_edge_t **edge_ptr_ptr = get_outgoing_edges(src, &edge_count);

	//Package every edge and drain
	uint8_t j = (uint8_t)sizeof(p_node_t);
	uint8_t k;
	for (k = 0; k < *edge_count; k++)
	{
		//Pack outgoing edge
		memcpy(memory_ptr, edge_ptr_ptr[k], sizeof(p_edge_t));
		memory_ptr = memory_ptr + sizeof(p_edge_t);

		//Pack drain of edge
		p_node_t *drain = find_node(&(edge_ptr_ptr[i]->dst));
		memcpy(memory_ptr, drain, sizeof(p_node_t));
		memory_ptr = memory_ptr + sizeof(p_node_t);

		//Package will get too big in next iteration
		if (j + (sizeof(p_node_t) + sizeof(p_edge_t)) < PACKETBUF_SIZE && k + 1 < *edge_count)
		{
			//Send subgraph
			packet_complete(memory, (size_t)j);

			//Reset size and packet
			j = sizeof(p_node_t);
			memory_ptr = memory_ptr + sizeof(p_node_t);
		}

		//Increment package size counter
		j = j + (sizeof(p_node_t) + sizeof(p_edge_t));
	}

	//Send subgraph
	packet_complete(memory, (size_t)j);
}

void deserialize(const rimeaddr_t *sender, void *packet, size_t length)
{

	//Create and add the edge to the sender
	p_edge_t edge;
	edge.src = *sender;
	edge.dst = rimeaddr_node_addr;
	edge.ttl = 0x00; //TODO: Replace with correct ttl
	add_edge(edge);

	uint8_t i;
	uint8_t j;
	for (i = 0, j = 0; i < length; j++)
	{
		//Memory layout: p_node_t src, (p_edge_t src_drain, p_node_t drain)*
		if (j % 2 == 0)
		{
			//Add node or update
			add_node((p_node_t) * (packet + i);
			         i = i + sizeof(p_node_t);
		} else {
			//Add edge or update
			add_edge((p_edge_t) * (packet + i);
			         i = i + sizeof(p_edge_t);
		}
	}
}
