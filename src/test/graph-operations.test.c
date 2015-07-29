#include "contiki.h"
#include "unit-test.h"

#include <stdlib.h>

#include "sys/clock.h"
#include "graph.h"
#include "graph-operations.h"

/**
 * Register unit tests that will be executed by using
 * the UNIT_TEST_RUN macro.
 */
UNIT_TEST_REGISTER(empty_hop, "Test empty graph get_hop_count");
UNIT_TEST_REGISTER(null_hop, "Test NULL pointer parameter get_hop_count");
UNIT_TEST_REGISTER(cyclic_hop, "Test cyclic graph get_hop_count");
UNIT_TEST_REGISTER(no_hop, "Test no reachable node get_hop_count");
UNIT_TEST_REGISTER(omnidirectional_hop, "Test with omnidirectional edges get_hop_count");

UNIT_TEST_REGISTER(empty_purge, "Test on empty and only root graph purge");
UNIT_TEST_REGISTER(delete_and_decrement_purge, "Test delete and decrement purge");

/**
 * Test get hopcounts with an empty graph
 * Tests the functions:
 * p_hop_t *get_hop_counts(uint8_t *count);
 */
UNIT_TEST(empty_hop)
{
	UNIT_TEST_BEGIN();

	uint8_t count;
	p_hop_t *ptr = get_hop_counts(&count);

	UNIT_TEST_ASSERT(count == 0x00);
	UNIT_TEST_ASSERT(ptr == NULL);

	UNIT_TEST_END();
}

/**
 * Test get hopcounts with invalid arguments
 * Tests the functions:
 * p_hop_t *get_hop_counts(uint8_t *count);
 */
UNIT_TEST(null_hop)
{
	UNIT_TEST_BEGIN();

	p_hop_t *ptr = get_hop_counts(NULL);

	UNIT_TEST_ASSERT(ptr == NULL);

	UNIT_TEST_END();
}

/**
 * Graph:
 *            r
 *  to r ->  / \
 *        n11   n12     n0
 *         /     \
 *      n21------>n22<---n23
 * Test get hopcount of a cyclic graph with one node not reachable
 * and the left subtree only reachable by ignoring edge direction
 * Tests the functions:
 * p_hop_t *get_hop_counts(uint8_t *count);
 */
UNIT_TEST(cyclic_hop)
{
	p_node_t r, n11, n12, n21, n22, n23, n0;

	r.addr = rimeaddr_null;
	n11.addr = rimeaddr_null;
	n12.addr = rimeaddr_null;
	n21.addr = rimeaddr_null;
	n22.addr = rimeaddr_null;
	n23.addr = rimeaddr_null;
	n0.addr = rimeaddr_null;
	r.addr.u8[0] = 0x01;
	n11.addr.u8[0] = 0x02;
	n12.addr.u8[0] = 0x03;
	n21.addr.u8[0] = 0x04;
	n22.addr.u8[0] = 0x05;
	n23.addr.u8[0] = 0x06;
	n0.addr.u8[0] = 0x07;

	p_edge_t e11r = {n11.addr, r.addr, 0x00};
	p_edge_t er12 = {r.addr, n12.addr, 0x00};
	p_edge_t e1121 = {n11.addr, n21.addr, 0x00};
	p_edge_t e1222 = {n12.addr, n22.addr, 0x00};
	p_edge_t e2122 = {n21.addr, n22.addr, 0x00};
	p_edge_t e2322 = {n23.addr, n22.addr, 0x00};

	rimeaddr_set_node_addr(&(r.addr));

	//Randomly add the nodes and edges
	add_node(r);
	add_node(n22);
	add_node(n12);
	add_node(n0);
	add_node(n23);
	add_node(n11);
	add_node(n21);

	add_edge(e2122);
	add_edge(er12);
	add_edge(e2322);
	add_edge(e1121);
	add_edge(e11r);
	add_edge(e1222);

	UNIT_TEST_BEGIN();

	uint8_t count;
	p_hop_t *ptr = get_hop_counts(&count);

	UNIT_TEST_ASSERT(count == 0x05);

	UNIT_TEST_ASSERT(ptr[1].addr.u8[0] == n11.addr.u8[0]);
	UNIT_TEST_ASSERT(ptr[1].hop_count == 0x01);

	UNIT_TEST_ASSERT(ptr[0].addr.u8[0] == n12.addr.u8[0]);
	UNIT_TEST_ASSERT(ptr[0].hop_count == 0x01);

	UNIT_TEST_ASSERT(ptr[3].addr.u8[0] == n21.addr.u8[0]);
	UNIT_TEST_ASSERT(ptr[3].hop_count == 0x02);

	UNIT_TEST_ASSERT(ptr[2].addr.u8[0] == n22.addr.u8[0]);
	UNIT_TEST_ASSERT(ptr[2].hop_count == 0x02);

	UNIT_TEST_ASSERT(ptr[4].addr.u8[0] == n23.addr.u8[0]);
	UNIT_TEST_ASSERT(ptr[4].hop_count == 0x03);

	free(ptr);

	UNIT_TEST_END();

	remove_node(&(r.addr));
	remove_node(&(n11.addr));
	remove_node(&(n12.addr));
	remove_node(&(n21.addr));
	remove_node(&(n22.addr));
	remove_node(&(n23.addr));
	remove_node(&(n0.addr));

	remove_edge(&(n11.addr), &(r.addr));
	remove_edge(&(r.addr), &(n12.addr));
	remove_edge(&(n11.addr), &(n21.addr));
	remove_edge(&(n12.addr), &(n22.addr));
	remove_edge(&(n21.addr), &(n22.addr));
	remove_edge(&(n23.addr), &(n22.addr));

}

