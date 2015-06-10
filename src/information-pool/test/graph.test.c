#include "contiki.h"
#include "unit-test.h"
//#include "serialize.h"
#include "graph.h"
#include "memory.h"
#include <stdlib.h>
#include <stdbool.h>


/**
 * Register unit tests that will be executed by using
 * the UNIT_TEST_RUN macro.
 */
UNIT_TEST_REGISTER(simpleMEMBtest, "Simple manually adding new Node+Edge to MEMB");
UNIT_TEST_REGISTER(simple_findNodeByAddr, "Simple manually adding new Node+Edge to MEMB and testing findNodeByAddr on it");
UNIT_TEST_REGISTER(complex_findNodeByAddr, "Fill MEMB directly and test findNodeByAddr on it for multiple nodes");
UNIT_TEST_REGISTER(edgeAlreadyExists, "Fill MEMB directly and test edgeAlreadyExists on it for multiple edges");
UNIT_TEST_REGISTER(handleExistingNode, "Test handle existing Node for a small update");
UNIT_TEST_REGISTER(updateNeighbour, "Test if neighbours get correctly added to graph.root");
UNIT_TEST_REGISTER(simple_iterateUpdate, "Test if iterateUpdate works for simple things");
UNIT_TEST_REGISTER(iterateUpdate, "Test if the complete update function works");
//UNIT_TEST_REGISTER(iterateUpdate_withSerialisation, "Test if the complete update function works in combination wioth (de)serialisation");



/**
 * test if adding nodes and edges to node_memory and edge_memory works
 */
UNIT_TEST(simpleMEMBtest)
	{	
		p_graph_t graph;
        p_node_t *n2, *nS2;
        p_edge_t *e1, *eS1;

        UNIT_TEST_BEGIN();
	
        graph.num_edges = 0;
        graph.num_nodes = 0;
		graph.root = new_node(&(graph.num_nodes));
    	graph.root->addr.u8[0] = 0x00;
    	graph.root->addr.u8[1] = 0x00;
    	graph.root->edges = NULL;

        n2 = (p_node_t*) malloc(sizeof(p_node_t));

        e1 = (p_edge_t*) malloc(sizeof(p_edge_t));

        //Give nodes IDs
        n2->addr.u8[0] = 0x02;
        n2->addr.u8[1] = 0x02;

        //Build Graph
        //e1->n2
        n2->edges = NULL;
        e1->next = NULL;
        e1->drain = n2;

        //get static memory from MEMB
        nS2 = new_node(&(graph.num_nodes));
        eS1 = new_edge(&(graph.num_edges));

        //copy content
        memcpy(nS2, n2, sizeof(p_node_t));
        memcpy(eS1, e1, sizeof(p_edge_t));

        //set the new pointers correctly
        graph.root->edges = eS1;
        eS1->drain = nS2;

        //Assert different pointers
        UNIT_TEST_ASSERT(n2 != nS2);
        UNIT_TEST_ASSERT(e1 != eS1);

        //Assert rimeaddr
        UNIT_TEST_ASSERT(rimeaddr_cmp(&(n2->addr), &(nS2->addr)));
        UNIT_TEST_ASSERT(rimeaddr_cmp(&(e1->drain->addr), &(eS1->drain->addr)));
        UNIT_TEST_ASSERT(rimeaddr_cmp(&(nS2->addr), &(graph.root->edges->drain->addr)));

        //Assert NULL pointers
        UNIT_TEST_ASSERT(e1->next == NULL);
        UNIT_TEST_ASSERT(eS1->next == NULL);
        UNIT_TEST_ASSERT(n2->edges == NULL);
        UNIT_TEST_ASSERT(nS2->edges == NULL);

        //Cleanup
        free(n2);
        free(e1);
        free_node(nS2, &(graph.num_nodes));
        free_edge(eS1, &(graph.num_edges));

        free_node(graph.root, &(graph.num_nodes));

        UNIT_TEST_ASSERT(graph.num_nodes == 0);
        UNIT_TEST_ASSERT(graph.num_edges == 0);

        UNIT_TEST_END();
        }


/**
 * test findNodeByAddr on a small tree
 */
