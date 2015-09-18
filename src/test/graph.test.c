#include "contiki.h"
#include "unit-test.h"

#include <stdlib.h>

#include "graph.h"
#include "positions.h"

/**
 * Register unit tests that will be executed by using
 * the UNIT_TEST_RUN macro.
 */
UNIT_TEST_REGISTER(add_nodes, "Test adding and removing nodes");
UNIT_TEST_REGISTER(add_edges, "Test adding and removing edges");
UNIT_TEST_REGISTER(find_nodes, "Test finding nodes");
UNIT_TEST_REGISTER(find_edges, "Test finding edges");
UNIT_TEST_REGISTER(in_out_edges, "Test incoming and outgoing edges of a node");
UNIT_TEST_REGISTER(test_NULL_param, "Test if functions return without doing anything if a parameter is NULL");
UNIT_TEST_REGISTER(test_members, "Test if all members of the node and edge struct are saved correctly");

/**
 * Test adding and removing some nodes
 * Tests the functions:
 * void add_node(const p_node_t node)
 * void remove_node(const rimeaddr_t *addr)
 * p_node_t **get_all_nodes(uint8_t * count)
 * uint8_t get_node_count()
 */
UNIT_TEST(add_nodes)
{
	p_node_t n1, n2, n3;
	p_node_t ntemp;
	p_node_t **node_array;
	uint8_t counter;

	UNIT_TEST_BEGIN();

	n1.addr = rimeaddr_null;
	n2.addr = rimeaddr_null;
	n3.addr = rimeaddr_null;
	n1.addr.u8[0] = 0x01;
	n2.addr.u8[0] = 0x02;
	n3.addr.u8[0] = 0x03;
	node_array = get_all_nodes(&counter);

	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(counter == 0);
	UNIT_TEST_ASSERT(node_array[0] == NULL);

	add_node(n1);
	node_array = get_all_nodes(&counter);

	UNIT_TEST_ASSERT(get_node_count() == 1);
	UNIT_TEST_ASSERT(counter == 1);
	UNIT_TEST_ASSERT(node_array[0] != NULL);
	UNIT_TEST_ASSERT(node_array[1] == NULL);

	add_node(n2);
	node_array = get_all_nodes(&counter);

	UNIT_TEST_ASSERT(get_node_count() == 2);
	UNIT_TEST_ASSERT(counter == 2);
	UNIT_TEST_ASSERT(node_array[0] != NULL);
	UNIT_TEST_ASSERT(node_array[1] != NULL);
	UNIT_TEST_ASSERT(node_array[2] == NULL);

	add_node(n3);
	node_array = get_all_nodes(&counter);

	UNIT_TEST_ASSERT(get_node_count() == 3);
	UNIT_TEST_ASSERT(counter == 3);
	UNIT_TEST_ASSERT(node_array[0] != NULL);
	UNIT_TEST_ASSERT(node_array[1] != NULL);
	UNIT_TEST_ASSERT(node_array[2] != NULL);
	UNIT_TEST_ASSERT(node_array[3] == NULL);

	add_node(n1);
	node_array = get_all_nodes(&counter);

	UNIT_TEST_ASSERT(get_node_count() == 3);
	UNIT_TEST_ASSERT(counter == 3);
	UNIT_TEST_ASSERT(node_array[0] != NULL);
	UNIT_TEST_ASSERT(node_array[1] != NULL);
	UNIT_TEST_ASSERT(node_array[2] != NULL);
	UNIT_TEST_ASSERT(node_array[3] == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(node_array[0]->addr), &(n1.addr)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(node_array[1]->addr), &(n2.addr)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(node_array[2]->addr), &(n3.addr)));

	remove_node(&(n3.addr));

	UNIT_TEST_ASSERT(get_node_count() == 2);
	UNIT_TEST_ASSERT(node_array[0] != NULL);
	UNIT_TEST_ASSERT(node_array[1] != NULL);
	UNIT_TEST_ASSERT(node_array[2] == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(node_array[0]->addr), &(n1.addr)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(node_array[1]->addr), &(n2.addr)));

	remove_node(&(n3.addr));

	UNIT_TEST_ASSERT(get_node_count() == 2);
	UNIT_TEST_ASSERT(node_array[0] != NULL);
	UNIT_TEST_ASSERT(node_array[1] != NULL);
	UNIT_TEST_ASSERT(node_array[2] == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(node_array[0]->addr), &(n1.addr)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(node_array[1]->addr), &(n2.addr)));

	remove_node(&(n1.addr));

	UNIT_TEST_ASSERT(get_node_count() == 1);
	UNIT_TEST_ASSERT(node_array[0] != NULL);
	UNIT_TEST_ASSERT(node_array[1] == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(node_array[0]->addr), &(n2.addr)));

	remove_node(&(n2.addr));

	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(node_array[0] == NULL);

	//Try to remove nodes when there are non saved

	remove_node(&(n1.addr));
	remove_node(&(n2.addr));
	remove_node(&(n3.addr));

	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(node_array[0] == NULL);

	//Test what happens if there are more nodes added than MEMB allocated space
	uint8_t i;
	for (i = 0; i < (MAX_NODES + 1); i++)
	{
		ntemp.addr = rimeaddr_null;
		ntemp.addr.u8[0] = (unsigned char) i;
		add_node(ntemp);
	}

	UNIT_TEST_ASSERT(get_node_count() == MAX_NODES);

	node_array = get_all_nodes(&counter);
	for (i = 0; i < counter; i++)
	{
		remove_node(&(node_array[0]->addr));
	}

	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(node_array[0] == NULL);

	UNIT_TEST_END();
}

/**
 * Test adding and removing some edges
 * Tests the functions:
 * void add_edge(const p_edge_t edge)
 * void remove_edge(const rimeaddr_t *src, const rimeaddr_t *dst)
 * p_edge_t **get_all_edges(uint8_t * count)
 * uint8_t get_edge_count()
 */
