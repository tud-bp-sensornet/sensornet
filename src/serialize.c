#include "serialize.h"
#include "graph.h"

void serialize(void (*packet_complete)(const void *packet_data, size_t length))
{
	//Single memory to avoid fragementation
	//Memory layout: p_node_t src, (p_edge_t src_drain, p_node_t drain)*
	void* memory = malloc(PACKETBUF_SIZE);
	void* memory_ptr = memory;

	uint8_t* node_count;
	p_node_t** array_ptr_ptr = get_all_nodes(node_count);

	int i;
	for (i = 0; i < (int)*node_count; i++)
	{
		//Package source node
		memcpy(memory_ptr, array_ptr_ptr[i], sizeof(p_node_t));
		memory_ptr = memory_ptr + sizeof(p_node_t);

		uint8_t* edge_count;
		p_edge_t** edge_ptr_ptr = get_outgoing_edges(edge_count);

		//Package every edge and drain
		int j = sizeof(p_node_t);
		int k;
		for (k = 0; k < (int)*edge_count; k++)
		{
			//Pack outgoing edge
			memcpy(memory_ptr, edge_ptr_ptr[k], sizeof(p_edge_t));
			memory_ptr = memory_ptr + sizeof(p_edge_t);

			//Pack drain of edge
			p_node_t* drain = find_node(&(edge_ptr_ptr[i]->dst));
			memcpy(memory_ptr, drain, sizeof(p_node_t));
			memory_ptr = memory_ptr + sizeof(p_node_t);

			//Package will get too big in next iteration
			if (j + (sizeof(p_node_t) + sizeof(p_edge_t)) < PACKETBUF_SIZE && k + 1 < (int)*edge_count)
			{
				//Send subgraph
				packet_complete(memory, (size_t)j);

				//Reset size and packet
				j = sizeof(p_node_t);
				memory_ptr = memory_ptr + sizeof(p_node_t);
			}

			j = j + (sizeof(p_node_t) + sizeof(p_edge_t));
		}

		//Send subgraph
		packet_complete(memory, full_size);

		free(edge_ptr_ptr);
		free(memory);
	}

}

void deserialize(const rimeaddr_t *sender, void *packet, size_t length)
{
	int i;
	int j;
	for (i = 0, j = 0; i < (int)length; j++)
	{
		//Memory layout: p_node_t src, (p_edge_t src_drain, p_node_t drain)*
		if(j % 2 == 0)
		{
			//Add node or update
			add_node((p_node_t)*(packet + i);
			i = i + sizeof(p_node_t);
		}else{
			//Add edge or update
			add_edge((p_edge_t)*(packet + i);
			i = i + sizeof(p_edge_t);
		}
	}
}