UNIT_TEST(simple_findNodeByAddr)
	{
		p_graph_t graph;
        p_node_t *n2, *nS2;
        p_edge_t *e1, *eS1;

        UNIT_TEST_BEGIN();

        graph.num_edges = 0;
        graph.num_nodes = 0;
		graph.root = new_node(&(graph.num_nodes));
    	graph.root->addr.u8[0] = 0x00;
    	graph.root->addr.u8[1] = 0x00;
    	graph.root->edges = NULL;

        n2 = (p_node_t*) malloc(sizeof(p_node_t));

        e1 = (p_edge_t*) malloc(sizeof(p_edge_t));

        //Give nodes IDs
        n2->addr.u8[0] = 0x00;
        n2->addr.u8[1] = 0x02;

        //Build Graph
        //e1->n2
        n2->edges = NULL;
        e1->next = NULL;
        e1->drain = n2;

        //Assert if graph.root can be found, but nS2 can't yet
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, graph.root->addr) == graph.root);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n2->addr) == NULL);

        //get static memory from MEMB
        nS2 = new_node(&(graph.num_nodes));
        eS1 = new_edge(&(graph.num_edges));

        //copy content
        memcpy(nS2, n2, sizeof(p_node_t));
        memcpy(eS1, e1, sizeof(p_edge_t));

        //set the new pointers correctly
        graph.root->edges = eS1;
        eS1->drain = nS2;

        //Assert if graph.root can be found and nS2 too
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, graph.root->addr) == graph.root);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n2->addr) == nS2);

        //Assert different pointers
        UNIT_TEST_ASSERT(n2 != nS2);
        UNIT_TEST_ASSERT(e1 != eS1);

        //Assert rimeaddr
        UNIT_TEST_ASSERT(rimeaddr_cmp(&(n2->addr), &(nS2->addr)));
        UNIT_TEST_ASSERT(rimeaddr_cmp(&(e1->drain->addr), &(eS1->drain->addr)));
        UNIT_TEST_ASSERT(rimeaddr_cmp(&(nS2->addr), &(graph.root->edges->drain->addr)));

        //Assert NULL pointers
        UNIT_TEST_ASSERT(e1->next == NULL);
        UNIT_TEST_ASSERT(eS1->next == NULL);
        UNIT_TEST_ASSERT(n2->edges == NULL);
        UNIT_TEST_ASSERT(nS2->edges == NULL);

        //Cleanup
        free(n2);
        free(e1);
        free_node(nS2, &(graph.num_nodes));
        free_edge(eS1, &(graph.num_edges));

        free_node(graph.root, &(graph.num_nodes));
		
        UNIT_TEST_ASSERT(graph.num_nodes == 0);
        UNIT_TEST_ASSERT(graph.num_edges == 0);

        UNIT_TEST_END();
        }

/**
 * test findNodeByAddr with a more complex tree
 */
UNIT_TEST(complex_findNodeByAddr)
	{
		p_graph_t graph;
        p_node_t *n1, *n2, *n3, *n4, *n5, *n6;
        p_edge_t *e1, *e2, *e3, *e4, *e5, *e6;
        p_node_t *nX;

        UNIT_TEST_BEGIN();

        //set graph.root
        graph.num_edges = 0;
        graph.num_nodes = 0;
		graph.root = new_node(&(graph.num_nodes));
    	graph.root->addr.u8[0] = 0x00;
    	graph.root->addr.u8[1] = 0x00;
    	graph.root->edges = NULL;

    	//allocate nodes and edges in MEMB
    	n1 = new_node(&(graph.num_nodes));
    	n2 = new_node(&(graph.num_nodes));
    	n3 = new_node(&(graph.num_nodes));
    	n4 = new_node(&(graph.num_nodes));
    	n5 = new_node(&(graph.num_nodes));
    	n6 = new_node(&(graph.num_nodes));
    	nX = new_node(&(graph.num_nodes));

    	e1 = new_edge(&(graph.num_edges));
    	e2 = new_edge(&(graph.num_edges));
    	e3 = new_edge(&(graph.num_edges));
    	e4 = new_edge(&(graph.num_edges));
    	e5 = new_edge(&(graph.num_edges));
    	e6 = new_edge(&(graph.num_edges));

        //Give nodes IDs
        n1->addr.u8[0] = 0x01;
        n1->addr.u8[1] = 0x01;
        n2->addr.u8[0] = 0x02;
        n2->addr.u8[1] = 0x02;
        n3->addr.u8[0] = 0x03;
        n3->addr.u8[1] = 0x03;
        n4->addr.u8[0] = 0x04;
        n4->addr.u8[1] = 0x04;
        n5->addr.u8[0] = 0x05;
        n5->addr.u8[1] = 0x05;
        n6->addr.u8[0] = 0x06;
        n6->addr.u8[1] = 0x06;

        nX->addr.u8[0] = 0x99;
        nX->addr.u8[1] = 0x99;

        //Build Graph
        //graph.root->n1
        //graph.root->n2,n2->n3, n3->n4, n4->n5, n5->n6
        graph.root->edges = e1;
        e1->next = e2;
        e2->next = NULL;
        e1->drain = n1;
        e2->drain = n2;

        n1->edges = NULL;

        n2->edges = e3;
        e3->drain = n3;
        e3->next = NULL;

        n3->edges = e4;
        e4->drain = n4;
        e4->next = NULL;

        n4->edges = e5;
        e5->drain = n5;
        e5->next = NULL;

        n5->edges = e6;
        e6->drain = n6;
        e6->next = NULL;

        //Assert if every Node can be found
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, graph.root->addr) == graph.root);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n1->addr) == n1);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n2->addr) == n2);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n3->addr) == n3);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n4->addr) == n4);

        //These nodes are too deep for K=3 and shouldn't be found
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n5->addr) == NULL);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n6->addr) == NULL);

        //nX shouldn't be found since it isn't connected by an edge
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, nX->addr) == NULL);

        //cleanup
        free_node(n1, &(graph.num_nodes));
        free_node(n2, &(graph.num_nodes));
        free_node(n3, &(graph.num_nodes));
        free_node(n4, &(graph.num_nodes));
        free_node(n5, &(graph.num_nodes));
        free_node(n6, &(graph.num_nodes));
        free_node(nX, &(graph.num_nodes));

        free_edge(e1, &(graph.num_edges));
        free_edge(e2, &(graph.num_edges));
        free_edge(e3, &(graph.num_edges));
        free_edge(e4, &(graph.num_edges));
        free_edge(e5, &(graph.num_edges));
        free_edge(e6, &(graph.num_edges));

        free_node(graph.root, &(graph.num_nodes));
		
        UNIT_TEST_ASSERT(graph.num_nodes == 0);
        UNIT_TEST_ASSERT(graph.num_edges == 0);

        UNIT_TEST_END();
        }


