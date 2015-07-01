/**
 * \file File for the graph operations containing all help functions working on the graph
 * \author tud-bp-sensornet
 */

#include <math.h>

CCIF unsigned long last_time = 0;

p_hop_t *get_hop_counts(uint8_t *count)
{
	//Don't do anything if parameters are NULL
	if (count == NULL)
	{
		return NULL;
	}

	uint8_t *node_count, *edge_count;
	p_node_t **all_nodes = get_all_nodes(node_count);
	p_edge_t **all_edges = get_all_edges(edge_count);

	p_hop_t *hop_dict = malloc((node_count - 0x01) * sizeof(p_hop_t));

	//Initialize with zero values
	p_hop_t null_hop = {rimeaddr_null, 0x00};
	uint8_t i;
	for (i = 0; i < node_count - 1; i++)
	{
		hop_dict[i] = null_hop;
	}

	//First get all direct neighbours
	p_hop_t *tmp_hop_ptr = hop_dict;
	for (i = 0; i < (int)edge_count; i++)
	{
		if (rimeaddr_cmp(&(all_edges[i]->src), &rimeaddr_node_addr))
		{
			tmp_hop_ptr->addr = all_edges[i]->dst;
			tmp_hop_ptr->hop_count = 0x01;
			tmp_hop_ptr++;
		}
	}

	//...Beginning with the nodes with the lowest hopcount...
	for (i = 0; i < (int)(node_count - 1); i++)
	{
		//...we get all outgoing edges from this node...
		uint8_t *tmp_count;
		p_edge_t **tmp_outgoing = get_outgoing_edges(&(hop_dict[i].addr), tmp_count);
		int j;
		for (j = 0; j < (int)*tmp_count; j++)
		{
			//...and check if we reach a node we couldn't earlier...
			int k;
			int is_in = 0;
			for (k = 0; k < (int)(node_count - 1); k++)
			{
				if (rimeaddr_cmp(&(tmp_outgoing[j]->dst, &(hop_dict[k].addr)))
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
}

void purge()
{
	CCIF unsigned long this_time = clock_seconds();
	signed long diff = this_time - last_time;

	//Test for clock overflow
	if (diff < (signed long)0)
	{
		//correct diff
		diff = diff % pow(2, sizeof(CCIF unsigned long) * 8);
	}

	//Seconds to Minutes
	diff = diff / 60;

	//Iterate over all edges and remove them when ttl <= 0
	uint8_t *count;
	p_edge_t **all_edges = get_all_edges(count);

	uint8_t i;
	for (i = 0; i < count; i++)
	{
		//If ttl is over
		if (all_edges[i]->ttl - diff <= 0x00)
		{
			uint8_t tmp_cnt;
			p_edge_t **ingiong = get_ingoing_edges(&(all_edges[i]->dst), &tmp_cnt);
			if (ingiong != NULL)
			{
				free(ingiong);
			} else {
				//TODO: TBD: when to delete notes
				//No incoming edges to this node anymore, it can't be reached
				//remove_node(&(all_edges[i]->dst);
			}

			//Remove this edge
			remove_edge(all_edges[i].src, all_edges[i].dst);
		} else {
			//Set new - decreased - ttl
			all_edges[i]->ttl = all_edges[i]->ttl - diff;
		}
	}

	last_time = clock_seconds();
}