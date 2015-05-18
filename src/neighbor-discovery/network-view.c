#include "contiki.h"
#include "net/rime.h"
#include <stdio.h>

#include "network-view.h"

MEMB(node_memory, node_t, MAX_NODES);
LIST(known_nodes);

// Inits network view and adds current sensor to known nodes list.
void init_network_view()
{
	memb_init(&node_memory);
	list_init(known_nodes);

	node_t* this_node = (node_t*)memb_alloc(&node_memory);
	this_node->node_addr = rimeaddr_node_addr;
	this_node->hop_count = 0;
	this_node->last_update = -1;

	list_add(known_nodes, this_node);

	debug_print_nodes();
}

list_t get_known_nodes()
{
	return known_nodes;
}

void debug_print_nodes()
{
	printf("NODES (len %d):", list_length(known_nodes));

	node_t *node;

	for (node = list_head(known_nodes); node != NULL; node = list_item_next(node))
	{
		printf(" (%d.%d,%d,%d)",
			node->node_addr.u8[0], node->node_addr.u8[1], node->hop_count, node->last_update);
	}

	printf("\n");
}
