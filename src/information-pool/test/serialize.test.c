#include "contiki.h"
#include "unit-test.h"
#include "serialize.h"
#include "graph.h"
#include <stdlib.h>
#include <stdio.h>

UNIT_TEST_REGISTER(seriDeseri_root, "Serialize and deserialize only root");
UNIT_TEST_REGISTER(seriDeseri_0_hop, "Serialize and deserialize tree 0 hop");
UNIT_TEST_REGISTER(seriDeseri_1_hop, "Serialize and deserialize tree 1 hop");
UNIT_TEST_REGISTER(seriDeseri_2_hop, "Serialize and deserialize tree 2 hop");
UNIT_TEST_REGISTER(seriDeseri_3_hop, "Serialize and deserialize tree 3 hop");
UNIT_TEST_REGISTER(seriDeseri_cyclical_2_hop, "Serialize and deserialize cyclical tree 2 hop");
UNIT_TEST_REGISTER(seriDeseri_big_cyclical_2_hop, "Serialize and deserialize big cyclical tree 2 hop");

/**
 * only the root without edges is initialized and crawls a depth of 99
 */
UNIT_TEST(seriDeseri_root)
{
	p_graph_t *graph;
	p_node_t* root;
	size_t length;

	UNIT_TEST_BEGIN();

	root = malloc(sizeof *root);
	root->addr.u8[0] = 0x01;
	root->edges = NULL;

	graph = malloc(sizeof *graph);
	graph->root = root;
	graph->num_edges = 0;
	graph->num_nodes = 1;

	void* serialized = serialize(graph, 99, &length);
	p_node_t* deserialized = deserialize(serialized);

	UNIT_TEST_ASSERT(rimeaddr_cmp(&(root->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(root->edges == deserialized->edges);
	UNIT_TEST_ASSERT(length == sizeof(p_node_t));

	UNIT_TEST_END();
}

/**
 * Creates an example graph used for testing.
 */
p_graph_t* test_graph()
{
	p_graph_t *g;
	p_node_t *n1, *n2, *n3, *n4;
	p_edge_t *e1, *e2, *e3;

	g = malloc(sizeof *g);

	n1 = malloc(sizeof *n1);
	n2 = malloc(sizeof *n2);
	n3 = malloc(sizeof *n3);
	n4 = malloc(sizeof *n4);

	e1 = malloc(sizeof *e1);
	e2 = malloc(sizeof *e2);
	e3 = malloc(sizeof *e3);

	// Give nodes IDs
	n1->addr.u8[0] = 0x01;
	n2->addr.u8[0] = 0x02;
	n3->addr.u8[0] = 0x03;
	n4->addr.u8[0] = 0x04;

	// Build Graph, root = n1
	// n1->n2, n1->n3, n2->n4
	n1->edges = e1;
	e1->next = e2;
	e2->next = NULL;
	n2->edges = e3;
	e3->next = NULL;
	e1->drain = n2;
	e2->drain = n3;
	e3->drain = n4;
	n3->edges = NULL;
	n4->edges = NULL;

	g->root = n1;
	g->num_nodes = 4;
	g->num_edges = 3;

	return g;
}

/**
 * initialize the graph
 * n1->n2,n1->n3,n2->n4
 * and then serialize and deserialize with k = 0
 */
UNIT_TEST(seriDeseri_0_hop)
{
	p_graph_t* testGraph = test_graph();
	size_t length;

	UNIT_TEST_BEGIN();

	void* serialized = serialize(testGraph, 0, &length);
	p_node_t* deserialized = deserialize(serialized);

	//Assert correct deserialisation
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(deserialized->edges == NULL);

	UNIT_TEST_ASSERT(length == sizeof(p_node_t));

	UNIT_TEST_END();
}

/**
 * initialize the graph
 * n1->n2,n1->n3,n2->n4
 * and then serialize and deserialize with k = 1
 */
UNIT_TEST(seriDeseri_1_hop)
{
	p_graph_t* testGraph = test_graph();
	size_t length;

	UNIT_TEST_BEGIN();

	void* serialized = serialize(testGraph, 1, &length);
	p_node_t* deserialized = deserialize(serialized);

	//Assert correct deserialisation
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->edges->drain->addr), &(deserialized->edges->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->edges->next->drain->addr), &(deserialized->edges->next->drain->addr)) != 0);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->next->drain->edges == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->next->next == NULL);

	UNIT_TEST_ASSERT(length == 3*sizeof(p_node_t) + 2*sizeof(p_edge_t));

	UNIT_TEST_END();
}

/**
 * initialize the graph
 * n1->n2,n1->n3,n2->n4
 * and then serialize and deserialize with k = 2
 */
UNIT_TEST(seriDeseri_2_hop)
{
	p_graph_t* testGraph = test_graph();
	size_t length;

	UNIT_TEST_BEGIN();

	void* serialized = serialize(testGraph, 2, &length);
	p_node_t* deserialized = deserialize(serialized);

	//Assert correct deserialisation
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->edges->drain->addr), &(deserialized->edges->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->edges->next->drain->addr), &(deserialized->edges->next->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->edges->drain->edges->drain->addr), &(deserialized->edges->drain->edges->drain->addr)) != 0);
	UNIT_TEST_ASSERT(deserialized->edges->next->drain->edges == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->next->next == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges->next == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges->drain->edges == NULL);

	UNIT_TEST_ASSERT(length == 4*sizeof(p_node_t) + 3*sizeof(p_edge_t));

	UNIT_TEST_END();
}

/**
 * initialize the graph
 * n1->n2,n1->n3,n2->n4
 * and then serialize and deserialize with k = 3
 */
