#include <string.h>
#include <stdio.h>

#include "contiki.h"
#include "unit-test.h"

#include "serialize.h"
#include "graph.h"

size_t tmp_length;
const void *tmp_packet_data;

void packet_complete_root(const void *packet_data, size_t length)
{
	tmp_length = length;
	tmp_packet_data = packet_data;
}

uint8_t called = 0x00;
void *memry;
size_t old_length;
uint8_t size = 0x00;

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
			node.u8[0] = (uint8_t)i;
			remove_node(&node);

			rimeaddr_t node2 = rimeaddr_null;
			node2.u8[0] = 0x01;
			remove_edge(&node2, &node);

		}

		//add new root
		p_node_t root = {rimeaddr_null};
		root.addr.u8[0] = 0x0F; //Node number 15
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

void *tmp_1_ptr;
size_t tmp_1_lngth;
void *tmp_2_ptr;
size_t tmp_2_lngth;
void *tmp_3_ptr;
size_t tmp_3_lngth;
void *tmp_4_ptr;
size_t tmp_4_lngth;
size_t cnt = 0;
void packet_complete_long(const void *packet_data, size_t length)
{

	//save data for later
	cnt++;
	if (cnt == 1)
	{
		tmp_1_ptr = malloc(length);
		memcpy(tmp_1_ptr, packet_data, length);
		tmp_1_lngth = length;
	}
	if (cnt == 2)
	{
		tmp_2_ptr = malloc(length);
		memcpy(tmp_2_ptr, packet_data, length);
		tmp_2_lngth = length;
	}
	if (cnt == 3)
	{
		tmp_3_ptr = malloc(length);
		memcpy(tmp_3_ptr, packet_data, length);
		tmp_3_lngth = length;
	}
	if (cnt == 4)
	{
		tmp_4_ptr = malloc(length);
		memcpy(tmp_4_ptr, packet_data, length);
		tmp_4_lngth = length;
	}
	if (cnt == 5)
	{
		//Delete all nodes and edges
		uint8_t node_count;
		p_node_t **all_nodes = get_all_nodes(&node_count);
		uint8_t edge_count;
		p_edge_t **all_edges = get_all_edges(&edge_count);

		uint8_t i;
		for (i = node_count; i > 0; i--)
		{
			remove_node(&(all_nodes[i - 1]->addr));
		}

		for (i = edge_count; i > 0; i--)
		{
			remove_edge(&(all_edges[i - 1]->src), &(all_edges[i - 1]->dst));
		}

		//Create new and old root
		p_node_t root;
		root.addr = rimeaddr_null;
		root.addr.u8[0] = 0x01;

		p_node_t new_root;
		new_root.addr = rimeaddr_null;
		new_root.addr.u8[0] = 0x0A;
		add_node(new_root);
		rimeaddr_set_node_addr(&(new_root.addr));

		//Deserialize everything
		deserialize(&(root.addr), tmp_1_ptr, tmp_1_lngth);
		deserialize(&(root.addr), tmp_2_ptr, tmp_2_lngth);
		deserialize(&(root.addr), tmp_3_ptr, tmp_3_lngth);
		deserialize(&(root.addr), tmp_4_ptr, tmp_4_lngth);
		deserialize(&(root.addr), packet_data, length);

		//Cleanup
		free(tmp_1_ptr);
		free(tmp_2_ptr);
		free(tmp_3_ptr);
		free(tmp_4_ptr);

	}
}

/**
 * Register unit tests that will be executed by using
 * the UNIT_TEST_RUN macro.
 */
UNIT_TEST_REGISTER(root_test, "Root test");
UNIT_TEST_REGISTER(null_test, "Empty graph test");
UNIT_TEST_REGISTER(multi_sub_graph_test, "Multi sub graph test");
UNIT_TEST_REGISTER(long_graph_test, "Long graph test");

/**
 * Test serialize with only root in the graph
 * Tests the functions:
 * void serialize(void (*packet_complete)(const void *packet_data, size_t length));
 */
UNIT_TEST(root_test)
{

	p_node_t r;
	r.addr = rimeaddr_null;
	r.addr.u8[0] = 0x01;
	rimeaddr_set_node_addr(&(r.addr));
	add_node(r);

	UNIT_TEST_BEGIN();

	serialize(packet_complete_root);

	UNIT_TEST_ASSERT(tmp_length == sizeof(p_node_t));
	UNIT_TEST_ASSERT(((p_node_t *)tmp_packet_data)->addr.u8[0] == 0x01);

	UNIT_TEST_END();

	remove_node(&(r.addr));

}

