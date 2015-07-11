/**
 * \file                       A serializer and deserializer
 * \author                     tud-bp-sensornet
 */

#define __SERIALIZE_DEBUG__ 0

#include <string.h>

#include "serialize.h"
#include "graph.h"
#include "graph-operations.h"

#if __SERIALIZE_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void package_and_send_edges(void *memory, void *memory_ptr, rimeaddr_t *src, void (*packet_complete)(const void *packet_data, size_t length));

/*---------------------------------------------------------------------------*/
void serialize(void (*packet_complete)(const void *packet_data, size_t length))
{

	PRINTF("Node size: %d Edge size: %d\n", sizeof(p_node_t), sizeof(p_edge_t));
	PRINTF("Debug: PACKETBUF SIZE: %d\n", PACKETBUF_SIZE);

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

	if (memory == NULL)
	{
		PRINTF("Debug: Could not allocate memory. Function will not proceed.\n");
		return;
	}

	//Package root (on K >= 1) (reachable in 0 hops)
	p_node_t *root = find_node(&rimeaddr_node_addr);

	if (root == NULL)
	{
		PRINTF("Debug: Could not find root in graph. Function will not proceed.\n");
		free(memory);
		return;
	}

	PRINTF("Debug: Pack root %d\n", root->addr.u8[0]);
	memcpy(memory_ptr, root, sizeof(p_node_t));
	memory_ptr = memory_ptr + sizeof(p_node_t);

	//Package all outgoing edges and edge destinations from root (on K >= 2) (reachable in 1 hops)
	if (K >= 2)
	{
		package_and_send_edges(memory, memory_ptr, &rimeaddr_node_addr, packet_complete);
	}
	else
	{
		return;
	}

	//Get all reachable nodes and their hopcount
	uint8_t reachable_count;
	p_hop_t *hops = get_hop_counts(&reachable_count);

	//Package nodes and their edges reachable in up to K-1 hops
	uint8_t l;
	for (l = 0; l < reachable_count; l++)
	{
		if (hops[l].hop_count <= K - 1)
		{
			//Package this node
			PRINTF("Debug: Pack node %d\n", hops[l].addr.u8[0]);

			p_node_t *nd = find_node(&(hops[l].addr));

			if (nd == NULL)
			{
				PRINTF("Debug: Could not find node in graph. Function will not proceed.\n");
				free(hops);
				return;
			}

			memcpy(memory, nd, sizeof(p_node_t));
			memory_ptr = memory + sizeof(p_node_t);
			package_and_send_edges(memory, memory_ptr, &(hops[l].addr), packet_complete);
		}
	}

	if (hops != NULL)
	{
		free(hops);
	}
}
/*---------------------------------------------------------------------------*/
void package_and_send_edges(void *memory, void *memory_ptr, rimeaddr_t *src, void (*packet_complete)(const void *packet_data, size_t length))
{
	//Get all outgoing edges from that node
	uint8_t edge_count;
	p_edge_t **edge_ptr_ptr = get_outgoing_edges(src, &edge_count);

	PRINTF("Debug: Node has %d edges.\n", edge_count);

	//Package every edge and drain
	uint8_t j = (uint8_t)sizeof(p_node_t);
	uint8_t k;
	for (k = 0; k < edge_count; k++)
	{
		PRINTF("Debug: Pack edge to %d\n", edge_ptr_ptr[k]->dst.u8[0]);

		//Increment package size counter
		j = j + (sizeof(p_node_t) + sizeof(p_edge_t));

		//Pack outgoing edge
		memcpy(memory_ptr, edge_ptr_ptr[k], sizeof(p_edge_t));
		memory_ptr = memory_ptr + sizeof(p_edge_t);

		//Pack drain of edge
		p_node_t *drain = find_node(&(edge_ptr_ptr[k]->dst));

		if (drain == NULL)
		{
			PRINTF("Debug: Cannot find drain. Function will not proceed.\n");
			return;
		}

		PRINTF("Debug: Pack node %d\n", drain->addr.u8[0]);

		memcpy(memory_ptr, drain, sizeof(p_node_t));
		memory_ptr = memory_ptr + sizeof(p_node_t);

		//Package will get too big in next iteration
		PRINTF("Debug: Size is now: %d\n", j);
		if (j + (sizeof(p_node_t) + sizeof(p_edge_t)) > PACKETBUF_SIZE && k + 1 < edge_count)
		{
			PRINTF("Debug: Send package (too big). Size: %d\n", j);
			//Send subgraph
			packet_complete(memory, (size_t)j);

			//Reset size and packet
			j = sizeof(p_node_t);
			memory_ptr = memory + sizeof(p_node_t);
		}
	}

	if (edge_ptr_ptr != NULL)
	{
		free(edge_ptr_ptr);
	}

	//Send subgraph
	PRINTF("Debug: Send package.\n");
	packet_complete(memory, (size_t)j);
}
/*---------------------------------------------------------------------------*/
void deserialize(const rimeaddr_t *sender, const void *packet, size_t length)
{

	//Create and add the edge to the sender
	p_edge_t edge;
	edge.src = *sender;
	edge.dst = rimeaddr_node_addr;
	edge.ttl = 0x1E; //TODO: Replace with correct ttl
	add_edge(edge);

	uint8_t i;
	uint8_t j;
	for (i = 0, j = 0; i < length; j++)
	{
		//Memory layout: p_node_t src, (p_edge_t src_drain, p_node_t drain)*
		if (j % 2 == 0)
		{
			//Add node or update
			add_node(*((p_node_t *)(packet + i)));
			PRINTF("Debug: Added Node: %d now %d\n", ((p_node_t *)(packet + i))->addr.u8[0], get_node_count());
			i = i + sizeof(p_node_t);
		}
		else
		{
			//Add edge or update
			add_edge(*((p_edge_t *)(packet + i)));
			PRINTF("Debug: Added Edge to: %d\n", ((p_edge_t *)(packet + i))->dst.u8[0]);
			i = i + sizeof(p_edge_t);
		}
	}
}
/*---------------------------------------------------------------------------*/