UNIT_TEST(add_edges)
{
	p_node_t n1, n2, n3, n4, n7;
	p_edge_t e1, e2, e3, e4, e5, e6, e1_new;
	p_edge_t etemp;
	p_edge_t **edge_array;
	p_node_t **node_array;
	uint8_t counter;

	UNIT_TEST_BEGIN();

	e1.dst = rimeaddr_null;
	e1.src = rimeaddr_null;
	e2.dst = rimeaddr_null;
	e2.src = rimeaddr_null;
	e3.dst = rimeaddr_null;
	e3.src = rimeaddr_null;
	e4.dst = rimeaddr_null;
	e4.src = rimeaddr_null;
	e5.dst = rimeaddr_null;
	e5.src = rimeaddr_null;
	e6.dst = rimeaddr_null;
	e6.src = rimeaddr_null;
	e1_new.dst = rimeaddr_null;
	e1_new.src = rimeaddr_null;
	e1.dst.u8[0] = 0x01;
	e1.src.u8[0] = 0x02;
	e2.dst.u8[0] = 0x02;
	e2.src.u8[0] = 0x03;
	e3.dst.u8[0] = 0x04;
	e3.src.u8[0] = 0x07;
	e4.dst.u8[0] = 0x02;
	e4.src.u8[0] = 0x01;
	e5.dst.u8[0] = 0x01;
	e5.src.u8[0] = 0x03;
	e6.dst.u8[0] = 0x01;
	e6.src.u8[0] = 0x01;
	e1_new.dst.u8[0] = 0x01;
	e1_new.src.u8[0] = 0x02;
	e1.ttl = 0;
	e2.ttl = 0;
	e3.ttl = 0;
	e4.ttl = 0;
	e5.ttl = 0;
	e6.ttl = 0;
	e1_new.ttl = 1;
	edge_array = get_all_edges(&counter);
	n1.addr = e1.dst;
	n2.addr = e1.src;
	n3.addr = e2.src;
	n4.addr = e3.dst;
	n7.addr = e3.src;

	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(counter == 0);
	UNIT_TEST_ASSERT(edge_array[0] == NULL);

	add_node(n1);
	add_node(n2);
	add_node(n3);
	add_node(n4);
	add_node(n7);

	add_edge(e1);
	edge_array = get_all_edges(&counter);

	UNIT_TEST_ASSERT(get_edge_count() == 1);
	UNIT_TEST_ASSERT(counter == 1);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] == NULL);

	add_edge(e2);
	edge_array = get_all_edges(&counter);

	UNIT_TEST_ASSERT(get_edge_count() == 2);
	UNIT_TEST_ASSERT(counter == 2);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] == NULL);

	add_edge(e3);
	edge_array = get_all_edges(&counter);

	UNIT_TEST_ASSERT(get_edge_count() == 3);
	UNIT_TEST_ASSERT(counter == 3);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] != NULL);
	UNIT_TEST_ASSERT(edge_array[3] == NULL);

	add_edge(e4);
	edge_array = get_all_edges(&counter);

	UNIT_TEST_ASSERT(get_edge_count() == 4);
	UNIT_TEST_ASSERT(counter == 4);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] != NULL);
	UNIT_TEST_ASSERT(edge_array[3] != NULL);
	UNIT_TEST_ASSERT(edge_array[4] == NULL);

	add_edge(e5);
	edge_array = get_all_edges(&counter);

	UNIT_TEST_ASSERT(get_edge_count() == 5);
	UNIT_TEST_ASSERT(counter == 5);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] != NULL);
	UNIT_TEST_ASSERT(edge_array[3] != NULL);
	UNIT_TEST_ASSERT(edge_array[4] != NULL);
	UNIT_TEST_ASSERT(edge_array[5] == NULL);

	add_edge(e6);
	edge_array = get_all_edges(&counter);

	UNIT_TEST_ASSERT(get_edge_count() == 6);
	UNIT_TEST_ASSERT(counter == 6);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] != NULL);
	UNIT_TEST_ASSERT(edge_array[3] != NULL);
	UNIT_TEST_ASSERT(edge_array[4] != NULL);
	UNIT_TEST_ASSERT(edge_array[5] != NULL);
	UNIT_TEST_ASSERT(edge_array[6] == NULL);

	add_edge(e1);
	edge_array = get_all_edges(&counter);

	UNIT_TEST_ASSERT(get_edge_count() == 6);
	UNIT_TEST_ASSERT(counter == 6);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] != NULL);
	UNIT_TEST_ASSERT(edge_array[3] != NULL);
	UNIT_TEST_ASSERT(edge_array[4] != NULL);
	UNIT_TEST_ASSERT(edge_array[5] != NULL);
	UNIT_TEST_ASSERT(edge_array[6] == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[0]->src), &(e1.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e1.dst)) && (edge_array[0]->ttl == e1.ttl));
	UNIT_TEST_ASSERT((rimeaddr_cmp(&(edge_array[0]->src), &(e1_new.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e1_new.dst)) && (edge_array[0]->ttl == e1_new.ttl)) == 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[1]->src), &(e2.src)) && rimeaddr_cmp(&(edge_array[1]->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[2]->src), &(e3.src)) && rimeaddr_cmp(&(edge_array[2]->dst), &(e3.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[3]->src), &(e4.src)) && rimeaddr_cmp(&(edge_array[3]->dst), &(e4.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[4]->src), &(e5.src)) && rimeaddr_cmp(&(edge_array[4]->dst), &(e5.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[5]->src), &(e6.src)) && rimeaddr_cmp(&(edge_array[5]->dst), &(e6.dst)));

	add_edge(e1_new);
	edge_array = get_all_edges(&counter);

	UNIT_TEST_ASSERT(get_edge_count() == 6);
	UNIT_TEST_ASSERT(counter == 6);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] != NULL);
	UNIT_TEST_ASSERT(edge_array[3] != NULL);
	UNIT_TEST_ASSERT(edge_array[4] != NULL);
	UNIT_TEST_ASSERT(edge_array[5] != NULL);
	UNIT_TEST_ASSERT(edge_array[6] == NULL);
	UNIT_TEST_ASSERT((rimeaddr_cmp(&(edge_array[0]->src), &(e1.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e1.dst)) && (edge_array[0]->ttl == e1.ttl)) == 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[0]->src), &(e1_new.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e1_new.dst)) && (edge_array[0]->ttl == e1_new.ttl));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[1]->src), &(e2.src)) && rimeaddr_cmp(&(edge_array[1]->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[2]->src), &(e3.src)) && rimeaddr_cmp(&(edge_array[2]->dst), &(e3.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[3]->src), &(e4.src)) && rimeaddr_cmp(&(edge_array[3]->dst), &(e4.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[4]->src), &(e5.src)) && rimeaddr_cmp(&(edge_array[4]->dst), &(e5.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[5]->src), &(e6.src)) && rimeaddr_cmp(&(edge_array[5]->dst), &(e6.dst)));

	add_edge(e1);
	edge_array = get_all_edges(&counter);

	UNIT_TEST_ASSERT(get_edge_count() == 6);
	UNIT_TEST_ASSERT(counter == 6);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] != NULL);
	UNIT_TEST_ASSERT(edge_array[3] != NULL);
	UNIT_TEST_ASSERT(edge_array[4] != NULL);
	UNIT_TEST_ASSERT(edge_array[5] != NULL);
	UNIT_TEST_ASSERT(edge_array[6] == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[0]->src), &(e1.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e1.dst)) && (edge_array[0]->ttl == e1.ttl));
	UNIT_TEST_ASSERT((rimeaddr_cmp(&(edge_array[0]->src), &(e1_new.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e1_new.dst)) && (edge_array[0]->ttl == e1_new.ttl)) == 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[1]->src), &(e2.src)) && rimeaddr_cmp(&(edge_array[1]->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[2]->src), &(e3.src)) && rimeaddr_cmp(&(edge_array[2]->dst), &(e3.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[3]->src), &(e4.src)) && rimeaddr_cmp(&(edge_array[3]->dst), &(e4.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[4]->src), &(e5.src)) && rimeaddr_cmp(&(edge_array[4]->dst), &(e5.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[5]->src), &(e6.src)) && rimeaddr_cmp(&(edge_array[5]->dst), &(e6.dst)));

	remove_edge(&(e6.src), &(e6.dst));

	UNIT_TEST_ASSERT(get_edge_count() == 5);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] != NULL);
	UNIT_TEST_ASSERT(edge_array[3] != NULL);
	UNIT_TEST_ASSERT(edge_array[4] != NULL);
	UNIT_TEST_ASSERT(edge_array[5] == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[0]->src), &(e1.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e1.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[1]->src), &(e2.src)) && rimeaddr_cmp(&(edge_array[1]->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[2]->src), &(e3.src)) && rimeaddr_cmp(&(edge_array[2]->dst), &(e3.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[3]->src), &(e4.src)) && rimeaddr_cmp(&(edge_array[3]->dst), &(e4.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[4]->src), &(e5.src)) && rimeaddr_cmp(&(edge_array[4]->dst), &(e5.dst)));

	remove_edge(&(e6.src), &(e6.dst));

	UNIT_TEST_ASSERT(get_edge_count() == 5);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] != NULL);
	UNIT_TEST_ASSERT(edge_array[3] != NULL);
	UNIT_TEST_ASSERT(edge_array[4] != NULL);
	UNIT_TEST_ASSERT(edge_array[5] == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[0]->src), &(e1.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e1.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[1]->src), &(e2.src)) && rimeaddr_cmp(&(edge_array[1]->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[2]->src), &(e3.src)) && rimeaddr_cmp(&(edge_array[2]->dst), &(e3.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[3]->src), &(e4.src)) && rimeaddr_cmp(&(edge_array[3]->dst), &(e4.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[4]->src), &(e5.src)) && rimeaddr_cmp(&(edge_array[4]->dst), &(e5.dst)));

	remove_edge(&(e3.src), &(e3.dst));

	UNIT_TEST_ASSERT(get_edge_count() == 4);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] != NULL);
	UNIT_TEST_ASSERT(edge_array[3] != NULL);
	UNIT_TEST_ASSERT(edge_array[4] == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[0]->src), &(e1.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e1.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[1]->src), &(e2.src)) && rimeaddr_cmp(&(edge_array[1]->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[2]->src), &(e4.src)) && rimeaddr_cmp(&(edge_array[2]->dst), &(e4.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[3]->src), &(e5.src)) && rimeaddr_cmp(&(edge_array[3]->dst), &(e5.dst)));

	remove_edge(&(e1.src), &(e1.dst));

	UNIT_TEST_ASSERT(get_edge_count() == 3);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] != NULL);
	UNIT_TEST_ASSERT(edge_array[3] == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[0]->src), &(e2.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[1]->src), &(e4.src)) && rimeaddr_cmp(&(edge_array[1]->dst), &(e4.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[2]->src), &(e5.src)) && rimeaddr_cmp(&(edge_array[2]->dst), &(e5.dst)));

	remove_edge(&(e5.src), &(e5.dst));

	UNIT_TEST_ASSERT(get_edge_count() == 2);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] != NULL);
	UNIT_TEST_ASSERT(edge_array[2] == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[0]->src), &(e2.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[1]->src), &(e4.src)) && rimeaddr_cmp(&(edge_array[1]->dst), &(e4.dst)));

	remove_edge(&(e4.src), &(e4.dst));

	UNIT_TEST_ASSERT(get_edge_count() == 1);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[0]->src), &(e2.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e2.dst)));

	remove_edge(&(e2.src), &(e2.dst));

	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(edge_array[0] == NULL);

	remove_node(&(n1.addr));
	remove_node(&(n2.addr));
	remove_node(&(n3.addr));
	remove_node(&(n4.addr));
	remove_node(&(n7.addr));

	//Try to remove edges when there are non saved

	remove_edge(&(e1.src), &(e1.dst));
	remove_edge(&(e2.src), &(e2.dst));
	remove_edge(&(e3.src), &(e3.dst));
	remove_edge(&(e4.src), &(e4.dst));
	remove_edge(&(e5.src), &(e5.dst));
	remove_edge(&(e6.src), &(e6.dst));
	remove_edge(&(e1_new.src), &(e1_new.dst));

	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(edge_array[0] == NULL);

	//Test what happens if there are more edges added than MEMB allocated space
	
	//Can we create enough edges?
	UNIT_TEST_ASSERT(MAX_NODES*(MAX_NODES-1) > MAX_EDGES);
	
	//Add MAX_NODES nodes
	uint8_t i;
	for (i = 1; i <= MAX_NODES; i++)
	{
		p_node_t n = {rimeaddr_null};
		n.addr.u8[0] = (unsigned char) i;
		add_node(n);
	}
	
	UNIT_TEST_ASSERT(get_node_count() == MAX_NODES);
	
	//Add MAX_EDGES + 1 edges 
	for (i = 0; i < (MAX_EDGES + 1); i++)
	{
		etemp.dst = rimeaddr_null;
		etemp.src = rimeaddr_null;
		etemp.src.u8[0] = ((unsigned char)(i/MAX_NODES)) + 1;
		etemp.dst.u8[0] = (unsigned char)(i % MAX_NODES) + 1;
		etemp.ttl = 0;
		add_edge(etemp);
	}

	UNIT_TEST_ASSERT(get_edge_count() == MAX_EDGES);

	edge_array = get_all_edges(&counter);
	for (i = 0; i < counter; i++)
	{
		remove_edge(&(edge_array[0]->src), &(edge_array[0]->dst));
	}

	node_array = get_all_nodes(&counter);
	for (i = 0; i < counter; i++)
	{
		remove_node(&(node_array[0]->addr));
	}

	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(edge_array[0] == NULL);

	UNIT_TEST_END();
}