/**
 * test the function edgeAlreadyExists with a graph that has a lot of edges. Actually not necessary to have a tree, since the function requires the source:node and thus doesn't crawl the tree.
 */
UNIT_TEST(edgeAlreadyExists)
	{
		p_graph_t graph;
        p_node_t *n1, *n2, *n3, *n4, *n5, *n6;
        p_edge_t *er1, *er2, *er3, *e11, *e12, *e13, *e14, *e21, *e31, *e32, *e33, *e41, *e51;
        p_node_t *nX;
        p_edge_t *eX;

        UNIT_TEST_BEGIN();

        //set graph.root
        graph.num_edges = 0;
        graph.num_nodes = 0;
		graph.root = new_node(&(graph.num_nodes));
    	graph.root->addr.u8[0] = 0x00;
    	graph.root->addr.u8[1] = 0x00;
    	graph.root->edges = NULL;

    	//allocate nodes and edges in MEMB
    	n1 = new_node(&(graph.num_nodes));
    	n2 = new_node(&(graph.num_nodes));
    	n3 = new_node(&(graph.num_nodes));
    	n4 = new_node(&(graph.num_nodes));
    	n5 = new_node(&(graph.num_nodes));
    	n6 = new_node(&(graph.num_nodes));
    	nX = new_node(&(graph.num_nodes));

    	er1 = new_edge(&(graph.num_edges));
    	er2 = new_edge(&(graph.num_edges));
    	er3 = new_edge(&(graph.num_edges));
    	e11 = new_edge(&(graph.num_edges));
    	e12 = new_edge(&(graph.num_edges));
    	e13 = new_edge(&(graph.num_edges));
    	e14 = new_edge(&(graph.num_edges));
    	e21 = new_edge(&(graph.num_edges));
    	e31 = new_edge(&(graph.num_edges));
    	e32 = new_edge(&(graph.num_edges));
    	e33 = new_edge(&(graph.num_edges));
    	e41 = new_edge(&(graph.num_edges));
    	e51 = new_edge(&(graph.num_edges));
    	eX = new_edge(&(graph.num_edges));

        //Give nodes IDs
        n1->addr.u8[0] = 0x01;
        n1->addr.u8[1] = 0x01;
        n2->addr.u8[0] = 0x02;
        n2->addr.u8[1] = 0x02;
        n3->addr.u8[0] = 0x03;
        n3->addr.u8[1] = 0x03;
        n4->addr.u8[0] = 0x04;
        n4->addr.u8[1] = 0x04;
        n5->addr.u8[0] = 0x05;
        n5->addr.u8[1] = 0x05;
        n6->addr.u8[0] = 0x06;
        n6->addr.u8[1] = 0x06;

        nX->addr.u8[0] = 0x99;
        nX->addr.u8[1] = 0x99;

        //Build Graph
        //graph.root->n1, n2, n3
        //n1->graph.root, n2, n3, n4
        //n2->n3
        //n3->graph.root, n1, n2
        //n4->n5
        //n5->n6
        //nX->NULL
        graph.root->edges = er1;
        er1->next = er2;
        er2->next = er3;
        er3->next = NULL;
        er1->drain = n1;
        er2->drain = n2;
        er3->drain = n3;

        n1->edges = e11;
        e11->next = e12;
        e12->next = e13;
        e13->next = e14;
        e14->next = NULL;
        e11->drain = graph.root;
        e12->drain = n2;
        e13->drain = n3;
        e14->drain = n4;

        n2->edges = e21;
        e21->next = NULL;
        e21->drain = n3;

        n3->edges = e31;
        e31->next = e32;
        e32->next = e33;
        e33->next = NULL;
        e31->drain = graph.root;
        e32->drain = n1;
        e33->drain = n2;

        n4->edges = e41;
        e41->next = NULL;
        e41->drain = n5;

        n5->edges = e51;
        e51->next = NULL;
        e51->drain = n6;

        n6->edges = NULL;

        nX->edges = eX;
        eX->next = NULL;
        eX->drain = nX;

        //Assert Edges
        //!!! edges are identified by their drain, since their source is implicit. this leads if tested with the same source node to:
        //er1==e32; er2==e12==e33; er3==e13==e21; e11==e31;
        //so for ease of testing, just use e41,e51 and eX as negative for most nodes
        UNIT_TEST_ASSERT(edgeAlreadyExists(er1, graph.root) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(er2, graph.root) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(er3, graph.root) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e41, graph.root) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e51, graph.root) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(eX, graph.root) == false);

        UNIT_TEST_ASSERT(edgeAlreadyExists(e11, n1) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e12, n1) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e13, n1) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e14, n1) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e41, n1) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e51, n1) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(eX, n1) == false);

        UNIT_TEST_ASSERT(edgeAlreadyExists(e21, n2) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e41, n2) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e51, n2) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(eX, n2) == false);

        UNIT_TEST_ASSERT(edgeAlreadyExists(e31, n3) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e32, n3) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e33, n3) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e41, n3) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e51, n3) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(eX, n3) == false);

        UNIT_TEST_ASSERT(edgeAlreadyExists(er1, n4) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(er2, n4) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(er3, n4) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e11, n4) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e12, n4) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e13, n4) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e14, n4) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e21, n4) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e31, n4) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e32, n4) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e33, n4) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e41, n4) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e51, n4) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(eX, n4) == false);

        UNIT_TEST_ASSERT(edgeAlreadyExists(er1, n5) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(er2, n5) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(er3, n5) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e11, n5) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e12, n5) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e13, n5) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e14, n5) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e21, n5) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e31, n5) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e32, n5) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e33, n5) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e41, n5) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e51, n5) == true);
        UNIT_TEST_ASSERT(edgeAlreadyExists(eX, n5) == false);

        UNIT_TEST_ASSERT(edgeAlreadyExists(er1, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(er2, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(er3, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e11, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e12, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e13, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e14, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e21, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e31, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e32, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e33, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e41, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(e51, nX) == false);
        UNIT_TEST_ASSERT(edgeAlreadyExists(eX, nX) == true);

        //cleanup
        free_node(n1, &(graph.num_nodes));
        free_node(n2, &(graph.num_nodes));
        free_node(n3, &(graph.num_nodes));
        free_node(n4, &(graph.num_nodes));
        free_node(n5, &(graph.num_nodes));
        free_node(n6, &(graph.num_nodes));
        free_node(nX, &(graph.num_nodes));

        free_edge(er1, &(graph.num_edges));
        free_edge(er2, &(graph.num_edges));
        free_edge(er3, &(graph.num_edges));
        free_edge(e11, &(graph.num_edges));
        free_edge(e12, &(graph.num_edges));
        free_edge(e13, &(graph.num_edges));
        free_edge(e14, &(graph.num_edges));
        free_edge(e21, &(graph.num_edges));
        free_edge(e31, &(graph.num_edges));
        free_edge(e32, &(graph.num_edges));
        free_edge(e33, &(graph.num_edges));
        free_edge(e41, &(graph.num_edges));
        free_edge(e51, &(graph.num_edges));
        free_edge(eX, &(graph.num_edges));

        free_node(graph.root, &(graph.num_nodes));
		
        UNIT_TEST_ASSERT(graph.num_nodes == 0);
        UNIT_TEST_ASSERT(graph.num_edges == 0);

        UNIT_TEST_END();
        }


/**
 * test if the handleExistingNode function works correctly by calling the function with a small update graph, which consists of 2 nodes already in the tree and 1 new one
 */
UNIT_TEST(handleExistingNode)
	{
		p_graph_t graph;
        p_node_t *n1, *n2, *n3, *n4, *n5, *n6;
        p_edge_t *e1, *e2, *e3, *e4, *e5, *e6;
        p_node_t *nX;

        p_node_t *un1, *un2, *un99;
        p_edge_t *ue1, *ue2, *ue3;

        UNIT_TEST_BEGIN();

        //set graph.root
        graph.num_edges = 0;
        graph.num_nodes = 0;
		graph.root = new_node(&(graph.num_nodes));
    	graph.root->addr.u8[0] = 0x00;
    	graph.root->addr.u8[1] = 0x00;
    	graph.root->edges = NULL;

    	//allocate nodes and edges in MEMB
    	n1 = new_node(&(graph.num_nodes));
    	n2 = new_node(&(graph.num_nodes));
    	n3 = new_node(&(graph.num_nodes));
    	n4 = new_node(&(graph.num_nodes));
    	n5 = new_node(&(graph.num_nodes));
    	n6 = new_node(&(graph.num_nodes));
    	nX = new_node(&(graph.num_nodes));

    	e1 = new_edge(&(graph.num_edges));
    	e2 = new_edge(&(graph.num_edges));
    	e3 = new_edge(&(graph.num_edges));
    	e4 = new_edge(&(graph.num_edges));
    	e5 = new_edge(&(graph.num_edges));
    	e6 = new_edge(&(graph.num_edges));

        un1 = (p_node_t*) malloc(sizeof(p_node_t));
        un2 = (p_node_t*) malloc(sizeof(p_node_t));
        un99 = (p_node_t*) malloc(sizeof(p_node_t));

        ue1 = (p_edge_t*) malloc(sizeof(p_edge_t));
        ue2 = (p_edge_t*) malloc(sizeof(p_edge_t));
        ue3 = (p_edge_t*) malloc(sizeof(p_edge_t));

        //Give nodes IDs
        n1->addr.u8[0] = 0x01;
        n1->addr.u8[1] = 0x01;
        n2->addr.u8[0] = 0x02;
        n2->addr.u8[1] = 0x02;
        n3->addr.u8[0] = 0x03;
        n3->addr.u8[1] = 0x03;
        n4->addr.u8[0] = 0x04;
        n4->addr.u8[1] = 0x04;
        n5->addr.u8[0] = 0x05;
        n5->addr.u8[1] = 0x05;
        n6->addr.u8[0] = 0x06;
        n6->addr.u8[1] = 0x06;

        nX->addr.u8[0] = 0x99;
        nX->addr.u8[1] = 0x99;

        un1->addr.u8[0] = 0x01;
        un1->addr.u8[1] = 0x01;
        un2->addr.u8[0] = 0x02;
        un2->addr.u8[1] = 0x02;
        un99->addr.u8[0] = 0x99;
        un99->addr.u8[1] = 0x99;

        //Build Graph
        //graph.root->n1
        //graph.root->n2,n2->n3, n3->n4, n4->n5, n5->n6
        graph.root->edges = e1;
        e1->next = e2;
        e2->next = NULL;
        e1->drain = n1;
        e2->drain = n2;

        n1->edges = NULL;

        n2->edges = e3;
        e3->drain = n3;
        e3->next = NULL;

        n3->edges = e4;
        e4->drain = n4;
        e4->next = NULL;

        n4->edges = e5;
        e5->drain = n5;
        e5->next = NULL;

        n5->edges = e6;
        e6->drain = n6;
        e6->next = NULL;

        //Update Graph
        //un1->un2,un99
        //un2->un99
        un1->edges = ue1;
        ue1->drain = un2;
        ue1->next = ue2;
        ue2->drain = un99;
        ue2->next = NULL;

        un2->edges = ue3;
        ue3->drain = un99;
        ue3->next = NULL;

        un99->edges = NULL;

        //handleExistingNode
        handleExistingNode(&graph, n1, un1, 1);

        //Assert if un99 can be found and the edges oh n1 and n2 are updated
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, un99->addr) != NULL);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, un99->addr) != un99);
        UNIT_TEST_ASSERT(edgeAlreadyExists(ue1, n1));
        UNIT_TEST_ASSERT(edgeAlreadyExists(ue2, n1));
        UNIT_TEST_ASSERT(edgeAlreadyExists(ue3, n2));

        //free update
        free(un1);
        free(un2);
        free(un99);
        free(ue1);
        free(ue2);
        free(ue3);

        //Assert if everything is alright after freeing the update
        rimeaddr_t tempaddr;
        tempaddr.u8[0] = 0x99;
        tempaddr.u8[1] = 0x99;
        p_edge_t tempe1;
        p_edge_t tempe2;
        p_edge_t tempe3;
        tempe1.drain = n2;
        tempe1.next = &tempe2;
        tempe2.drain = n1->edges->next->drain;
        tempe2.next = NULL;
        tempe3.drain = n1->edges->next->drain;
        tempe3.next = NULL;
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, tempaddr) != NULL);
        UNIT_TEST_ASSERT(edgeAlreadyExists(&tempe1, n1));
        UNIT_TEST_ASSERT(edgeAlreadyExists(&tempe2, n1));
        UNIT_TEST_ASSERT(edgeAlreadyExists(&tempe3, n2));

        //cleanup
        //new allocated memory from handleExistingNode
        free_node(n1->edges->next->drain, &(graph.num_nodes));
        free_edge(n1->edges->next, &(graph.num_edges));
        free_edge(n1->edges, &(graph.num_edges));
        free_edge(n2->edges->next, &(graph.num_edges));

        free_node(n1, &(graph.num_nodes));
        free_node(n2, &(graph.num_nodes));
        free_node(n3, &(graph.num_nodes));
        free_node(n4, &(graph.num_nodes));
        free_node(n5, &(graph.num_nodes));
        free_node(n6, &(graph.num_nodes));
        free_node(nX, &(graph.num_nodes));

        free_edge(e1, &(graph.num_edges));
        free_edge(e2, &(graph.num_edges));
        free_edge(e3, &(graph.num_edges));
        free_edge(e4, &(graph.num_edges));
        free_edge(e5, &(graph.num_edges));
        free_edge(e6, &(graph.num_edges));

        free_node(graph.root, &(graph.num_nodes));
		
        UNIT_TEST_ASSERT(graph.num_nodes == 0);
        UNIT_TEST_ASSERT(graph.num_edges == 0);

        UNIT_TEST_END();
        }