/**
 * Test serialize with a empty graph
 * Tests the functions:
 * void serialize(void (*packet_complete)(const void *packet_data, size_t length));
 */
UNIT_TEST(null_test)
{

	p_node_t r;
	r.addr = rimeaddr_null;
	r.addr.u8[0] = 0x01;
	rimeaddr_set_node_addr(&(r.addr));

	UNIT_TEST_BEGIN();

	serialize(packet_complete_root);

	//Just do not break

	UNIT_TEST_END();

}

/**
 * Test serialize and deserialize with a graph creating subgraphs not fitting into one package
 * Tests the functions:
 * void serialize(void (*packet_complete)(const void *packet_data, size_t length));
 * void deserialize(const rimeaddr_t *sender, const void *packet, size_t length);
 */
UNIT_TEST(multi_sub_graph_test)
{
	p_node_t root;
	root.addr = rimeaddr_null;
	root.addr.u8[0] = 0x01;
	rimeaddr_set_node_addr(&(root.addr));
	add_node(root);

	//Create a graph which serialization is bigger than the PACKETBUF_SIZE (128)
	//Smallest functional node size is 2 Byte and edge is 5 Byte
	for (size = 2; size < 21; size++) //19 edges and nodes
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

	//This test works only with the smallest possible node/edge size
	UNIT_TEST_ASSERT(sizeof(p_node_t) == 2);
	UNIT_TEST_ASSERT(sizeof(p_edge_t) == 5);

	//This test works only with this configuration
	UNIT_TEST_ASSERT(K >= 1);
	UNIT_TEST_ASSERT(MAX_NODES >= 20);
	UNIT_TEST_ASSERT(MAX_EDGES >= 19);

	serialize(packet_complete_multi);

	UNIT_TEST_ASSERT(called == 0x02); //Function should be called 2 times

	//Check if we got everything
	uint8_t node_count = get_node_count();
	uint8_t edge_count = get_edge_count();

	UNIT_TEST_ASSERT(node_count == 20); //19 drains + root + new root, but new root is a drain
	UNIT_TEST_ASSERT(edge_count == 19); //19 edges from root + one extra edge to new root, but new root is a drain

	//cleanup and test if every edge/node was correctly de/serialized
	uint8_t i;
	for (i = 1; i < size; i++)
	{
		rimeaddr_t node = rimeaddr_null;
		node.u8[0] = i;

		UNIT_TEST_ASSERT(find_node(&node) != NULL);

		if (i != 0x01)
		{
			UNIT_TEST_ASSERT(find_edge(&root.addr, &node) != NULL);
		}

		remove_node(&node);
		remove_edge(&root.addr, &node);
	}

	UNIT_TEST_END();
}

/**
 * Graph:
 *                 r
 *               /  \
 *             n11->n12
 *             /      \
 *           n21----->n22
 *           /          \
 *         n31--------->n32
 *         /              \
 *       n41------------->n42
 *
 * Test serialize and deserialize with a graph with more than K = 3 levels
 * Tests the functions:
 * void serialize(void (*packet_complete)(const void *packet_data, size_t length));
 */