/**
 * Graph:
 *            r
 *           / \
 *        n11   n12
 *         /     \
 *      n21<----->n22
 * Test get hopcount of a cyclic graph with all edges in both directions
 * Tests the functions:
 * p_hop_t *get_hop_counts(uint8_t *count);
 */
UNIT_TEST(omnidirectional_hop)
{
	p_node_t r, n11, n12, n21, n22;

	r.addr = rimeaddr_null;
	n11.addr = rimeaddr_null;
	n12.addr = rimeaddr_null;
	n21.addr = rimeaddr_null;
	n22.addr = rimeaddr_null;
	r.addr.u8[0] = 0x01;
	n11.addr.u8[0] = 0x02;
	n12.addr.u8[0] = 0x03;
	n21.addr.u8[0] = 0x04;
	n22.addr.u8[0] = 0x05;

	p_edge_t e11r = {n11.addr, r.addr, 0x00};
	p_edge_t er11 = {r.addr, n11.addr, 0x00};

	p_edge_t er12 = {r.addr, n12.addr, 0x00};
	p_edge_t e12r = {n12.addr, r.addr, 0x00};

	p_edge_t e1121 = {n11.addr, n21.addr, 0x00};
	p_edge_t e2111 = {n21.addr, n11.addr, 0x00};

	p_edge_t e1222 = {n12.addr, n22.addr, 0x00};
	p_edge_t e2212 = {n22.addr, n12.addr, 0x00};

	p_edge_t e2122 = {n21.addr, n22.addr, 0x00};
	p_edge_t e2221 = {n22.addr, n21.addr, 0x00};

	rimeaddr_set_node_addr(&(r.addr));

	//Randomly add the nodes and edges
	add_node(r);
	add_node(n22);
	add_node(n12);
	add_node(n11);
	add_node(n21);

	add_edge(e2122);
	add_edge(er12);
	add_edge(e1121);
	add_edge(e11r);
	add_edge(e1222);
	add_edge(e2221);
	add_edge(e12r);
	add_edge(e2111);
	add_edge(er11);
	add_edge(e2212);

	UNIT_TEST_BEGIN();

	uint8_t count;
	p_hop_t *ptr = get_hop_counts(&count);

	UNIT_TEST_ASSERT(count == 0x04);

	UNIT_TEST_ASSERT(ptr[1].addr.u8[0] == n11.addr.u8[0]);
	UNIT_TEST_ASSERT(ptr[1].hop_count == 0x01);

	UNIT_TEST_ASSERT(ptr[0].addr.u8[0] == n12.addr.u8[0]);
	UNIT_TEST_ASSERT(ptr[0].hop_count == 0x01);

	UNIT_TEST_ASSERT(ptr[3].addr.u8[0] == n21.addr.u8[0]);
	UNIT_TEST_ASSERT(ptr[3].hop_count == 0x02);

	UNIT_TEST_ASSERT(ptr[2].addr.u8[0] == n22.addr.u8[0]);
	UNIT_TEST_ASSERT(ptr[2].hop_count == 0x02);

	free(ptr);

	UNIT_TEST_END();

	remove_node(&(r.addr));
	remove_node(&(n11.addr));
	remove_node(&(n12.addr));
	remove_node(&(n21.addr));
	remove_node(&(n22.addr));

	remove_edge(&(n11.addr), &(r.addr));
	remove_edge(&(r.addr), &(n11.addr));
	remove_edge(&(r.addr), &(n12.addr));
	remove_edge(&(n12.addr), &(r.addr));
	remove_edge(&(n11.addr), &(n21.addr));
	remove_edge(&(n21.addr), &(n11.addr));
	remove_edge(&(n12.addr), &(n22.addr));
	remove_edge(&(n22.addr), &(n12.addr));
	remove_edge(&(n21.addr), &(n22.addr));
	remove_edge(&(n22.addr), &(n21.addr));

}

/**
 * Graph:
 *            r
 *
 *        n11--->n12
 * Test get hopcount of a graph with no node reachable from root
 * Tests the functions:
 * p_hop_t *get_hop_counts(uint8_t *count);
 */
