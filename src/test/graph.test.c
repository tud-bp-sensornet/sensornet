#include "contiki.h"
#include "unit-test.h"

#include "graph.h"

/* Register example unit tests that will be executed by using
   the UNIT_TEST_RUN macro. */
UNIT_TEST_REGISTER(add_nodes, "Test adding and removing nodes");
UNIT_TEST_REGISTER(add_edges, "Test adding and removing edges");

/**
 * Test adding and removing some nodes
 */
UNIT_TEST(add_nodes)
{
	p_node_t n1, n2, n3;
	p_node_t** node_array;
	uint8_t counter;

	UNIT_TEST_BEGIN();

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


	UNIT_TEST_END();
}

/**
 * Test adding and removing some edges
 */
UNIT_TEST(add_edges)
{
	p_edge_t e1, e2, e3, e4, e5, e6;
	p_edge_t** edge_array;
	uint8_t counter;

	UNIT_TEST_BEGIN();

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
	edge_array = get_all_edges(&counter);

	UNIT_TEST_ASSERT(get_node_count() == 0);
	UNIT_TEST_ASSERT(get_edge_count() == 0);
	UNIT_TEST_ASSERT(counter == 0);
	UNIT_TEST_ASSERT(edge_array[0] == NULL);

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
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(edge_array[0]->src), &(e1.src)) && rimeaddr_cmp(&(edge_array[0]->dst), &(e1.dst)));
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

	UNIT_TEST_END();
}

void main() {
	init_graph();
	UNIT_TEST_RUN(add_nodes);
	UNIT_TEST_RUN(add_edges);
}
