#include "contiki.h"
#include "unit-test.h"

#include "serialize.h"
#include "graph.h"

uint8_t called = 0;
uint8_t size;
void *memry;
size_t old_length;

void packet_complete_root(const void *packet_data, size_t length)
{
	UNIT_TEST_ASSERT(length == sizeof(p_node_t));
	UNIT_TEST_ASSERT(((p_node_t*)packet_data)->addr.u8[0] == 0x01);
}

void packet_complete_multi(const void *packet_data, size_t length)
{
	called++;

	//Save first serialized data for later use
	if (called == 0x01)
	{
		memry = malloc(length);
		memcpy(memry, packet_data, length);
		old_length = length;
	}

	//On second call...
	if (called == 0x02)
	{
		//...delete all nodes and edges
		uint8_t i;
		for (i = 1; i < size; i++)
		{
			rimeaddr_t node = rimeaddr_null;
			node.u8[0] = (unit8_t)i;
			remove_node(&node);

			rimeaddr_t node2 = rimeaddr_null;
			node2.u8[0] = 0x01;
			remove_edge(&node2, &node);

		}

		//add new root
		p_node_t root = {rimeaddr_null};
		root.addr.u8[0] = 0x13; //Node number 19
		rimeaddr_set_node_addr(&(root.addr));
		add_node(root);

		//old root (sender)
		p_node_t sender;
		sender.addr = rimeaddr_null;
		sender.addr.u8[0] = 0x01;

		deserialize(&(sender.addr), memry, old_length);
		deserialize(&(sender.addr), packet_data, length);

		free(memry);
	}
}

/**
 * Register unit tests that will be executed by using
 * the UNIT_TEST_RUN macro.
 */
UNIT_TEST_REGISTER(root_test, "Root test");
UNIT_TEST_REGISTER(multi_sub_graph_test, "Multi sub graph test");

UNIT_TEST(root_test)
{
	p_node_t root;
	root.addr = rimeaddr_null;
	root.addr.u8[0] = 0x01;
	rimeaddr_set_node_addr(&root);
	add_node(root);

	UNIT_TEST_BEGIN();

	serialize(packet_complete_root);

	UNIT_TEST_END();
}

//This test works only with the smallest possible node/edge size
UNIT_TEST(multi_sub_graph_test)
{
	p_node_t root;
	root.addr = rimeaddr_null;
	root.addr.u8[0] = 0x01;
	rimeaddr_set_node_addr(&(root.addr));
	add_node(root);

	//Create a graph which serialization is bigger than the PACKETBUF_SIZE (128)
	//Smallest functional node size is 2 Byte and edge is 6 Byte
	for (size = 2; size < 18; size++)
	{
		p_node_t node;
		node.addr = rimeaddr_null;
		node.addr.u8[0] = size;
		add_node(node);

		p_edge_t edge = {root.addr, rimeaddr_null, 0x00};
		edge.dst.u8[0] = size;
		add_edge(edge);
	}

	UNIT_TEST_BEGIN();

	serialize(packet_complete_multi);

	UNIT_TEST_ASSERT(called == 2);

	//Check if we got everything

	uint8_t node_count;
	uint8_t edge_count;

	p_node_t** all_nodes == get_all_nodes(&node_count);
	p_edge_t** all_edges == get_all_edges(&edge_count);

	UNIT_TEST_ASSERT(node_count == size); //is 1 more than sent amount because of our new root
	UNIT_TEST_ASSERT(edge_count == size); //same here

	//Add the rime addr value to check if every addr was correctly de/serialized
	uint8_t i;
	uint8_t sum = 0;
	for (i = 0; i < node_count; i++)
	{
		sum = sum + all_nodes[i]->addr.u8[0];
	}

	UNIT_TEST_ASSERT(sum == 0.5 * (size - 1)*size);

	//TODO: Test edges

	UNIT_TEST_END();
}

void main() {
	UNIT_TEST_RUN(root_test);
	UNIT_TEST_RUN(multi_sub_graph_test);
}
