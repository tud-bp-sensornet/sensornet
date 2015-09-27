#include "unit-test.h"
#include "contiki.h"

#include <stdlib.h>

#include "graph.h"
#include "routing.h"
#include "positions.h"

linkaddr_t node_destination = {{0x04}};

/**
 * Register unit tests that will be executed by using
 * the UNIT_TEST_RUN macro.
 */
UNIT_TEST_REGISTER(no_edge_test, "No edge test");
UNIT_TEST_REGISTER(find_nearest_test, "Find nearest test");

/**
 * Test get_nearest_neighbour with no edges outgoing from root
 * Tests the functions:
 * linkaddr_t get_nearest_neighbour();
 */
UNIT_TEST(no_edge_test)
{

	p_node_t r;
	r.addr = linkaddr_null;
	r.addr.u8[0] = 0x01;
	linkaddr_set_node_addr(&(r.addr));
	add_node(r);

	UNIT_TEST_BEGIN();

	linkaddr_t addr = get_nearest_neighbour(&node_destination);

	UNIT_TEST_ASSERT(linkaddr_cmp(&addr, &linkaddr_null));

	UNIT_TEST_END();

	remove_node(&(r.addr));

}

/**
 * Test get_nearest_neighbour with positions and edges
 *
 *       n3
 *       |
 * n2 -> r <-> n1
 *
 * Tests the functions:
 * linkaddr_t get_nearest_neighbour();
 */
UNIT_TEST(find_nearest_test)
{

	p_node_t r, n1, n2, n3;
	r.addr = linkaddr_null;
	n1.addr = linkaddr_null;
	n2.addr = linkaddr_null;
	n3.addr = linkaddr_null;
	r.addr.u8[0] = 0x02;
	n1.addr.u8[0] = 0x03;
	n2.addr.u8[0] = 0x04;
	n3.addr.u8[0] = 0x06;
	r.pos = get_stored_position_of(&(r.addr));
	n1.pos = get_stored_position_of(&(n1.addr));
	n2.pos = get_stored_position_of(&(n2.addr));
	n3.pos = get_stored_position_of(&(n3.addr));
	linkaddr_set_node_addr(&(r.addr));
	add_node(r);
	add_node(n1);
	add_node(n2);
	add_node(n3);

	p_edge_t e2r = {n2.addr, r.addr, 0x00};
	p_edge_t er3 = {r.addr, n3.addr, 0x00};
	p_edge_t e1r = {n1.addr, r.addr, 0x00};
	p_edge_t er1 = {r.addr, n1.addr, 0x00};

	add_edge(e2r);
	add_edge(er3);
	add_edge(e1r);
	add_edge(er1);

	UNIT_TEST_BEGIN();

	linkaddr_t addr = get_nearest_neighbour(&node_destination);
	
	UNIT_TEST_ASSERT(linkaddr_cmp(&addr, &(n1.addr)));

	//Add edge r -> n2
	p_edge_t er2 = {r.addr, n2.addr, 0x00};
	add_edge(er2);

	addr = get_nearest_neighbour(&node_destination);

	UNIT_TEST_ASSERT(linkaddr_cmp(&addr, &(n2.addr)));

	UNIT_TEST_END();

	remove_node(&(r.addr));
	remove_node(&(n1.addr));
	remove_node(&(n2.addr));
	remove_node(&(n3.addr));

	remove_edge(&(n1.addr), &(r.addr));
	remove_edge(&(n2.addr), &(r.addr));
	remove_edge(&(r.addr), &(n3.addr));
	remove_edge(&(r.addr), &(n1.addr));
	remove_edge(&(r.addr), &(n2.addr));

}

int main()
{
	init_graph();

	UNIT_TEST_RUN(no_edge_test);
	UNIT_TEST_RUN(find_nearest_test);

	return EXIT_SUCCESS;
}

