/**
 * \file graph-operations.c
 * File for the graph operations containing all help functions working on the graph.
 * \author tud-bp-sensornet
 */

/**
 * \def __GRAPH_OPERATIONS_DEBUG__
 * \brief Set to 1 to activate debug output.
*/
#ifndef __GRAPH_OPERATIONS_DEBUG__
#define __GRAPH_OPERATIONS_DEBUG__ 0
#endif

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "graph-operations.h"

//\cond
#if __GRAPH_OPERATIONS_DEBUG__
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif
//\endcond

//The last time purge was called
static unsigned long last_time = 0;

//Seconds passed but ttl was not yet decremented
static uint8_t seconds = 0;

/*---------------------------------------------------------------------------*/
p_hop_t *get_hop_counts(uint8_t *count)
{
	//Don't do anything if parameters are NULL
	if (count == NULL)
	{
		PRINTF("[graph-operations.c/get_hop_counts] Count was a NULL pointer. Function will not proceed.\n");
		return NULL;
	}

	uint8_t node_count, edge_count;
	node_count = get_node_count();
	p_edge_t **all_edges = get_all_edges(&edge_count);

	//If there are 0-1 nodes or edges do nothing
	if (node_count <= 0x01 || edge_count == 0x00)
	{
		PRINTF("[graph-operations.c/get_hop_counts] Empty Graph. Function will not proceed.\n");
		*count = 0x00;
		return NULL;
	}

	p_hop_t *hop_dict = malloc((node_count - 0x01) * (uint8_t)sizeof(p_hop_t));

	//Test if memory could be allocated
	if (hop_dict == NULL)
	{
		PRINTF("[graph-operations.c/get_hop_counts] Could not allocate memory. Function will not proceed.\n");
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
		//Edge from root to neighbour
		if (rimeaddr_cmp(&(all_edges[i]->src), &rimeaddr_node_addr))
		{
			//Check if node is already in our hopcounts
			uint8_t k;
			uint8_t is_in = 0;
			for (k = 0; k < (tmp_hop_ptr - hop_dict); k++)
			{
				if (rimeaddr_cmp(&(all_edges[i]->dst), &(hop_dict[k].addr)))
				{
					PRINTF("[graph-operations.c/get_hop_counts] %d already in hop_count (outgoing direct)\n", all_edges[i]->dst.u8[0]);
					//...if the node is not in our hop_dict have not seen it yet...
					is_in = 1;
					break;
				}
			}

			//...but now we have.
			if (is_in == 0)
			{
				PRINTF("[graph-operations.c/get_hop_counts] Add node (outgoing direct): %d\n", all_edges[i]->dst.u8[0]);
				tmp_hop_ptr->addr = all_edges[i]->dst;
				tmp_hop_ptr->hop_count = 0x01;
				tmp_hop_ptr++;
			}
		}
		else
			//Edge from neighbour to root
			if (rimeaddr_cmp(&(all_edges[i]->dst), &rimeaddr_node_addr))
			{

				//Check if node is already in our hopcounts
				uint8_t k;
				uint8_t is_in = 0;
				for (k = 0; k < (tmp_hop_ptr - hop_dict); k++)
				{
					if (rimeaddr_cmp(&(all_edges[i]->src), &(hop_dict[k].addr)))
					{
						//...if the node is not in our hop_dict have not seen it yet...
						PRINTF("[graph-operations.c/get_hop_counts] %d already in hop_count (ingoing direct)\n", all_edges[i]->src.u8[0]);
						is_in = 1;
						break;
					}
				}

				//...but now we have.
				if (is_in == 0)
				{
					PRINTF("[graph-operations.c/get_hop_counts] Add node (ingoing direct): %d\n", all_edges[i]->src.u8[0]);
					tmp_hop_ptr->addr = all_edges[i]->src;
					tmp_hop_ptr->hop_count = 0x01;
					tmp_hop_ptr++;
				}
			}
	}

	//Proceed only iff there are any out/ingoing edges from root
	if ((uint8_t)(tmp_hop_ptr - hop_dict) != 0x00)
	{
		//Beginning with the nodes with the lowest hopcount...
		for (i = 0; i < node_count - 1; i++)
		{
			//...we get all out/ingoing edges from this node...
			uint8_t tmp_count;
			p_edge_t **tmp_outgoing = get_outgoing_edges(&(hop_dict[i].addr), &tmp_count);

			uint8_t tmp_count_in;
			p_edge_t **tmp_ingoing = get_ingoing_edges(&(hop_dict[i].addr), &tmp_count_in);

			if (tmp_outgoing == NULL && tmp_ingoing == NULL)
			{
				PRINTF("[graph-operations.c/get_hop_counts] Node has no ingoing and outgoing edges.\n");
				break;
			}

			uint8_t j;
			for (j = 0; j < tmp_count; j++)
			{
				//...and check if we reach a node we couldn't earlier...
				uint8_t k;
				uint8_t is_in = 0;
				for (k = 0; k < (tmp_hop_ptr - hop_dict); k++)
				{
					//...except root...
					if (rimeaddr_cmp(&(tmp_outgoing[j]->dst), &(hop_dict[k].addr)) || rimeaddr_cmp(&(tmp_outgoing[j]->dst), &rimeaddr_node_addr))
					{
						//...if the node is not in our hop_dict we weren't able to reach it yet...
						PRINTF("[graph-operations.c/get_hop_counts] %d already in hop_count (outgoing)\n", tmp_outgoing[j]->dst.u8[0]);
						is_in = 1;
						break;
					}
				}

				//...but now we are able to.
				if (is_in == 0)
				{
					PRINTF("[graph-operations.c/get_hop_counts] Add node (outgoing): %d\n", tmp_outgoing[j]->dst.u8[0]);
					tmp_hop_ptr->addr = tmp_outgoing[j]->dst;
					tmp_hop_ptr->hop_count = hop_dict[i].hop_count + 0x01;
					tmp_hop_ptr++;
				}
			}

			//Same for ingoing edges.
			for (j = 0; j < tmp_count_in; j++)
			{
				uint8_t k;
				uint8_t is_in = 0;
				for (k = 0; k < (tmp_hop_ptr - hop_dict); k++)
				{
					if (rimeaddr_cmp(&(tmp_ingoing[j]->src), &(hop_dict[k].addr)) || rimeaddr_cmp(&(tmp_ingoing[j]->src), &rimeaddr_node_addr))
					{
						PRINTF("[graph-operations.c/get_hop_counts] %d already in hop_count (ingoing)\n", tmp_ingoing[j]->src.u8[0]);
						is_in = 1;
						break;
					}
				}

				if (is_in == 0)
				{
					PRINTF("[graph-operations.c/get_hop_counts] Add node (ingoing): %d\n", tmp_ingoing[j]->src.u8[0]);
					tmp_hop_ptr->addr = tmp_ingoing[j]->src;
					tmp_hop_ptr->hop_count = hop_dict[i].hop_count + 0x01;
					tmp_hop_ptr++;
				}
			}

			free(tmp_outgoing);
			free(tmp_ingoing);
		}
	}

	*count = (uint8_t)(tmp_hop_ptr - hop_dict);
	return hop_dict;
}
/*---------------------------------------------------------------------------*/
void purge()
{
	uint16_t this_time = clock_seconds();
	uint16_t diff = this_time - last_time;
	uint8_t decrement;

	if (seconds + diff >= 60)
	{
		//Value to decrement in minutes
		decrement = (seconds +  diff) / 60;
		//Rest amount of decrement time
		seconds = (seconds + diff) % 60;

		//Iterate over all edges and remove them when ttl <= 0
		uint8_t count;
		p_edge_t **all_edges = get_all_edges(&count);

		if (all_edges == NULL)
		{
			PRINTF("[graph-operations.c/purge] There are no edges in the graph. Function will not proceed.\n");
			return;
		}

		uint8_t i;
		uint8_t deleted = 0;
		for (i = 0; i < count - deleted; i++)
		{
			//If ttl is over
			if (all_edges[i]->ttl - decrement <= 0)
			{
				//Save rimeaddr
				rimeaddr_t src = all_edges[i]->src;
				rimeaddr_t dst = all_edges[i]->dst;

				//Remove this edge
				remove_edge(&(all_edges[i]->src), &(all_edges[i]->dst));

				//Do not delete root
				if (!rimeaddr_cmp(&src, &rimeaddr_node_addr))
				{
					//Check for orphan nodes
					uint8_t src_out_count;
					p_edge_t **src_out = get_outgoing_edges(&src, &src_out_count);

					uint8_t src_in_count;
					p_edge_t **src_in = get_ingoing_edges(&src, &src_in_count);

					//Delete orphan nodes
					if (src_out_count == 0 && src_in_count == 0)
					{
						remove_node(&src);
					}
					else
					{
						if (src_out != NULL)
						{
							free(src_out);
						}

						if (src_in != NULL)
						{
							free(src_in);
						}
					}
				}

				if (!rimeaddr_cmp(&dst, &rimeaddr_node_addr))
				{
					uint8_t dst_out_count;
					p_edge_t **dst_out = get_outgoing_edges(&dst, &dst_out_count);

					uint8_t dst_in_count;
					p_edge_t **dst_in = get_ingoing_edges(&dst, &dst_in_count);

					if (dst_out_count == 0 && dst_in_count == 0)
					{
						remove_node(&dst);
					}
					else
					{
						if (dst_out != NULL)
						{
							free(dst_out);
						}

						if (dst_in != NULL)
						{
							free(dst_in);
						}
					}

				}

				//Array will be rearranged, adapt to new array
				i--;
				deleted++;
			}
			else
			{
				//Set new - decreased - ttl
				all_edges[i]->ttl = all_edges[i]->ttl - decrement;
			}
		}

	}
	else
	{
		seconds += diff;
	}

	last_time = clock_seconds();
}
/*---------------------------------------------------------------------------*/