/**
 * Test find_node after adding and removing some nodes
 * Tests the functions:
 * p_node_t *find_node(const rimeaddr_t *addr)
 * void add_node(const p_node_t node)
 * void remove_node(const rimeaddr_t *addr)
 * p_node_t **get_all_nodes(uint8_t * count)
 * uint8_t get_node_count()
 */
UNIT_TEST(find_nodes)
{
	p_node_t n1, n2, n3;
	p_node_t **node_array;
	uint8_t counter;

	UNIT_TEST_BEGIN();

	n1.addr = rimeaddr_null;
	n2.addr = rimeaddr_null;
	n3.addr = rimeaddr_null;
	n1.addr.u8[0] = 0x01;
	n2.addr.u8[0] = 0x02;
	n3.addr.u8[0] = 0x03;
	node_array = get_all_nodes(&counter);

	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(counter == 0);
	UNIT_TEST_ASSERT(node_array[0] == NULL);
	UNIT_TEST_ASSERT(find_node(&(n1.addr)) == NULL);
	UNIT_TEST_ASSERT(find_node(&(n2.addr)) == NULL);
	UNIT_TEST_ASSERT(find_node(&(n3.addr)) == NULL);

	add_node(n1);

	UNIT_TEST_ASSERT(find_node(&(n1.addr)) != NULL);
	UNIT_TEST_ASSERT(find_node(&(n2.addr)) == NULL);
	UNIT_TEST_ASSERT(find_node(&(n3.addr)) == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_node(&(n1.addr))->addr), &(n1.addr)));

	add_node(n2);

	UNIT_TEST_ASSERT(find_node(&(n1.addr)) != NULL);
	UNIT_TEST_ASSERT(find_node(&(n2.addr)) != NULL);
	UNIT_TEST_ASSERT(find_node(&(n3.addr)) == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_node(&(n1.addr))->addr), &(n1.addr)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_node(&(n2.addr))->addr), &(n2.addr)));

	add_node(n3);

	UNIT_TEST_ASSERT(find_node(&(n1.addr)) != NULL);
	UNIT_TEST_ASSERT(find_node(&(n2.addr)) != NULL);
	UNIT_TEST_ASSERT(find_node(&(n3.addr)) != NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_node(&(n1.addr))->addr), &(n1.addr)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_node(&(n2.addr))->addr), &(n2.addr)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_node(&(n3.addr))->addr), &(n3.addr)));

	remove_node(&(n3.addr));

	UNIT_TEST_ASSERT(find_node(&(n1.addr)) != NULL);
	UNIT_TEST_ASSERT(find_node(&(n2.addr)) != NULL);
	UNIT_TEST_ASSERT(find_node(&(n3.addr)) == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_node(&(n1.addr))->addr), &(n1.addr)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_node(&(n2.addr))->addr), &(n2.addr)));

	remove_node(&(n1.addr));

	UNIT_TEST_ASSERT(find_node(&(n1.addr)) == NULL);
	UNIT_TEST_ASSERT(find_node(&(n2.addr)) != NULL);
	UNIT_TEST_ASSERT(find_node(&(n3.addr)) == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_node(&(n2.addr))->addr), &(n2.addr)));

	remove_node(&(n2.addr));

	UNIT_TEST_ASSERT(find_node(&(n1.addr)) == NULL);
	UNIT_TEST_ASSERT(find_node(&(n2.addr)) == NULL);
	UNIT_TEST_ASSERT(find_node(&(n3.addr)) == NULL);


	node_array = get_all_nodes(&counter);
	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(counter == 0);
	UNIT_TEST_ASSERT(node_array[0] == NULL);

	UNIT_TEST_END();
}