UNIT_TEST(long_graph_test)
{
	p_node_t root, n11, n12, n21, n22, n31, n32, n41, n42;
	root.addr = rimeaddr_null;
	n11.addr = rimeaddr_null;
	n12.addr = rimeaddr_null;
	n21.addr = rimeaddr_null;
	n22.addr = rimeaddr_null;
	n31.addr = rimeaddr_null;
	n32.addr = rimeaddr_null;
	n41.addr = rimeaddr_null;
	n42.addr = rimeaddr_null;
	root.addr.u8[0] = 0x01;
	n11.addr.u8[0] = 0x02;
	n12.addr.u8[0] = 0x03;
	n21.addr.u8[0] = 0x04;
	n22.addr.u8[0] = 0x05;
	n31.addr.u8[0] = 0x06;
	n32.addr.u8[0] = 0x07;
	n41.addr.u8[0] = 0x08;
	n42.addr.u8[0] = 0x09;
	rimeaddr_set_node_addr(&(root.addr));
	add_node(root);
	add_node(n11);
	add_node(n12);
	add_node(n21);
	add_node(n22);
	add_node(n31);
	add_node(n32);
	add_node(n41);
	add_node(n42);

	p_edge_t er11, er12, e1112, e1121, e1222, e2122, e2131, e2232, e3132, e3141, e3242, e4142;
	er11.src = root.addr;
	er11.dst = n11.addr;
	er12.src = root.addr;
	er12.dst = n12.addr;
	e1112.src = n11.addr;
	e1112.dst = n12.addr;
	e1121.src = n11.addr;
	e1121.dst = n21.addr;
	e1222.src = n12.addr;
	e1222.dst = n22.addr;
	e2122.src = n21.addr;
	e2122.dst = n22.addr;
	e2131.src = n21.addr;
	e2131.dst = n31.addr;
	e2232.src = n22.addr;
	e2232.dst = n32.addr;
	e3132.src = n31.addr;
	e3132.dst = n32.addr;
	e3141.src = n31.addr;
	e3141.dst = n41.addr;
	e3242.src = n32.addr;
	e3242.dst = n42.addr;
	e4142.src = n41.addr;
	e4142.dst = n42.addr;
	add_edge(er11);
	add_edge(er12);
	add_edge(e1112);
	add_edge(e1121);
	add_edge(e1222);
	add_edge(e2122);
	add_edge(e2131);
	add_edge(e2232);
	add_edge(e3132);
	add_edge(e3141);
	add_edge(e3242);
	add_edge(e4142);

	UNIT_TEST_BEGIN();

	//This test works only with the smallest possible node/edge size (or a bit more)
	UNIT_TEST_ASSERT(sizeof(p_node_t) == 2);
	UNIT_TEST_ASSERT(sizeof(p_edge_t) == 5);

	//This test works only with this configuration
	UNIT_TEST_ASSERT(K == 3);
	UNIT_TEST_ASSERT(MAX_NODES >= 9);
	UNIT_TEST_ASSERT(MAX_EDGES >= 12);

	serialize(packet_complete_long);

	UNIT_TEST_ASSERT(get_node_count() == 0x08); //All nodes to lvl 3 + new root
	UNIT_TEST_ASSERT(get_edge_count() == 0x09); //All edges to lvl 3 except between lvl 3 + new root edge

	//cleanup and test if every edge/node was correctly de/serialized
	UNIT_TEST_ASSERT(find_node(&root.addr) != NULL);
	remove_node(&root.addr);
	UNIT_TEST_ASSERT(find_edge(&root.addr, &n11.addr) != NULL);
	remove_edge(&root.addr, &n11.addr);
	UNIT_TEST_ASSERT(find_edge(&root.addr, &n12.addr) != NULL);
	remove_edge(&root.addr, &n12.addr);

	UNIT_TEST_ASSERT(find_node(&n11.addr) != NULL);
	remove_node(&n11.addr);
	UNIT_TEST_ASSERT(find_edge(&n11.addr, &n12.addr) != NULL);
	remove_edge(&n11.addr, &n12.addr);
	UNIT_TEST_ASSERT(find_edge(&n11.addr, &n21.addr) != NULL);
	remove_edge(&n11.addr, &n21.addr);

	UNIT_TEST_ASSERT(find_node(&n12.addr) != NULL);
	remove_node(&n12.addr);
	UNIT_TEST_ASSERT(find_edge(&n12.addr, &n22.addr) != NULL);
	remove_edge(&n12.addr, &n22.addr);

	UNIT_TEST_ASSERT(find_node(&n21.addr) != NULL);
	remove_node(&n21.addr);
	UNIT_TEST_ASSERT(find_edge(&n21.addr, &n22.addr) != NULL);
	remove_edge(&n21.addr, &n22.addr);
	UNIT_TEST_ASSERT(find_edge(&n21.addr, &n31.addr) != NULL);
	remove_edge(&n21.addr, &n31.addr);

	UNIT_TEST_ASSERT(find_node(&n22.addr) != NULL);
	remove_node(&n22.addr);
	UNIT_TEST_ASSERT(find_edge(&n22.addr, &n32.addr) != NULL);
	remove_edge(&n22.addr, &n32.addr);

	UNIT_TEST_ASSERT(find_node(&n31.addr) != NULL);
	remove_node(&n31.addr);

	UNIT_TEST_ASSERT(find_node(&n32.addr) != NULL);
	remove_node(&n32.addr);

	rimeaddr_t node = rimeaddr_null;
	node.u8[0] = 0x0A;
	UNIT_TEST_ASSERT(find_node(&node) != NULL);
	remove_node(&node);
	UNIT_TEST_ASSERT(find_edge(&root.addr, &node) != NULL);
	remove_edge(&root.addr, &node);

	UNIT_TEST_END();
}

int main()
{
	init_graph();

	UNIT_TEST_RUN(root_test);
	UNIT_TEST_RUN(null_test);
	UNIT_TEST_RUN(multi_sub_graph_test);
	UNIT_TEST_RUN(long_graph_test);

	return EXIT_SUCCESS;
}