/**
 * tests if updateNeighbour works correctly by creating 3 new edges from graph.root
 */
UNIT_TEST(updateNeighbour)
	{
		p_graph_t graph;
        p_node_t *n1, *n2, *n3;

        UNIT_TEST_BEGIN();

        graph.num_edges = 0;
        graph.num_nodes = 0;
		graph.root = new_node(&(graph.num_nodes));
    	graph.root->addr.u8[0] = 0x00;
    	graph.root->addr.u8[1] = 0x00;
    	graph.root->edges = NULL;

    	n1 = new_node(&(graph.num_nodes));
    	n2 = new_node(&(graph.num_nodes));
    	n3 = new_node(&(graph.num_nodes));

        //Give nodes IDs
        n1->addr.u8[0] = 0x01;
        n1->addr.u8[1] = 0x01;
        n2->addr.u8[0] = 0x02;
        n2->addr.u8[1] = 0x02;
        n3->addr.u8[0] = 0x03;
        n3->addr.u8[1] = 0x03;

        //update neighbours
        updateNeighbour(&graph, n1);
        updateNeighbour(&graph, n2);
        updateNeighbour(&graph, n3);

        //Assert rimeaddr
        UNIT_TEST_ASSERT(rimeaddr_cmp(&(graph.root->edges->drain->addr), &(n1->addr)));
        UNIT_TEST_ASSERT(rimeaddr_cmp(&(graph.root->edges->next->drain->addr), &(n2->addr)));
        UNIT_TEST_ASSERT(rimeaddr_cmp(&(graph.root->edges->next->next->drain->addr), &(n3->addr)));

        //Cleanup
        //new allocated memory from updateNeighbour
        free_edge(graph.root->edges->next->next, &(graph.num_edges));
        free_edge(graph.root->edges->next, &(graph.num_edges));
        free_edge(graph.root->edges, &(graph.num_edges));

        free_node(n1, &(graph.num_nodes));
        free_node(n2, &(graph.num_nodes));
        free_node(n3, &(graph.num_nodes));

        free_node(graph.root, &(graph.num_nodes));
		
        UNIT_TEST_ASSERT(graph.num_nodes == 0);
        UNIT_TEST_ASSERT(graph.num_edges == 0);

        UNIT_TEST_END();
        }