/**
 * Test find_edge after adding and removing some edges
 * Tests the functions:
 * p_edge_t *find_edge(const rimeaddr_t *src, const rimeaddr_t *dst)
 * void add_edge(const p_edge_t edge)
 * void remove_edge(const rimeaddr_t *src, const rimeaddr_t *dst)
 * p_edge_t **get_all_edges(uint8_t * count)
 * uint8_t get_edge_count()
 */
UNIT_TEST(find_edges)
{
	p_node_t n1, n2, n3, n4, n7;
	p_edge_t e1, e2, e3, e4, e5;
	p_edge_t **edge_array;
	uint8_t counter;

	UNIT_TEST_BEGIN();

	e1.dst = rimeaddr_null;
	e1.src = rimeaddr_null;
	e2.dst = rimeaddr_null;
	e2.src = rimeaddr_null;
	e3.dst = rimeaddr_null;
	e3.src = rimeaddr_null;
	e4.dst = rimeaddr_null;
	e4.src = rimeaddr_null;
	e5.dst = rimeaddr_null;
	e5.src = rimeaddr_null;
	e1.dst.u8[0] = 0x01;
	e1.src.u8[0] = 0x02;
	e2.dst.u8[0] = 0x02;
	e2.src.u8[0] = 0x03;
	e3.dst.u8[0] = 0x04;
	e3.src.u8[0] = 0x07;
	e4.dst.u8[0] = 0x02;
	e4.src.u8[0] = 0x01;
	e5.dst.u8[0] = 0x01;
	e5.src.u8[0] = 0x03;
	e1.ttl = 0;
	e2.ttl = 0;
	e3.ttl = 0;
	e4.ttl = 0;
	e5.ttl = 0;
	edge_array = get_all_edges(&counter);
	n1.addr = e1.dst;
	n2.addr = e1.src;
	n3.addr = e2.src;
	n4.addr = e3.dst;
	n7.addr = e3.src;

	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(counter == 0);
	UNIT_TEST_ASSERT(edge_array[0] == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e1.src), &(e1.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e2.src), &(e2.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e3.src), &(e3.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e4.src), &(e4.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e5.src), &(e5.dst)) == NULL);

	add_node(n1);
	add_node(n2);
	add_node(n3);
	add_node(n4);
	add_node(n7);

	add_edge(e1);

	UNIT_TEST_ASSERT(find_edge(&(e1.src), &(e1.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e2.src), &(e2.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e3.src), &(e3.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e4.src), &(e4.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e5.src), &(e5.dst)) == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->src), &(e1.src)) && rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->dst), &(e1.dst)));

	add_edge(e2);

	UNIT_TEST_ASSERT(find_edge(&(e1.src), &(e1.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e2.src), &(e2.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e3.src), &(e3.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e4.src), &(e4.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e5.src), &(e5.dst)) == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->src), &(e1.src)) && rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->dst), &(e1.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->src), &(e2.src)) && rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->dst), &(e2.dst)));

	add_edge(e3);

	UNIT_TEST_ASSERT(find_edge(&(e1.src), &(e1.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e2.src), &(e2.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e3.src), &(e3.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e4.src), &(e4.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e5.src), &(e5.dst)) == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->src), &(e1.src)) && rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->dst), &(e1.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->src), &(e2.src)) && rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e3.src), &(e3.dst))->src), &(e3.src)) && rimeaddr_cmp(&(find_edge(&(e3.src), &(e3.dst))->dst), &(e3.dst)));

	add_edge(e4);

	UNIT_TEST_ASSERT(find_edge(&(e1.src), &(e1.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e2.src), &(e2.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e3.src), &(e3.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e4.src), &(e4.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e5.src), &(e5.dst)) == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->src), &(e1.src)) && rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->dst), &(e1.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->src), &(e2.src)) && rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e3.src), &(e3.dst))->src), &(e3.src)) && rimeaddr_cmp(&(find_edge(&(e3.src), &(e3.dst))->dst), &(e3.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e4.src), &(e4.dst))->src), &(e4.src)) && rimeaddr_cmp(&(find_edge(&(e4.src), &(e4.dst))->dst), &(e4.dst)));

	add_edge(e5);

	UNIT_TEST_ASSERT(find_edge(&(e1.src), &(e1.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e2.src), &(e2.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e3.src), &(e3.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e4.src), &(e4.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e5.src), &(e5.dst)) != NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->src), &(e1.src)) && rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->dst), &(e1.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->src), &(e2.src)) && rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e3.src), &(e3.dst))->src), &(e3.src)) && rimeaddr_cmp(&(find_edge(&(e3.src), &(e3.dst))->dst), &(e3.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e4.src), &(e4.dst))->src), &(e4.src)) && rimeaddr_cmp(&(find_edge(&(e4.src), &(e4.dst))->dst), &(e4.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e5.src), &(e5.dst))->src), &(e5.src)) && rimeaddr_cmp(&(find_edge(&(e5.src), &(e5.dst))->dst), &(e5.dst)));


	remove_edge(&(e5.src), &(e5.dst));

	UNIT_TEST_ASSERT(find_edge(&(e1.src), &(e1.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e2.src), &(e2.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e3.src), &(e3.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e4.src), &(e4.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e5.src), &(e5.dst)) == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->src), &(e1.src)) && rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->dst), &(e1.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->src), &(e2.src)) && rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e3.src), &(e3.dst))->src), &(e3.src)) && rimeaddr_cmp(&(find_edge(&(e3.src), &(e3.dst))->dst), &(e3.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e4.src), &(e4.dst))->src), &(e4.src)) && rimeaddr_cmp(&(find_edge(&(e4.src), &(e4.dst))->dst), &(e4.dst)));

	remove_edge(&(e3.src), &(e3.dst));

	UNIT_TEST_ASSERT(find_edge(&(e1.src), &(e1.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e2.src), &(e2.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e3.src), &(e3.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e4.src), &(e4.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e5.src), &(e5.dst)) == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->src), &(e1.src)) && rimeaddr_cmp(&(find_edge(&(e1.src), &(e1.dst))->dst), &(e1.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->src), &(e2.src)) && rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e4.src), &(e4.dst))->src), &(e4.src)) && rimeaddr_cmp(&(find_edge(&(e4.src), &(e4.dst))->dst), &(e4.dst)));

	remove_edge(&(e1.src), &(e1.dst));

	UNIT_TEST_ASSERT(find_edge(&(e1.src), &(e1.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e2.src), &(e2.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e3.src), &(e3.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e4.src), &(e4.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e5.src), &(e5.dst)) == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->src), &(e2.src)) && rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->dst), &(e2.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e4.src), &(e4.dst))->src), &(e4.src)) && rimeaddr_cmp(&(find_edge(&(e4.src), &(e4.dst))->dst), &(e4.dst)));

	remove_edge(&(e4.src), &(e4.dst));

	UNIT_TEST_ASSERT(find_edge(&(e1.src), &(e1.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e2.src), &(e2.dst)) != NULL);
	UNIT_TEST_ASSERT(find_edge(&(e3.src), &(e3.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e4.src), &(e4.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e5.src), &(e5.dst)) == NULL);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->src), &(e2.src)) && rimeaddr_cmp(&(find_edge(&(e2.src), &(e2.dst))->dst), &(e2.dst)));

	remove_edge(&(e2.src), &(e2.dst));

	UNIT_TEST_ASSERT(find_edge(&(e1.src), &(e1.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e2.src), &(e2.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e3.src), &(e3.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e4.src), &(e4.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e5.src), &(e5.dst)) == NULL);

	remove_node(&(n1.addr));
	remove_node(&(n2.addr));
	remove_node(&(n3.addr));
	remove_node(&(n4.addr));
	remove_node(&(n7.addr));

	edge_array = get_all_edges(&counter);
	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(counter == 0);
	UNIT_TEST_ASSERT(edge_array[0] == NULL);

	UNIT_TEST_END();
}

