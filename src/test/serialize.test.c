#include <string.h>
#include <stdio.h>

#include "contiki.h"
#include "unit-test.h"

#include "serialize.h"
#include "graph.h"

size_t tmp_length;
void *tmp_packet_data;

void packet_complete_root(const void *packet_data, size_t length)
{
	tmp_length = length;
	tmp_packet_data = malloc(length);
	memcpy(tmp_packet_data, packet_data, length);

	//Do not disturb serialize; Do not modify the graph here

}

uint8_t called = 0x00;
void *memry_1 = NULL;
size_t old_length_1 = 0;
void *memry_2 = NULL;
size_t old_length_2 = 0;
uint8_t size = 0x00;

void packet_complete_multi(const void *packet_data, size_t length)
{
	called++;

	//Save first serialized data for later use
	if (called == 0x01)
	{
		memry_1 = malloc(length);
		if (memry_1 != NULL)
		{
			memcpy(memry_1, packet_data, length);
		}
		old_length_1 = length;
	}

	//Save second serialized data
	if (called == 0x02)
	{
		memry_2 = malloc(length);
		if (memry_2 != NULL)
		{
			memcpy(memry_2, packet_data, length);
		}
		old_length_2 = length;
	}

	//Do not disturb serialize; Do not modify the graph here
}

void *tmp_1_ptr = NULL;
size_t tmp_1_lngth = 0;
void *tmp_2_ptr = NULL;
size_t tmp_2_lngth = 0;
void *tmp_3_ptr = NULL;
size_t tmp_3_lngth = 0;
void *tmp_4_ptr = NULL;
size_t tmp_4_lngth = 0;
void *tmp_5_ptr = NULL;
size_t tmp_5_lngth = 0;
size_t cnt = 0;
void packet_complete_long(const void *packet_data, size_t length)
{

	//save data for later
	cnt++;
	if (cnt == 1)
	{
		tmp_1_ptr = malloc(length);
		if (tmp_1_ptr != NULL)
		{
			memcpy(tmp_1_ptr, packet_data, length);
		}
		tmp_1_lngth = length;
	}
	if (cnt == 2)
	{
		tmp_2_ptr = malloc(length);
		if (tmp_2_ptr != NULL)
		{
			memcpy(tmp_2_ptr, packet_data, length);
		}
		tmp_2_lngth = length;
	}
	if (cnt == 3)
	{
		tmp_3_ptr = malloc(length);
		if (tmp_3_ptr != NULL)
		{
			memcpy(tmp_3_ptr, packet_data, length);
		}
		tmp_3_lngth = length;
	}
	if (cnt == 4)
	{
		tmp_4_ptr = malloc(length);
		if (tmp_4_ptr != NULL)
		{
			memcpy(tmp_4_ptr, packet_data, length);
		}
		tmp_4_lngth = length;
	}
	if (cnt == 5)
	{
		tmp_5_ptr = malloc(length);
		if (tmp_5_ptr != NULL)
		{
			memcpy(tmp_5_ptr, packet_data, length);
		}
		tmp_5_lngth = length;
	}

	//Do not disturb serialize; Do not modify the graph here
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

	serialize(packet_complete_root, 128);

	UNIT_TEST_ASSERT(tmp_length == sizeof(p_node_t));
	UNIT_TEST_ASSERT(((p_node_t *)tmp_packet_data)->addr.u8[0] == 0x01);

	UNIT_TEST_END();

	remove_node(&(r.addr));
	free(tmp_packet_data);
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

	serialize(packet_complete_root, 128);

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
	for (size = 2; size < 11; size++) //9 edges and nodes
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

	//This test works only with this node/edge size
	UNIT_TEST_ASSERT(sizeof(p_node_t) == 6);
	UNIT_TEST_ASSERT(sizeof(p_edge_t) == 10);

	//This test works only with this configuration
	UNIT_TEST_ASSERT(K >= 1);
	UNIT_TEST_ASSERT(MAX_NODES >= 20);
	UNIT_TEST_ASSERT(MAX_EDGES >= 19);

	serialize(packet_complete_multi, 128);

	UNIT_TEST_ASSERT(called == 0x02); //Function should be called 2 times

	//Delete all nodes and edges
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
	p_node_t new_root = {rimeaddr_null};
	new_root.addr.u8[0] = 0x07; //Node number 7
	rimeaddr_set_node_addr(&(new_root.addr));
	add_node(new_root);

	//Deserialize everything
	UNIT_TEST_ASSERT(memry_1 != NULL);
	UNIT_TEST_ASSERT(memry_2 != NULL);
	deserialize(&(root.addr), memry_1, old_length_1);
	deserialize(&(root.addr), memry_2, old_length_2);

	free(memry_1);
	free(memry_2);

	//Check if we got everything
	uint8_t node_count = get_node_count();
	uint8_t edge_count = get_edge_count();

	UNIT_TEST_ASSERT(node_count == 10); //9 drains + root + new root, but new root is a drain
	UNIT_TEST_ASSERT(edge_count == 9); //9 edges from root + one extra edge to new root, but new root is a drain

	//cleanup and test if every edge/node was correctly de/serialized
	for (i = 1; i < size; i++)
	{
		rimeaddr_t node = rimeaddr_null;
		node.u8[0] = i;

		UNIT_TEST_ASSERT(find_node(&node) != NULL);

		if (i != 0x01)
		{
			UNIT_TEST_ASSERT(find_edge(&(root.addr), &node) != NULL);
		}

		remove_node(&node);
		remove_edge(&(root.addr), &node);
	}

	UNIT_TEST_END();
}