UNIT_TEST(seriDeseri_3_hop)
{
	p_graph_t* testGraph = test_graph();
	size_t length;

	UNIT_TEST_BEGIN();

	void* serialized = serialize(testGraph, 3, &length);
	p_node_t* deserialized = deserialize(serialized);

	//Assert correct deserialisation
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->edges->drain->addr), &(deserialized->edges->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->edges->next->drain->addr), &(deserialized->edges->next->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->edges->drain->edges->drain->addr), &(deserialized->edges->drain->edges->drain->addr)) != 0);
	UNIT_TEST_ASSERT(deserialized->edges->next->drain->edges == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->next->next == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges->next == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges->drain->edges == NULL);

	UNIT_TEST_ASSERT(length == 4*sizeof(p_node_t) + 3*sizeof(p_edge_t));

	UNIT_TEST_END();
}

/**
 * Creates a cyclical example graph used for testing.
 */
p_graph_t* cyclical_test_graph()
{
	p_graph_t *g;
	p_node_t *n1, *n2;
	p_edge_t *e1, *e2;

	g = malloc(sizeof *g);

	n1 = malloc(sizeof *n1);
	n2 = malloc(sizeof *n2);

	e1 = malloc(sizeof *e1);
	e2 = malloc(sizeof *e2);

	// Give nodes IDs
	n1->addr.u8[0] = 0x01;
	n2->addr.u8[0] = 0x02;

	// Build Graph, root = n1
	// n1->n2, n2->n1
	n1->edges = e1;
	e1->drain = n2;
	e1->next = NULL;
	n2->edges = e2;
	e2->drain = n1;
	e2->next = NULL;

	g->root = n1;
	g->num_nodes = 2;
	g->num_edges = 2;

	return g;
}

/**
 * Creates a bigger cyclical example graph used for testing.
 */
p_graph_t* big_cyclical_test_graph()
{
	p_graph_t *g;
	p_node_t *n1, *n2, *n3;
	p_edge_t *e1, *e2, *e3, *e4, *e5, *e6;

	g = malloc(sizeof *g);

	n1 = malloc(sizeof *n1);
	n2 = malloc(sizeof *n2);
	n3 = malloc(sizeof *n3);

	e1 = malloc(sizeof *e1);
	e2 = malloc(sizeof *e2);
	e3 = malloc(sizeof *e3);
	e4 = malloc(sizeof *e4);
	e5 = malloc(sizeof *e5);
	e6 = malloc(sizeof *e6);

	// Give nodes IDs
	n1->addr.u8[0] = 0x01;
	n2->addr.u8[0] = 0x02;
	n3->addr.u8[0] = 0x03;

	// Build Graph, root = n1
	// n1->n2, n1->n3
	n1->edges = e1;
	e1->drain = n2;
	e1->next = e2;
	e2->drain = n3;
	e2->next = NULL;

	// n2->n1, n2->n3
	n2->edges = e3;
	e3->drain = n1;
	e3->next = e4;
	e4->drain = n3;
	e4->next = NULL;

	// n3->n1, n3->n2
	n3->edges = e5;
	e5->drain = n1;
	e5->next = e6;
	e6->drain = n2;
	e6->next = NULL;

	g->root = n1;
	g->num_nodes = 3;
	g->num_edges = 6;

	return g;
}

UNIT_TEST(seriDeseri_cyclical_2_hop)
{
	p_graph_t* testGraph = cyclical_test_graph();
	size_t length;

	UNIT_TEST_BEGIN();

	void* serialized = serialize(testGraph, 2, &length);

	UNIT_TEST_ASSERT(length == 2*sizeof(p_node_t) + 2*sizeof(p_edge_t));

	p_node_t* deserialized = deserialize(serialized);

	//Assert correct deserialisation
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->edges->drain->addr), &(deserialized->edges->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->edges->drain->edges->drain->addr), &(testGraph->root->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(deserialized->edges->drain->edges->drain->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges->drain == deserialized);
	UNIT_TEST_ASSERT(deserialized->edges->next == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges->next == NULL);

	UNIT_TEST_END();
}

UNIT_TEST(seriDeseri_big_cyclical_2_hop)
{
	p_graph_t* testGraph = big_cyclical_test_graph();
	size_t length;

	UNIT_TEST_BEGIN();

	void* serialized = serialize(testGraph, 2, &length);

	UNIT_TEST_ASSERT(length == 3*sizeof(p_node_t) + 6*sizeof(p_edge_t));

	p_node_t* deserialized = deserialize(serialized);

	//Assert correct deserialisation
	//TODO add more assertions
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->edges->drain->addr), &(deserialized->edges->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->root->edges->next->drain->addr), &(deserialized->edges->next->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(deserialized->edges->drain->edges->drain->addr), &(deserialized->addr)) != 0);

	UNIT_TEST_ASSERT(deserialized->edges->drain->edges->drain == deserialized);
	UNIT_TEST_ASSERT(deserialized->edges->next->next == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges->next->next == NULL);

	UNIT_TEST_END();
}

int main()
{
    UNIT_TEST_RUN(seriDeseri_root);
    UNIT_TEST_RUN(seriDeseri_0_hop);
    UNIT_TEST_RUN(seriDeseri_1_hop);
    UNIT_TEST_RUN(seriDeseri_2_hop);
    UNIT_TEST_RUN(seriDeseri_3_hop);

    UNIT_TEST_RUN(seriDeseri_cyclical_2_hop);
    UNIT_TEST_RUN(seriDeseri_big_cyclical_2_hop);

    return EXIT_SUCCESS;
}