/**
 * Test incoming and outgoing edges of a node
 * Tests the functions:
 * p_edge_t **get_outgoing_edges(const rimeaddr_t *src_addr, uint8_t *count)
 * p_edge_t **get_ingoing_edges(const rimeaddr_t *dst_addr, uint8_t *count)
 * void add_node(const p_node_t node)
 * void remove_node(const rimeaddr_t *addr)
 * p_node_t **get_all_nodes(uint8_t * count)
 * uint8_t get_node_count()
 */
UNIT_TEST(in_out_edges)
{
	p_node_t n1, n2, n3, n4, n5;
	p_node_t **node_array;
	p_edge_t e11, e12, e13, e14, e21, e23, e31, e32, e41;
	p_edge_t **edge_array;
	p_edge_t **in_edge_array;
	p_edge_t **out_edge_array;
	uint8_t counter1, counter2;

	UNIT_TEST_BEGIN();

	n1.addr = rimeaddr_null;
	n2.addr = rimeaddr_null;
	n3.addr = rimeaddr_null;
	n4.addr = rimeaddr_null;
	n5.addr = rimeaddr_null;
	n1.addr.u8[0] = 0x01;
	n2.addr.u8[0] = 0x02;
	n3.addr.u8[0] = 0x03;
	n4.addr.u8[0] = 0x04;
	n5.addr.u8[0] = 0x05;
	e11.dst = rimeaddr_null;
	e11.src = rimeaddr_null;
	e12.dst = rimeaddr_null;
	e12.src = rimeaddr_null;
	e13.dst = rimeaddr_null;
	e13.src = rimeaddr_null;
	e14.dst = rimeaddr_null;
	e14.src = rimeaddr_null;
	e21.dst = rimeaddr_null;
	e21.src = rimeaddr_null;
	e23.dst = rimeaddr_null;
	e23.src = rimeaddr_null;
	e31.dst = rimeaddr_null;
	e31.src = rimeaddr_null;
	e32.dst = rimeaddr_null;
	e32.src = rimeaddr_null;
	e41.dst = rimeaddr_null;
	e41.src = rimeaddr_null;
	e11.src.u8[0] = 0x01;
	e11.dst.u8[0] = 0x01;
	e12.src.u8[0] = 0x01;
	e12.dst.u8[0] = 0x02;
	e13.src.u8[0] = 0x01;
	e13.dst.u8[0] = 0x03;
	e14.src.u8[0] = 0x01;
	e14.dst.u8[0] = 0x04;
	e21.src.u8[0] = 0x02;
	e21.dst.u8[0] = 0x01;
	e23.src.u8[0] = 0x02;
	e23.dst.u8[0] = 0x03;
	e31.src.u8[0] = 0x03;
	e31.dst.u8[0] = 0x01;
	e32.src.u8[0] = 0x03;
	e32.dst.u8[0] = 0x02;
	e41.src.u8[0] = 0x04;
	e41.dst.u8[0] = 0x01;
	e11.ttl = 0;
	e12.ttl = 0;
	e13.ttl = 0;
	e14.ttl = 0;
	e21.ttl = 0;
	e23.ttl = 0;
	e31.ttl = 0;
	e32.ttl = 0;
	e41.ttl = 0;
	node_array = get_all_nodes(&counter1);
	edge_array = get_all_edges(&counter2);

	/**********************************************/
	/***Check that everything initialized with 0***/
	/**********************************************/
	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(counter1 == 0);
	UNIT_TEST_ASSERT(counter2 == 0);
	UNIT_TEST_ASSERT(node_array[0] == NULL);
	UNIT_TEST_ASSERT(edge_array[0] == NULL);

	in_edge_array = get_ingoing_edges(&(n1.addr), &counter1);
	out_edge_array = get_outgoing_edges(&(n1.addr), &counter2);
	UNIT_TEST_ASSERT(counter1 == 0);
	UNIT_TEST_ASSERT(counter2 == 0);
	UNIT_TEST_ASSERT(in_edge_array == NULL);
	UNIT_TEST_ASSERT(out_edge_array == NULL);

	in_edge_array = get_ingoing_edges(&(n2.addr), &counter1);
	out_edge_array = get_outgoing_edges(&(n2.addr), &counter2);
	UNIT_TEST_ASSERT(counter1 == 0);
	UNIT_TEST_ASSERT(counter2 == 0);
	UNIT_TEST_ASSERT(in_edge_array == NULL);
	UNIT_TEST_ASSERT(out_edge_array == NULL);

	in_edge_array = get_ingoing_edges(&(n3.addr), &counter1);
	out_edge_array = get_outgoing_edges(&(n3.addr), &counter2);
	UNIT_TEST_ASSERT(counter1 == 0);
	UNIT_TEST_ASSERT(counter2 == 0);
	UNIT_TEST_ASSERT(in_edge_array == NULL);
	UNIT_TEST_ASSERT(out_edge_array == NULL);

	in_edge_array = get_ingoing_edges(&(n4.addr), &counter1);
	out_edge_array = get_outgoing_edges(&(n4.addr), &counter2);
	UNIT_TEST_ASSERT(counter1 == 0);
	UNIT_TEST_ASSERT(counter2 == 0);
	UNIT_TEST_ASSERT(in_edge_array == NULL);
	UNIT_TEST_ASSERT(out_edge_array == NULL);

	in_edge_array = get_ingoing_edges(&(n5.addr), &counter1);
	out_edge_array = get_outgoing_edges(&(n5.addr), &counter2);
	UNIT_TEST_ASSERT(counter1 == 0);
	UNIT_TEST_ASSERT(counter2 == 0);
	UNIT_TEST_ASSERT(in_edge_array == NULL);
	UNIT_TEST_ASSERT(out_edge_array == NULL);

	/**********************************************/
	/**Add edges and test if the change is correct*/
	/**********************************************/

	add_node(n1);
	add_node(n2);
	add_node(n3);
	add_node(n4);
	add_node(n5);

	add_edge(e11);
	add_edge(e12);
	add_edge(e13);
	add_edge(e14);
	add_edge(e21);
	add_edge(e23);
	add_edge(e31);
	add_edge(e32);
	add_edge(e41);

	in_edge_array = get_ingoing_edges(&(n1.addr), &counter1);
	out_edge_array = get_outgoing_edges(&(n1.addr), &counter2);
	UNIT_TEST_ASSERT(counter1 == 4);
	UNIT_TEST_ASSERT(counter2 == 4);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(in_edge_array[0]->src), &(e11.src)) && rimeaddr_cmp(&(in_edge_array[0]->dst), &(e11.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(in_edge_array[1]->src), &(e21.src)) && rimeaddr_cmp(&(in_edge_array[1]->dst), &(e21.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(in_edge_array[2]->src), &(e31.src)) && rimeaddr_cmp(&(in_edge_array[2]->dst), &(e31.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(in_edge_array[3]->src), &(e41.src)) && rimeaddr_cmp(&(in_edge_array[3]->dst), &(e41.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(out_edge_array[0]->src), &(e11.src)) && rimeaddr_cmp(&(out_edge_array[0]->dst), &(e11.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(out_edge_array[1]->src), &(e12.src)) && rimeaddr_cmp(&(out_edge_array[1]->dst), &(e12.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(out_edge_array[2]->src), &(e13.src)) && rimeaddr_cmp(&(out_edge_array[2]->dst), &(e13.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(out_edge_array[3]->src), &(e14.src)) && rimeaddr_cmp(&(out_edge_array[3]->dst), &(e14.dst)));
	free(in_edge_array);
	free(out_edge_array);

	in_edge_array = get_ingoing_edges(&(n2.addr), &counter1);
	out_edge_array = get_outgoing_edges(&(n2.addr), &counter2);
	UNIT_TEST_ASSERT(counter1 == 2);
	UNIT_TEST_ASSERT(counter2 == 2);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(in_edge_array[0]->src), &(e12.src)) && rimeaddr_cmp(&(in_edge_array[0]->dst), &(e12.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(in_edge_array[1]->src), &(e32.src)) && rimeaddr_cmp(&(in_edge_array[1]->dst), &(e32.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(out_edge_array[0]->src), &(e21.src)) && rimeaddr_cmp(&(out_edge_array[0]->dst), &(e21.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(out_edge_array[1]->src), &(e23.src)) && rimeaddr_cmp(&(out_edge_array[1]->dst), &(e23.dst)));
	free(in_edge_array);
	free(out_edge_array);

	in_edge_array = get_ingoing_edges(&(n3.addr), &counter1);
	out_edge_array = get_outgoing_edges(&(n3.addr), &counter2);
	UNIT_TEST_ASSERT(counter1 == 2);
	UNIT_TEST_ASSERT(counter2 == 2);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(in_edge_array[0]->src), &(e13.src)) && rimeaddr_cmp(&(in_edge_array[0]->dst), &(e13.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(in_edge_array[1]->src), &(e23.src)) && rimeaddr_cmp(&(in_edge_array[1]->dst), &(e23.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(out_edge_array[0]->src), &(e31.src)) && rimeaddr_cmp(&(out_edge_array[0]->dst), &(e31.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(out_edge_array[1]->src), &(e32.src)) && rimeaddr_cmp(&(out_edge_array[1]->dst), &(e32.dst)));
	free(in_edge_array);
	free(out_edge_array);

	in_edge_array = get_ingoing_edges(&(n4.addr), &counter1);
	out_edge_array = get_outgoing_edges(&(n4.addr), &counter2);
	UNIT_TEST_ASSERT(counter1 == 1);
	UNIT_TEST_ASSERT(counter2 == 1);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(in_edge_array[0]->src), &(e14.src)) && rimeaddr_cmp(&(in_edge_array[0]->dst), &(e14.dst)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(out_edge_array[0]->src), &(e41.src)) && rimeaddr_cmp(&(out_edge_array[0]->dst), &(e41.dst)));
	free(in_edge_array);
	free(out_edge_array);

	in_edge_array = get_ingoing_edges(&(n5.addr), &counter1);
	out_edge_array = get_outgoing_edges(&(n5.addr), &counter2);
	UNIT_TEST_ASSERT(counter1 == 0);
	UNIT_TEST_ASSERT(counter2 == 0);
	UNIT_TEST_ASSERT(in_edge_array == NULL);
	UNIT_TEST_ASSERT(out_edge_array == NULL);

	/**********************************************/
	/*******************Cleanup********************/
	/**********************************************/

	remove_edge(&(e11.src), &(e11.dst));
	remove_edge(&(e12.src), &(e12.dst));
	remove_edge(&(e13.src), &(e13.dst));
	remove_edge(&(e14.src), &(e14.dst));
	remove_edge(&(e21.src), &(e21.dst));
	remove_edge(&(e23.src), &(e23.dst));
	remove_edge(&(e31.src), &(e31.dst));
	remove_edge(&(e32.src), &(e32.dst));
	remove_edge(&(e41.src), &(e41.dst));
	
	remove_node(&(n1.addr));
	remove_node(&(n2.addr));
	remove_node(&(n3.addr));
	remove_node(&(n4.addr));
	remove_node(&(n5.addr));

	/**********************************************/
	/****Check if the cleanup worked correctly*****/
	/**********************************************/
	node_array = get_all_nodes(&counter1);
	edge_array = get_all_edges(&counter2);
	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(counter1 == 0);
	UNIT_TEST_ASSERT(counter2 == 0);
	UNIT_TEST_ASSERT(node_array[0] == NULL);
	UNIT_TEST_ASSERT(edge_array[0] == NULL);

	UNIT_TEST_END();
}

/**
 * Test if functions return without doing anything if a parameter is NULL
 * Tests the functions:
 * void add_node(const p_node_t node)
 * void remove_node(const rimeaddr_t *addr)
 * p_node_t **get_all_nodes(uint8_t * count)
 * uint8_t get_node_count()
 * p_node_t *find_node(const rimeaddr_t *addr)
 * void add_edge(const p_edge_t edge)
 * void remove_edge(const rimeaddr_t *src, const rimeaddr_t *dst)
 * p_edge_t **get_all_edges(uint8_t *count)
 * p_edge_t **get_outgoing_edges(const rimeaddr_t *src_addr, uint8_t *count)
 * p_edge_t **get_ingoing_edges(const rimeaddr_t *dst_addr, uint8_t *count)
 * p_edge_t *find_edge(const rimeaddr_t *src, const rimeaddr_t *dst)
 * uint8_t get_edge_count()
 */
UNIT_TEST(test_NULL_param)
{
	p_node_t n1, n2;
	p_edge_t e1;
	p_node_t **node_array;
	p_edge_t **edge_array;
	uint8_t counter1, counter2;

	UNIT_TEST_BEGIN();

	n1.addr = rimeaddr_null;
	n1.addr.u8[0] = 0x01;
	e1.dst = rimeaddr_null;
	e1.src = rimeaddr_null;
	e1.dst.u8[0] = 0x01;
	e1.src.u8[0] = 0x02;
	e1.ttl = 0;
	n2.addr = rimeaddr_null;
	n2.addr.u8[0] = 0x02;
	node_array = get_all_nodes(&counter1);
	edge_array = get_all_edges(&counter2);

	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(counter1 == 0);
	UNIT_TEST_ASSERT(counter2 == 0);
	UNIT_TEST_ASSERT(node_array[0] == NULL);
	UNIT_TEST_ASSERT(edge_array[0] == NULL);

	add_node(n1);
	add_node(n2);
	add_edge(e1);
	node_array = get_all_nodes(&counter1);
	edge_array = get_all_edges(&counter2);

	UNIT_TEST_ASSERT(get_node_count() == 2);
	UNIT_TEST_ASSERT(get_edge_count() == 1);
	UNIT_TEST_ASSERT(counter1 == 2);
	UNIT_TEST_ASSERT(counter2 == 1);
	UNIT_TEST_ASSERT(node_array[0] != NULL);
	UNIT_TEST_ASSERT(node_array[1] != NULL);
	UNIT_TEST_ASSERT(node_array[2] == NULL);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] == NULL);

	//Test functions with NULL as param
	UNIT_TEST_ASSERT(get_all_nodes(NULL) == NULL);
	UNIT_TEST_ASSERT(find_node(NULL) == NULL);
	UNIT_TEST_ASSERT(get_all_edges(NULL) == NULL);
	UNIT_TEST_ASSERT(get_ingoing_edges(NULL, &counter2) == NULL);
	UNIT_TEST_ASSERT(get_ingoing_edges(&(e1.dst), NULL) == NULL);
	UNIT_TEST_ASSERT(get_ingoing_edges(NULL, NULL) == NULL);
	UNIT_TEST_ASSERT(get_outgoing_edges(NULL, &counter2) == NULL);
	UNIT_TEST_ASSERT(get_outgoing_edges(&(e1.src), NULL) == NULL);
	UNIT_TEST_ASSERT(get_outgoing_edges(NULL, NULL) == NULL);
	UNIT_TEST_ASSERT(find_edge(NULL, &(e1.dst)) == NULL);
	UNIT_TEST_ASSERT(find_edge(&(e1.src), NULL) == NULL);
	UNIT_TEST_ASSERT(find_edge(NULL, NULL) == NULL);

	remove_node(NULL);
	remove_edge(&(e1.src), NULL);
	remove_edge(NULL, &(e1.dst));
	remove_edge(NULL, NULL);
	node_array = get_all_nodes(&counter1);
	edge_array = get_all_edges(&counter2);
	UNIT_TEST_ASSERT(get_node_count() == 2);
	UNIT_TEST_ASSERT(get_edge_count() == 1);
	UNIT_TEST_ASSERT(counter1 == 2);
	UNIT_TEST_ASSERT(counter2 == 1);
	UNIT_TEST_ASSERT(node_array[0] != NULL);
	UNIT_TEST_ASSERT(node_array[1] != NULL);
	UNIT_TEST_ASSERT(node_array[2] == NULL);
	UNIT_TEST_ASSERT(edge_array[0] != NULL);
	UNIT_TEST_ASSERT(edge_array[1] == NULL);

	//cleanup
	remove_node(&(n1.addr));
	remove_node(&(n2.addr));
	remove_edge(&(e1.src), &(e1.dst));
	node_array = get_all_nodes(&counter1);
	edge_array = get_all_edges(&counter2);

	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(counter1 == 0);
	UNIT_TEST_ASSERT(counter2 == 0);
	UNIT_TEST_ASSERT(node_array[0] == NULL);
	UNIT_TEST_ASSERT(edge_array[0] == NULL);

	UNIT_TEST_END();
}

/**
 * Test if all members of the node and edge struct are saved correctly
 * Tests the functions:
 * void add_node(const p_node_t node)
 * void remove_node(const rimeaddr_t *addr)
 * p_node_t *find_node(const rimeaddr_t *addr)
 * void add_edge(const p_edge_t edge)
 * void remove_edge(const rimeaddr_t *src, const rimeaddr_t *dst)
 * p_edge_t *find_edge(const rimeaddr_t *src, const rimeaddr_t *dst)
 */
UNIT_TEST(test_members)
{
	position_t pos = {0, 0};
	p_node_t n1 = {rimeaddr_null, pos};
	p_node_t n2 = {rimeaddr_null, pos};
	n2.addr.u8[0] = 2;
	p_edge_t e1 = {rimeaddr_null, n2.addr, 0, 0, 0};

	UNIT_TEST_BEGIN();

	//If new members are added, modify this test
	UNIT_TEST_ASSERT(sizeof(p_node_t) == 6);
	UNIT_TEST_ASSERT(sizeof(p_edge_t) == 10);

	add_node(n1);
	add_node(n2);
	add_edge(e1);

	p_node_t *node = find_node(&(n1.addr));
	UNIT_TEST_ASSERT(node->pos.x == pos.x);
	UNIT_TEST_ASSERT(node->pos.y == pos.y);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(node->addr), &rimeaddr_null));

	p_edge_t *edge = find_edge(&(e1.src), &(e1.dst));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge->src), &rimeaddr_null));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge->dst), &(n2.addr)));
	UNIT_TEST_ASSERT(edge->ttl == 0);
	UNIT_TEST_ASSERT(edge->rssi == 0);
	UNIT_TEST_ASSERT(edge->lqi == 0);

	pos.x = 1;
	pos.y = 1;
	n1.addr.u8[0] = 1;
	n1.pos = pos;

	e1.src.u8[0] = 1;
	e1.dst.u8[0] = 2;
	e1.ttl = 3;
	e1.rssi = 4;
	e1.lqi = 5;

	add_node(n1);
	add_edge(e1);

	node = find_node(&(n1.addr));
	UNIT_TEST_ASSERT(node->pos.x == pos.x);
	UNIT_TEST_ASSERT(node->pos.y == pos.y);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(node->addr), &(n1.addr)));

	edge = find_edge(&(e1.src), &(e1.dst));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge->src), &(e1.src)));
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge->dst), &(e1.dst)));
	UNIT_TEST_ASSERT(edge->ttl == 3);
	UNIT_TEST_ASSERT(edge->rssi == 4);
	UNIT_TEST_ASSERT(edge->lqi == 5);

	remove_node(&(node->addr));
	remove_node(&(n2.addr));
	remove_edge(&(edge->src), &(edge->dst));

	UNIT_TEST_END();
}

int main()
{
	init_graph();
	UNIT_TEST_RUN(add_nodes);
	UNIT_TEST_RUN(add_edges);
	UNIT_TEST_RUN(find_nodes);
	UNIT_TEST_RUN(find_edges);
	UNIT_TEST_RUN(in_out_edges);
	UNIT_TEST_RUN(test_NULL_param);
	UNIT_TEST_RUN(test_members);
	return EXIT_SUCCESS;
}
