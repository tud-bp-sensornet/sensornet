/**
 * \file File for the graph operations containing all help functions working on the graph
 * \author tud-bp-sensornet
 */

#define __GRAPH_OPERATIONS_DEBUG__ 0

#include <stdlib.h>
#include <math.h>

#include "graph-operations.h"

#if __GRAPH_OPERATIONS_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

unsigned long last_time = 0;

/*---------------------------------------------------------------------------*/
p_hop_t *get_hop_counts(uint8_t *count)
{
	//Don't do anything if parameters are NULL
	if (count == NULL)
	{
		PRINTF("Debug: Count was a NULL pointer. Function will not proceed.");
		return NULL;
	}

	uint8_t node_count, edge_count;
	node_count = get_node_count();
	p_edge_t **all_edges = get_all_edges(&edge_count);

	//If there are no nodes or edges do nothing
	if (node_count == 0x00 || edge_count == 0x00)
	{
		PRINTF("Debug: Empty Graph. Function will not proceed.");
		*count = 0x00;
		return NULL;
	}

	p_hop_t *hop_dict = malloc((node_count - 0x01) * (uint8_t)sizeof(p_hop_t));

	//Test if memory could be allocated
	if (hop_dict == NULL)
	{
		PRINTF("Debug: Could not allocate memory. Function will not proceed.");
		return NULL;
	}

	//Initialize with zero values
	p_hop_t null_hop = {rimeaddr_null, 0x00};
	uint8_t i;
	for (i = 0; i < node_count - 1; i++)
	{
		memcpy(&(hop_dict[i]), &null_hop, sizeof(p_hop_t));
	}

	//First get all direct neighbours
	p_hop_t *tmp_hop_ptr = hop_dict;
	for (i = 0; i < edge_count; i++)
	{
		if (rimeaddr_cmp(&(all_edges[i]->src), &rimeaddr_node_addr))
		{
			tmp_hop_ptr->addr = all_edges[i]->dst;
			tmp_hop_ptr->hop_count = 0x01;
			tmp_hop_ptr++;
		}
	}

	//...Beginning with the nodes with the lowest hopcount...
	for (i = 0; i < node_count - 1; i++)
	{
		//...we get all outgoing edges from this node...
		uint8_t tmp_count;
		p_edge_t **tmp_outgoing = get_outgoing_edges(&(hop_dict[i].addr), &tmp_count);

		if (tmp_outgoing == NULL)
		{
			PRINTF("Debug: Node has no outgoing edges.");
			break;
		}

		uint8_t j;
		for (j = 0; j < tmp_count; j++)
		{
			//...and check if we reach a node we couldn't earlier...
			uint8_t k;
			uint8_t is_in = 0;
			for (k = 0; k < (node_count - 1); k++)
			{
				if (rimeaddr_cmp(&(tmp_outgoing[j]->dst), &(hop_dict[k].addr)))
				{
					//...if the node is not in our hop_dict we weren't able to reach it yet...
					is_in = 1;
					break;
				}
			}

			//...but now we are able to.
			if (is_in == 0)
			{
				tmp_hop_ptr->addr = tmp_outgoing[j]->dst;
				tmp_hop_ptr->hop_count = hop_dict[i].hop_count + 0x01;
				tmp_hop_ptr++;
			}
		}

		*count = (uint8_t)(tmp_hop_ptr - hop_dict);
	}

	return hop_dict;
}
/*---------------------------------------------------------------------------*/
void purge()
{
	uint16_t this_time = clock_seconds();
	int16_t diff = this_time - last_time;

	//Test for clock overflow
	if (diff < 0x0000)
	{
		//correct diff
		diff = (((uint16_t)pow(2, (sizeof(unsigned long) * 8) - 1)) + diff) / 0x003C;
	}
	else
	{
		//Seconds to Minutes
		diff = diff / 0x003C;
	}

	//Iterate over all edges and remove them when ttl <= 0
	uint8_t count;
	p_edge_t **all_edges = get_all_edges(&count);

	if (all_edges == NULL)
	{
		PRINTF("Debug: There are no edges in the graph. Function will not proceed.");
		return;
	}

	uint8_t i;
	for (i = 0; i < count; i++)
	{
		//If ttl is over
		if (all_edges[i]->ttl - diff <= 0x00)
		{
			//Remove this edge
			remove_edge(&(all_edges[i]->src), &(all_edges[i]->dst));
		}
		else
		{
			//Set new - decreased - ttl
			all_edges[i]->ttl = all_edges[i]->ttl - diff;
		}
	}

	last_time = clock_seconds();
}
/*---------------------------------------------------------------------------*/