/**
 * test iterateUpdate with a simple tree in MEMB and a simple tree as update
 */
UNIT_TEST(simple_iterateUpdate)
	{
		p_graph_t graph;
		//the already known tree
		p_node_t *nS1, *nS2;
		p_edge_t *eSr1, *eS11;
		//the update
        p_node_t *n1, *n2, *n3, *n4;
        p_edge_t *e11, *e12, *e13, *e21;

        UNIT_TEST_BEGIN();

        //set graph.root
        graph.num_edges = 0;
        graph.num_nodes = 0;
		graph.root = new_node(&(graph.num_nodes));
    	graph.root->addr.u8[0] = 0x00;
    	graph.root->addr.u8[1] = 0x00;
    	graph.root->edges = NULL;

    	//allocate nodes and edges
    	nS1 = new_node(&(graph.num_nodes));
    	nS2 = new_node(&(graph.num_nodes));

    	eSr1 = new_edge(&(graph.num_edges));
    	eS11 = new_edge(&(graph.num_edges));

        n1 = (p_node_t*) malloc(sizeof(p_node_t));
        n2 = (p_node_t*) malloc(sizeof(p_node_t));
        n3 = (p_node_t*) malloc(sizeof(p_node_t));
        n4 = (p_node_t*) malloc(sizeof(p_node_t));

    	e11 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e12 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e13 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e21 = (p_edge_t*) malloc(sizeof(p_edge_t));

        //Give nodes IDs
        nS1->addr.u8[0] = 0x01;
        nS1->addr.u8[1] = 0x01;
        nS2->addr.u8[0] = 0x02;
        nS2->addr.u8[1] = 0x02;

        n1->addr.u8[0] = 0x01;
        n1->addr.u8[1] = 0x01;
        n2->addr.u8[0] = 0x02;
        n2->addr.u8[1] = 0x02;
        n3->addr.u8[0] = 0x03;
        n3->addr.u8[1] = 0x03;
        n4->addr.u8[0] = 0x04;
        n4->addr.u8[1] = 0x04;

        //Build Graph (MEMB)
        graph.root->edges = eSr1;
        eSr1->next = NULL;
        eSr1->drain = nS1;

        nS1->edges = eS11;
        eS11->next = NULL;
        eS11->drain = nS2;

        nS2->edges = NULL;

        //Build Graph (Update)
        //n1->n2, n3, n4
        //n2->n3
        n1->edges = e11;
        e11->next = e12;
        e11->drain = n2;
        e12->next = e13;
        e12->drain = n3;
        e13->next = NULL;
        e13->drain = n4;

        n2->edges = e21;
        e21->next = NULL;
        e21->drain = n3;

        //update
        updateGraph(&graph, n1);

        //Assert if every Node can be found
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, graph.root->addr) == graph.root);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, nS1->addr) == nS1); //n1 got freed at the end of iterateUpdate
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, nS2->addr) == nS2);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n2->addr) == nS2);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n3->addr) == nS1->edges->next->drain);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n4->addr) == nS1->edges->next->next->drain);

        //cleanup
        //first the generated ones
        free_node(nS1->edges->next->next->drain, &(graph.num_nodes));
        free_node(nS1->edges->next->drain, &(graph.num_nodes));
        free_edge(nS1->edges->next->next, &(graph.num_edges));
        free_edge(nS1->edges->next, &(graph.num_edges));
        free_edge(nS2->edges, &(graph.num_edges));

        free_node(nS1, &(graph.num_nodes));
        free_node(nS2, &(graph.num_nodes));

        free_edge(eSr1, &(graph.num_edges));
        free_edge(eS11, &(graph.num_edges));

        free_node(graph.root, &(graph.num_nodes));
		
        UNIT_TEST_ASSERT(graph.num_nodes == 0);
        UNIT_TEST_ASSERT(graph.num_edges == 0);

        UNIT_TEST_END();
        }