/**
 * Graph:
 *                 r                      r -> r2
 *               /  \                   /  \
 *             n11<>n12               n11<>n12
 *             /      \               /      \
 *           n21<---->n22    ->     n21      n22
 *           /          \
 *         n31          n32
 *
 * Test serialize and deserialize with a graph with more than K = 3 levels
 * and (some) bidirectional edges producing a graph
 * Tests the functions:
 * void serialize(void (*packet_complete)(const void *packet_data, size_t length));
 * void deserialize(const rimeaddr_t *sender, const void *packet, size_t length);
 */
UNIT_TEST(long_graph_test)
{
	p_node_t root, n11, n12, n21, n22, n31, n32;
	root.addr = rimeaddr_null;
	n11.addr = rimeaddr_null;
	n12.addr = rimeaddr_null;
	n21.addr = rimeaddr_null;
	n22.addr = rimeaddr_null;
	n31.addr = rimeaddr_null;
	n32.addr = rimeaddr_null;
	root.addr.u8[0] = 0x01;
	n11.addr.u8[0] = 0x02;
	n12.addr.u8[0] = 0x03;
	n21.addr.u8[0] = 0x04;
	n22.addr.u8[0] = 0x05;
	n31.addr.u8[0] = 0x06;
	n32.addr.u8[0] = 0x07;
	rimeaddr_set_node_addr(&(root.addr));
	add_node(root);
	add_node(n11);
	add_node(n12);
	add_node(n21);
	add_node(n22);
	add_node(n31);
	add_node(n32);

	//edge r -> n11
	p_edge_t er11 = {root.addr, n11.addr, 0x00};

	//edge n12 -> r
	p_edge_t e12r = {n12.addr, root.addr, 0x00};

	//edge n11 <-> n12
	p_edge_t e1112 = {n11.addr, n12.addr, 0x00};
	p_edge_t e1211 = {n12.addr, n11.addr, 0x00};

	//edge n21 <-> n11
	p_edge_t e1121 = {n11.addr, n21.addr, 0x00};
	p_edge_t e2111 = {n21.addr, n11.addr, 0x00};

	//edge n22 -> n12
	p_edge_t e2212 = {n22.addr, n12.addr, 0x00};

	//edge n21 <-> n22 (edge is out of K-1 view; between 2 K-1 nodes)
	p_edge_t e2122 = {n21.addr, n22.addr, 0x00};
	p_edge_t e2221 = {n22.addr, n21.addr, 0x00};

	//edge n22 -> n32 (edge is out of K-1 view; between K-1 and K node)
	p_edge_t e2232 = {n22.addr, n32.addr, 0x00};

	//edge n21 <-> n31 (edge is out of K-1 view; between K-1 and K node)
	p_edge_t e2131 = {n21.addr, n31.addr, 0x00};
	p_edge_t e3121 = {n31.addr, n21.addr, 0x00};

	add_edge(er11);
	add_edge(e12r);
	add_edge(e1112);
	add_edge(e1211);
	add_edge(e1121);
	add_edge(e2111);
	add_edge(e2212);
	add_edge(e2122);
	add_edge(e2221);
	add_edge(e2232);
	add_edge(e2131);
	add_edge(e3121);

	UNIT_TEST_BEGIN();

	//This test works only with this node/edge size (or a bit more)
	UNIT_TEST_ASSERT(sizeof(p_node_t) == 6);
	UNIT_TEST_ASSERT(sizeof(p_edge_t) == 10);

	//This test works only with this configuration
	UNIT_TEST_ASSERT(K == 3);
	UNIT_TEST_ASSERT(MAX_NODES >= 7);
	UNIT_TEST_ASSERT(MAX_EDGES >= 12);

	serialize(packet_complete_long, 128);

	UNIT_TEST_ASSERT(cnt == 0x05);

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

	//Create new root
	p_node_t new_root;
	new_root.addr = rimeaddr_null;
	new_root.addr.u8[0] = 0x0A;
	add_node(new_root);
	rimeaddr_set_node_addr(&(new_root.addr));

	//Deserialize everything
	UNIT_TEST_ASSERT(tmp_1_ptr != NULL);
	UNIT_TEST_ASSERT(tmp_2_ptr != NULL);
	UNIT_TEST_ASSERT(tmp_3_ptr != NULL);
	UNIT_TEST_ASSERT(tmp_4_ptr != NULL);
	UNIT_TEST_ASSERT(tmp_5_ptr != NULL);
	deserialize(&(root.addr), tmp_1_ptr, tmp_1_lngth);
	deserialize(&(root.addr), tmp_2_ptr, tmp_2_lngth);
	deserialize(&(root.addr), tmp_3_ptr, tmp_3_lngth);
	deserialize(&(root.addr), tmp_4_ptr, tmp_4_lngth);
	deserialize(&(root.addr), tmp_5_ptr, tmp_5_lngth);

	//Cleanup
	free(tmp_1_ptr);
	free(tmp_2_ptr);
	free(tmp_3_ptr);
	free(tmp_4_ptr);
	free(tmp_5_ptr);

	UNIT_TEST_ASSERT(get_node_count() == 0x06); //K-1 View + new root = K View
	UNIT_TEST_ASSERT(get_edge_count() == 0x08); //All edges to lvl 2 except between lvl 2 + new root edge

	//cleanup and test if every edge/node was correctly de/serialized
	UNIT_TEST_ASSERT(find_node(&(root.addr)) != NULL);
	remove_node(&(root.addr));
	UNIT_TEST_ASSERT(find_edge(&(root.addr), &(n11.addr)) != NULL);
	remove_edge(&(root.addr), &(n11.addr));
	UNIT_TEST_ASSERT(find_edge(&(n12.addr), &(root.addr)) != NULL);
	remove_edge(&(n12.addr), &(root.addr));

	UNIT_TEST_ASSERT(find_node(&(n11.addr)) != NULL);
	remove_node(&(n11.addr));
	UNIT_TEST_ASSERT(find_edge(&(n11.addr), &(n12.addr)) != NULL);
	remove_edge(&(n11.addr), &(n12.addr));
	UNIT_TEST_ASSERT(find_edge(&(n12.addr), &(n11.addr)) != NULL);
	remove_edge(&(n12.addr), &(n11.addr));
	UNIT_TEST_ASSERT(find_edge(&(n11.addr), &(n21.addr)) != NULL);
	remove_edge(&(n11.addr), &(n21.addr));
	UNIT_TEST_ASSERT(find_edge(&(n21.addr), &(n11.addr)) != NULL);
	remove_edge(&(n21.addr), &(n11.addr));

	UNIT_TEST_ASSERT(find_node(&(n12.addr)) != NULL);
	remove_node(&(n12.addr));
	UNIT_TEST_ASSERT(find_edge(&(n22.addr), &(n12.addr)) != NULL);
	remove_edge(&(n22.addr), &(n12.addr));

	UNIT_TEST_ASSERT(find_node(&(n21.addr)) != NULL);
	remove_node(&(n21.addr));

	UNIT_TEST_ASSERT(find_node(&(n22.addr)) != NULL);
	remove_node(&(n22.addr));

	UNIT_TEST_ASSERT(find_node(&(new_root.addr)) != NULL);
	remove_node(&(new_root.addr));
	UNIT_TEST_ASSERT(find_edge(&(root.addr), &(new_root.addr)) != NULL);
	remove_edge(&(root.addr), &(new_root.addr));

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
