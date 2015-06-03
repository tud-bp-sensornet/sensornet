#include "contiki.h"
#include "unit-test.h"
#include "serialize.h"
#include "graph.h"
#include <stdlib.h>

UNIT_TEST_REGISTER(seriDeseri_root, "Serialize and deserialize only root");
UNIT_TEST_REGISTER(seriDeseri_0_hop, "Serialize and deserialize tree 0 hop");
UNIT_TEST_REGISTER(seriDeseri_1_hop, "Serialize and deserialize tree 1 hop");
UNIT_TEST_REGISTER(seriDeseri_2_hop, "Serialize and deserialize tree 2 hop");
UNIT_TEST_REGISTER(seriDeseri_3_hop, "Serialize and deserialize tree 3 hop");

/**
 * only the root without edges is initialized and crawls a depth of 99
 */
UNIT_TEST(seriDeseri_root)
{
	p_node_t *root, *deserialized;
	uint16_t countNode, countEdge;

	UNIT_TEST_BEGIN();

	root = malloc(sizeof *root);
	root->addr.u8[0] = 0x01;
	root->last_seen = 0;
	root->edges = NULL;

	void *serialized = serialize(root, 99, 1, 0, &countNode, &countEdge);
	deserialized = deserialize(serialized);

	UNIT_TEST_ASSERT(rimeaddr_cmp(&(root->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(root->edges == deserialized->edges);

	UNIT_TEST_END();
}

/**
 * Creates an example graph used for testing.
 */
p_node_t* test_graph()
{
	p_node_t *n1, *n2, *n3, *n4;
	p_edge_t *e1, *e2, *e3;

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

	return n1;
}

/**
 * initialize the graph
 * n1->n2,n1->n3,n2->n4
 * and then serialize and deserialize with k = 0
 */
UNIT_TEST(seriDeseri_0_hop)
{
	p_node_t* testGraph = test_graph();
	uint16_t maxNodes = 4,  maxEdges = 3;
	uint16_t countNode, countEdge;

	UNIT_TEST_BEGIN();

	void* serialized = serialize(testGraph, 0, maxNodes, maxEdges, &countNode, &countEdge);
	p_node_t* deserialized = deserialize(serialized);

	//Assert correct count
	UNIT_TEST_ASSERT(countNode == 1);
	UNIT_TEST_ASSERT(countEdge == 0);

	//Assert correct deserialisation
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(deserialized->edges == NULL);

	UNIT_TEST_END();
}

/**
 * initialize the graph
 * n1->n2,n1->n3,n2->n4
 * and then serialize and deserialize with k = 1
 */
UNIT_TEST(seriDeseri_1_hop)
{
	p_node_t* testGraph = test_graph();
	uint16_t maxNodes = 4,  maxEdges = 3;
	uint16_t countNode, countEdge;

	UNIT_TEST_BEGIN();

	void* serialized = serialize(testGraph, 1, maxNodes, maxEdges, &countNode, &countEdge);
	p_node_t* deserialized = deserialize(serialized);

	//Assert correct count
	UNIT_TEST_ASSERT(countNode == 3);
	UNIT_TEST_ASSERT(countEdge == 2);

	//Assert correct deserialisation
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->edges->drain->addr), &(deserialized->edges->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->edges->next->drain->addr), &(deserialized->edges->next->drain->addr)) != 0);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->next->drain->edges == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->next->next == NULL);

	UNIT_TEST_END();
}

/**
 * initialize the graph
 * n1->n2,n1->n3,n2->n4
 * and then serialize and deserialize with k = 2
 */
UNIT_TEST(seriDeseri_2_hop)
{
	p_node_t* testGraph = test_graph();
	uint16_t maxNodes = 4,  maxEdges = 3;
	uint16_t countNode, countEdge;

	UNIT_TEST_BEGIN();

	void* serialized = serialize(testGraph, 2, maxNodes,  maxEdges, &countNode, &countEdge);
	p_node_t* deserialized = deserialize(serialized);

	//Assert correct count
	UNIT_TEST_ASSERT(countNode == 4);
	UNIT_TEST_ASSERT(countEdge == 3);

	//Assert correct deserialisation
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->edges->drain->addr), &(deserialized->edges->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->edges->next->drain->addr), &(deserialized->edges->next->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->edges->drain->edges->drain->addr), &(deserialized->edges->drain->edges->drain->addr)) != 0);
	UNIT_TEST_ASSERT(deserialized->edges->next->drain->edges == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->next->next == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges->next == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges->drain->edges == NULL);

	UNIT_TEST_END();
}

/**
 * initialize the graph
 * n1->n2,n1->n3,n2->n4
 * and then serialize and deserialize with k = 3
 */
UNIT_TEST(seriDeseri_3_hop)
{
	p_node_t* testGraph = test_graph();
	uint16_t maxNodes = 4,  maxEdges = 3;
	uint16_t countNode, countEdge;

	UNIT_TEST_BEGIN();

	void* serialized = serialize(testGraph, 3, maxNodes,  maxEdges, &countNode, &countEdge);
	p_node_t* deserialized = deserialize(serialized);

	//Assert correct count
	UNIT_TEST_ASSERT(countNode == 4);
	UNIT_TEST_ASSERT(countEdge == 3);

	//Assert correct deserialisation
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->addr), &(deserialized->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->edges->drain->addr), &(deserialized->edges->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->edges->next->drain->addr), &(deserialized->edges->next->drain->addr)) != 0);
	UNIT_TEST_ASSERT(rimeaddr_cmp(&(testGraph->edges->drain->edges->drain->addr), &(deserialized->edges->drain->edges->drain->addr)) != 0);
	UNIT_TEST_ASSERT(deserialized->edges->next->drain->edges == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->next->next == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges->next == NULL);
	UNIT_TEST_ASSERT(deserialized->edges->drain->edges->drain->edges == NULL);

	UNIT_TEST_END();
}

int main()
{
    UNIT_TEST_RUN(seriDeseri_root);
    UNIT_TEST_RUN(seriDeseri_0_hop);
    UNIT_TEST_RUN(seriDeseri_1_hop);
    UNIT_TEST_RUN(seriDeseri_2_hop);
    UNIT_TEST_RUN(seriDeseri_3_hop);

    return EXIT_SUCCESS;
}