UNIT_TEST(no_hop)
{
	p_node_t r, n11, n12;

	r.addr = rimeaddr_null;
	n11.addr = rimeaddr_null;
	n12.addr = rimeaddr_null;
	r.addr.u8[0] = 0x01;
	n11.addr.u8[0] = 0x02;
	n12.addr.u8[0] = 0x03;

	p_edge_t e1112 = {n11.addr, n12.addr, 0x00};

	rimeaddr_set_node_addr(&(r.addr));

	add_node(r);
	add_node(n11);
	add_node(n12);

	add_edge(e1112);

	UNIT_TEST_BEGIN();

	uint8_t count;
	p_hop_t *ptr = get_hop_counts(&count);

	UNIT_TEST_ASSERT(count == 0x00);

	free(ptr);

	UNIT_TEST_END();

	remove_node(&(r.addr));
	remove_node(&(n11.addr));
	remove_node(&(n12.addr));

	remove_edge(&(n11.addr), &(n12.addr));

}

/**
 * Graph:
 *      r     and    <empty>
 * Test purge with a graph with no nodes or only root
 * Tests the functions:
 * void purge();
 */
UNIT_TEST(empty_purge)
{
	p_node_t r;
	r.addr = rimeaddr_null;
	r.addr.u8[0] = 0x01;

	rimeaddr_set_node_addr(&(r.addr));

	UNIT_TEST_BEGIN();

	purge();

	add_node(r);

	purge();

	uint8_t node_count;
	p_node_t **all_nodes = get_all_nodes(&node_count);

	//Root should still be there
	UNIT_TEST_ASSERT(all_nodes[0]->addr.u8[0] == r.addr.u8[0]);
	UNIT_TEST_ASSERT(node_count == 0x01);

	UNIT_TEST_END();

	remove_node(&(r.addr));
}

/**
 * Graph:
 *     r         n1             r                    r
 *    / \        |      ->       \           ->
 * n11   n12     n0               n12
 * Test purge with a graph with edges to be deleted and to be decremented
 * and nodes to be deleted.
 * Tests the functions:
 * void purge();
 */
UNIT_TEST(delete_and_decrement_purge)
{

	p_node_t r, n11, n12, n0, n1;

	r.addr = rimeaddr_null;
	n11.addr = rimeaddr_null;
	n12.addr = rimeaddr_null;
	n0.addr = rimeaddr_null;
	n1.addr = rimeaddr_null;
	r.addr.u8[0] = 0x01;
	n11.addr.u8[0] = 0x02;
	n12.addr.u8[0] = 0x03;
	n0.addr.u8[0] = 0x06;
	n1.addr.u8[0] = 0x07;

	p_edge_t er11 = {r.addr, n11.addr, 0x01}; //1 minute ttl
	p_edge_t er12 = {r.addr, n12.addr, 0x0A}; //10 minute ttl
	p_edge_t e10 = {n1.addr, n0.addr, 0x01}; //1 minute ttl

	rimeaddr_set_node_addr(&(r.addr));

	add_node(r);
	add_node(n12);
	add_node(n0);
	add_node(n11);
	add_node(n1);

	add_edge(er12);
	add_edge(er11);
	add_edge(e10);

	UNIT_TEST_BEGIN();

	clock_set_seconds((unsigned long) 300); //Set clock to 5 minutes

	purge();

	uint8_t node_count;
	uint8_t edge_count;
	p_edge_t **all_edges = get_all_edges(&edge_count);
	p_node_t **all_nodes = get_all_nodes(&node_count);

	UNIT_TEST_ASSERT(all_edges[0]->src.u8[0] == r.addr.u8[0]);
	UNIT_TEST_ASSERT(all_edges[0]->dst.u8[0] == n12.addr.u8[0]);
	UNIT_TEST_ASSERT(edge_count == 0x01);
	UNIT_TEST_ASSERT(all_edges[0]->ttl == 0x05); //5 Minutes left

	//Nodes only 2 nodes left
	UNIT_TEST_ASSERT(all_nodes[0]->addr.u8[0] == r.addr.u8[0]);
	UNIT_TEST_ASSERT(all_nodes[1]->addr.u8[0] == n12.addr.u8[0]);
	UNIT_TEST_ASSERT(node_count == 0x02);

	//Test for clock overflow
	clock_set_seconds((unsigned long) 60); //Set clock to 1 Minute

	purge();

	all_edges = get_all_edges(&edge_count);
	all_nodes = get_all_nodes(&node_count);

	//Last edge should be deleted
	UNIT_TEST_ASSERT(edge_count == 0x00);

	//Every node is deleted except root
	UNIT_TEST_ASSERT(all_nodes[0]->addr.u8[0] == r.addr.u8[0]);
	UNIT_TEST_ASSERT(node_count == 0x01);

	UNIT_TEST_END();

	remove_node(&(r.addr));
}

int main()
{
	init_graph();

	//p_hop_t *get_hop_counts(uint8_t *count);
	UNIT_TEST_RUN(empty_hop);
	UNIT_TEST_RUN(null_hop);
	UNIT_TEST_RUN(cyclic_hop);
	UNIT_TEST_RUN(no_hop);
	UNIT_TEST_RUN(omnidirectional_hop);

	//void purge();
	UNIT_TEST_RUN(empty_purge);
	UNIT_TEST_RUN(delete_and_decrement_purge);

	return EXIT_SUCCESS;
}