/**
 * test iterateUpdate with a graph already in MEMB and a bigger Graph as update that gets trimmed
 * then update 3 more times from different start nodes of the update graph
 */
UNIT_TEST(iterateUpdate)
	{
		p_graph_t graph;
		//the already known tree
		p_node_t *nS1, *nS2, *nS3, *nS4, *nS5;
		p_edge_t *eSr1, *eS11, *eS12, *eS14, *eS21, *eS32, *eS33, *eS41;
		//the update
        p_node_t *n1, *n2, *n3, *n4, *n5, *n6, *n7, *n8;
        p_edge_t *e11, *e12, *e13, *e14, *e21, *e22, *e31, *e32, *e33, *e41, *e51, *e61;
        //disconnected from the tree, but still inside MEMB
        p_node_t *nX;
        p_edge_t *eX;

        UNIT_TEST_BEGIN();

        //set graph.root
        graph.num_edges = 0;
        graph.num_nodes = 0;
		graph.root = new_node(&(graph.num_nodes));
    	graph.root->addr.u8[0] = 0x00;
    	graph.root->addr.u8[1] = 0x00;
    	graph.root->edges = NULL;

    	//allocate nodes and edges
    	nS1 = new_node(&(graph.num_nodes));
    	nS2 = new_node(&(graph.num_nodes));
    	nS3 = new_node(&(graph.num_nodes));
    	nS4 = new_node(&(graph.num_nodes));
    	nS5 = new_node(&(graph.num_nodes));
    	nX = new_node(&(graph.num_nodes));

    	eSr1 = new_edge(&(graph.num_edges));
    	eS11 = new_edge(&(graph.num_edges));
    	eS12 = new_edge(&(graph.num_edges));
    	eS14 = new_edge(&(graph.num_edges));
    	eS21 = new_edge(&(graph.num_edges));
    	eS32 = new_edge(&(graph.num_edges));
    	eS33 = new_edge(&(graph.num_edges));
    	eS41 = new_edge(&(graph.num_edges));
    	eX = new_edge(&(graph.num_edges));

        n1 = (p_node_t*) malloc(sizeof(p_node_t));
        n2 = (p_node_t*) malloc(sizeof(p_node_t));
        n3 = (p_node_t*) malloc(sizeof(p_node_t));
        n4 = (p_node_t*) malloc(sizeof(p_node_t));
        n5 = (p_node_t*) malloc(sizeof(p_node_t));
        n6 = (p_node_t*) malloc(sizeof(p_node_t));
        n7 = (p_node_t*) malloc(sizeof(p_node_t));
        n8 = (p_node_t*) malloc(sizeof(p_node_t));

    	e11 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e12 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e13 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e14 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e21 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e22 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e31 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e32 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e33 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e41 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e51 = (p_edge_t*) malloc(sizeof(p_edge_t));
    	e61 = (p_edge_t*) malloc(sizeof(p_edge_t));

        //Give nodes IDs
        nS1->addr.u8[0] = 0x01;
        nS1->addr.u8[1] = 0x01;
        nS2->addr.u8[0] = 0x02;
        nS2->addr.u8[1] = 0x02;
        nS3->addr.u8[0] = 0x03;
        nS3->addr.u8[1] = 0x03;
        nS4->addr.u8[0] = 0x04;
        nS4->addr.u8[1] = 0x04;
        nS5->addr.u8[0] = 0x05;
        nS5->addr.u8[1] = 0x05;

        n1->addr.u8[0] = 0x01;
        n1->addr.u8[1] = 0x01;
        n2->addr.u8[0] = 0x02;
        n2->addr.u8[1] = 0x02;
        n3->addr.u8[0] = 0x03;
        n3->addr.u8[1] = 0x03;
        n4->addr.u8[0] = 0x04;
        n4->addr.u8[1] = 0x04;
        n5->addr.u8[0] = 0x05;
        n5->addr.u8[1] = 0x05;
        n6->addr.u8[0] = 0x06;
        n6->addr.u8[1] = 0x06;
        n7->addr.u8[0] = 0x07;
        n7->addr.u8[1] = 0x07;

        n8->addr.u8[0] = 0x99;
        n8->addr.u8[1] = 0x99;

        nX->addr.u8[0] = 0x99;
        nX->addr.u8[1] = 0x99;


        //Build Graph (MEMB)
        graph.root->edges = eSr1;
        eSr1->next = NULL;
        eSr1->drain = nS1;

        nS1->edges = eS11;
        eS11->next = eS12;
        eS12->next = eS14;
        eS14->next = NULL;
        eS11->drain = graph.root;
        eS12->drain = nS2;
        eS14->drain = nS4;

        nS2->edges = eS21;
        eS21->next = NULL;
        eS21->drain = nS3;

        nS3->edges = eS32;
        eS32->next = eS33;
        eS33->next = NULL;
        eS32->drain = nS1;
        eS33->drain = nS2;

        nS4->edges = eS41;
        eS41->next = NULL;
        eS41->drain = nS5;

        nS5->edges = NULL;

        nX->edges = eX;
        eX->next = NULL;
        eX->drain = nX;


        //Build Graph (Update)
        //n1->graph.root, n2, n3, n4
        //n2->n3,n8
        //n3->graph.root, n1, n2
        //n4->n5
        //n5->n6
        //n6->n7
        n1->edges = e11;
        e11->next = e12;
        e12->next = e13;
        e13->next = e14;
        e14->next = NULL;
        e11->drain = graph.root;
        e12->drain = n2;
        e13->drain = n3;
        e14->drain = n4;

        n2->edges = e21;
        e21->next = e22;
        e21->drain = n3;
        e22->next = NULL;
        e22->drain = n8;

        n3->edges = e31;
        e31->next = e32;
        e32->next = e33;
        e33->next = NULL;
        e31->drain = graph.root;
        e32->drain = n1;
        e33->drain = n2;

        n4->edges = e41;
        e41->next = NULL;
        e41->drain = n5;

        n5->edges = e51;
        e51->next = NULL;
        e51->drain = n6;

        n6->edges = e61;
        e61->next = NULL;
        e61->drain = n7;

        n7->edges = NULL;

        //nX shouldn't be found since it isn't connected by an edge
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, nX->addr) == NULL);

        //update
        updateGraph(&graph, n1);

        //Assert if every Node can be found
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, graph.root->addr) == graph.root);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, nS1->addr) == nS1); //n1 got freed at the end of iterateUpdate, nS1 will do fine anyways
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n2->addr) == nS2);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n3->addr) == nS3);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n4->addr) == nS4);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n5->addr) == nS5);

        //n8 is special, because nX exists with the same address in MEMB, but is not reachable from graph.root
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n8->addr) != NULL);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n8->addr) != nX);

        //These nodes are too deep for K=3 and shouldn't be found
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n6->addr) == NULL);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n7->addr) == NULL);

        //update again with n2 and n3 to create new neighbours for graph.root
        updateGraph(&graph, n2);
        updateGraph(&graph, n3);

        //Assert again if something broke
        //Assert if every Node can be found
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, graph.root->addr) == graph.root);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, nS1->addr) == nS1); //n1 got freed at the end of iterateUpdate, nS1 will do fine anyways
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, nS2->addr) == nS2); //now for n2 too
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, nS3->addr) == nS3); //now for n3 too
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n4->addr) == nS4);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n5->addr) == nS5);

        //n8 is special, because nX exists with the same address in MEMB, but is not reachable from graph.root
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n8->addr) != NULL);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n8->addr) != nX);

        //These nodes are too deep for K=3 and shouldn't be found
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n6->addr) == NULL);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n7->addr) == NULL);

        //update with n6 to see if the deeper nodes (n7) can be found now
        updateGraph(&graph, n6);

        //Assert again if something broke
        //Assert if every Node can be found
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, graph.root->addr) == graph.root);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, nS1->addr) == nS1); //n1 got freed at the end of iterateUpdate, nS1 will do fine anyways
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, nS2->addr) == nS2); //now for n2 too
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, nS3->addr) == nS3); //now for n3 too
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n4->addr) == nS4);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n5->addr) == nS5);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, graph.root->edges->next->next->next->drain->addr) == graph.root->edges->next->next->next->drain); //now for n6 too

        //n8 is special, because nX exists with the same address in MEMB, but is not reachable from graph.root
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n8->addr) != NULL);
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n8->addr) != nX);

        //n7 should be found now
        UNIT_TEST_ASSERT(findNodeByAddr(graph.root, n7->addr) != NULL);

        //cleanup
        //first the new ones
        free_node(eS21->next->drain, &(graph.num_nodes));
        free_node(graph.root->edges->next->next->next->drain->edges->drain, &(graph.num_nodes));
        free_edge(graph.root->edges->next->next->next->drain->edges, &(graph.num_edges));
        free_node(graph.root->edges->next->next->next->drain, &(graph.num_nodes));
        free_edge(graph.root->edges->next->next->next, &(graph.num_edges));
        free_edge(graph.root->edges->next->next, &(graph.num_edges));
        free_edge(graph.root->edges->next, &(graph.num_edges));
        free_edge(eS33->next, &(graph.num_edges));
        free_edge(eS14->next, &(graph.num_edges));
        free_edge(eS21->next, &(graph.num_edges));

        free_node(nS1, &(graph.num_nodes));
        free_node(nS2, &(graph.num_nodes));
        free_node(nS3, &(graph.num_nodes));
        free_node(nS4, &(graph.num_nodes));
        free_node(nS5, &(graph.num_nodes));
        free_node(nX, &(graph.num_nodes));

        free_edge(eSr1, &(graph.num_edges));
        free_edge(eS11, &(graph.num_edges));
        free_edge(eS12, &(graph.num_edges));
        free_edge(eS14, &(graph.num_edges));
        free_edge(eS21, &(graph.num_edges));
        free_edge(eS32, &(graph.num_edges));
        free_edge(eS33, &(graph.num_edges));
        free_edge(eS41, &(graph.num_edges));
        free_edge(eX, &(graph.num_edges));

        free(n1);
        free(n2);
        free(n3);
        free(n4);
        free(n5);
        free(n6);
        free(n7);
        free(n8);

        free(e11);
        free(e12);
        free(e13);
        free(e14);
        free(e21);
        free(e22);
        free(e31);
        free(e32);
        free(e33);
        free(e41);
        free(e51);
        free(e61);

        free_node(graph.root, &(graph.num_nodes));
		
        UNIT_TEST_ASSERT(graph.num_nodes == 0);
        UNIT_TEST_ASSERT(graph.num_edges == 0);

        UNIT_TEST_END();
        }

void main() {
	//initialize edge_memory and node_memory
	init_mem();

	UNIT_TEST_RUN(simpleMEMBtest);
	UNIT_TEST_RUN(simple_findNodeByAddr);
	UNIT_TEST_RUN(complex_findNodeByAddr);
	UNIT_TEST_RUN(edgeAlreadyExists);
	UNIT_TEST_RUN(handleExistingNode);
	UNIT_TEST_RUN(updateNeighbour);
	UNIT_TEST_RUN(simple_iterateUpdate);
	UNIT_TEST_RUN(iterateUpdate);
	//TODO: UNIT_TEST_RUN(iterateUpdate_withSerialisation);


}
