#include "contiki.h"
#include "unit-test.h"
#include "serialize.h"
#include "graph.h"
#include <stdlib.h>

/**
 * Register unit tests that will be executed by using
 * the UNIT_TEST_RUN macro.
 */
UNIT_TEST_REGISTER(seriDeseri_root, "Deserialize only root");
UNIT_TEST_REGISTER(seriDeseri_0_hop, "Deserialize tree 0 hop");
UNIT_TEST_REGISTER(seriDeseri_1_hop, "Deserialize tree 1 hop");
UNIT_TEST_REGISTER(seriDeseri_2_hop, "Deserialize tree 2 hop");
UNIT_TEST_REGISTER(seriDeseri_3_hop, "Deserialize tree 3 hop");

/**
 * only the root without edges is initialized and crawls a depth of 99
 */
UNIT_TEST(seriDeseri_root)
        {
                p_node_t *root;
        void* serializeptr;
        uint16_t countNode;
        uint16_t countEdge;
        p_node_t *deserialized;

        UNIT_TEST_BEGIN();

        root = (p_node_t*) malloc(sizeof(p_node_t));
        root->addr.u8[0] = 0x01;
        root->last_seen = 0;
        root->edges = NULL;

        serializeptr = serialize(root,99,1,0,&countNode,&countEdge);

        UNIT_TEST_ASSERT(root->addr.u8[0] == ((p_node_t*)serializeptr)->addr.u8[0]);
        UNIT_TEST_ASSERT(root->edges == ((p_node_t*)serializeptr)->edges);

        deserialized = deserialize(serializeptr);

        UNIT_TEST_ASSERT(root->addr.u8[0] == deserialized->addr.u8[0]);
        UNIT_TEST_ASSERT(root->edges == deserialized->edges);


        UNIT_TEST_END();
        }

/**
 * initialize the graph
 * n1->n2,n1->n3,n2->n4
 * and then serialize and deserialize with k = 0
 */
UNIT_TEST(seriDeseri_0_hop)
        {
                p_node_t *n1, *n2, *n3, *n4;
        p_edge_t *e1, *e2, *e3;
        uint16_t maxNodes,  maxEdges;
        uint16_t *countNode, *countEdge;
        void *sn1, *sn2, *sn3, *sn4, *se1, *se2, *se3;
        void* testptr;
        p_node_t *nD;

        UNIT_TEST_BEGIN();

        n1 = (p_node_t*) malloc(sizeof(p_node_t));
        n2 = (p_node_t*) malloc(sizeof(p_node_t));
        n3 = (p_node_t*) malloc(sizeof(p_node_t));
        n4 = (p_node_t*) malloc(sizeof(p_node_t));

        e1 = (p_edge_t*) malloc(sizeof(p_edge_t));
        e2 = (p_edge_t*) malloc(sizeof(p_edge_t));
        e3 = (p_edge_t*) malloc(sizeof(p_edge_t));

        maxNodes = 4;
        maxEdges = 3;
        countNode = (uint16_t*) malloc(sizeof(uint16_t));
        countEdge = (uint16_t*) malloc(sizeof(uint16_t));

        //Give nodes IDs
        n1->addr.u8[0] = 0x01;
        n2->addr.u8[0] = 0x02;
        n3->addr.u8[0] = 0x03;
        n4->addr.u8[0] = 0x04;

        //Build Graph, root = n1
        //n1->n2,n1->n3,n2->n4
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

        //serialize
        testptr = serialize(n1,0, maxNodes,  maxEdges, countNode, countEdge);

        //get the serialized graph
        sn1 = testptr;
        se1 = (void*)testptr+(uint16_t)((p_node_t*)sn1)->edges;
        se2 = (void*)testptr+(uint16_t)((p_edge_t*)se1)->next;
        sn2 = (void*)testptr+(uint16_t)((p_edge_t*)se1)->drain;
        sn3 = (void*)testptr+(uint16_t)((p_edge_t*)se2)->drain;
        se3 = (void*)testptr+(uint16_t)((p_node_t*)sn2)->edges;
        sn4 = (void*)testptr+(uint16_t)((p_edge_t*)se3)->drain;

        //deserialize
        nD = deserialize(testptr);

        //Assert original unchanged
        UNIT_TEST_ASSERT(n1->addr.u8[0] == 1);
        UNIT_TEST_ASSERT(n1->edges->drain->addr.u8[0] == 2);
        UNIT_TEST_ASSERT(n1->edges->next->drain->addr.u8[0] == 3);
        UNIT_TEST_ASSERT(n1->edges->drain->edges->drain->addr.u8[0] == 4);

        //Assert correct count
        UNIT_TEST_ASSERT(*countNode == 1);
        UNIT_TEST_ASSERT(*countEdge == 0);

        //Assert different pointers after serialization
        UNIT_TEST_ASSERT(n1 != sn1);

        //Assert different pointers after deserialization
        UNIT_TEST_ASSERT(n1 != nD);

        //Assert correct serialization
        UNIT_TEST_ASSERT(n1->addr.u8[0] == ((p_node_t*)sn1)->addr.u8[0]);
        UNIT_TEST_ASSERT(((p_node_t*)sn1)->edges == NULL);

        //Assert correct deserialisation
        UNIT_TEST_ASSERT(n1->addr.u8[0] == nD->addr.u8[0]);
        UNIT_TEST_ASSERT(nD->edges == NULL);

        UNIT_TEST_END();
        }

/**
 * initialize the graph
 * n1->n2,n1->n3,n2->n4
 * and then serialize and deserialize with k = 1
 */
UNIT_TEST(seriDeseri_1_hop)
        {
                p_node_t *n1, *n2, *n3, *n4;
        p_edge_t *e1, *e2, *e3;
        uint16_t maxNodes,  maxEdges;
        uint16_t *countNode, *countEdge;
        void *sn1, *sn2, *sn3, *sn4, *se1, *se2, *se3;
        void* testptr;
        p_node_t *nD;

        UNIT_TEST_BEGIN();

        n1 = (p_node_t*) malloc(sizeof(p_node_t));
        n2 = (p_node_t*) malloc(sizeof(p_node_t));
        n3 = (p_node_t*) malloc(sizeof(p_node_t));
        n4 = (p_node_t*) malloc(sizeof(p_node_t));

        e1 = (p_edge_t*) malloc(sizeof(p_edge_t));
        e2 = (p_edge_t*) malloc(sizeof(p_edge_t));
        e3 = (p_edge_t*) malloc(sizeof(p_edge_t));

        maxNodes = 4;
        maxEdges = 3;
        countNode = (uint16_t*) malloc(sizeof(uint16_t));
        countEdge = (uint16_t*) malloc(sizeof(uint16_t));

        //Give nodes IDs
        n1->addr.u8[0] = 0x01;
        n2->addr.u8[0] = 0x02;
        n3->addr.u8[0] = 0x03;
        n4->addr.u8[0] = 0x04;

        //Build Graph, root = n1
        //n1->n2,n1->n3,n2->n4
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

        //serialize
        testptr = serialize(n1,1, maxNodes,  maxEdges, countNode, countEdge);

        //get the serialized graph
        sn1 = testptr;
        se1 = (void*)testptr+(uint16_t)((p_node_t*)sn1)->edges;
        se2 = (void*)testptr+(uint16_t)((p_edge_t*)se1)->next;
        sn2 = (void*)testptr+(uint16_t)((p_edge_t*)se1)->drain;
        sn3 = (void*)testptr+(uint16_t)((p_edge_t*)se2)->drain;
        se3 = (void*)testptr+(uint16_t)((p_node_t*)sn2)->edges;
        sn4 = (void*)testptr+(uint16_t)((p_edge_t*)se3)->drain;

        //deserialize
        nD = deserialize(testptr);

        //Assert original unchanged
        UNIT_TEST_ASSERT(n1->addr.u8[0] == 1);
        UNIT_TEST_ASSERT(n1->edges->drain->addr.u8[0] == 2);
        UNIT_TEST_ASSERT(n1->edges->next->drain->addr.u8[0] == 3);
        UNIT_TEST_ASSERT(n1->edges->drain->edges->drain->addr.u8[0] == 4);

        //Assert correct count
        UNIT_TEST_ASSERT(*countNode == 3);
        UNIT_TEST_ASSERT(*countEdge == 2);

        //Assert different pointers after serialization
        UNIT_TEST_ASSERT(n1 != sn1);
        UNIT_TEST_ASSERT(n2 != sn2);
        UNIT_TEST_ASSERT(n3 != sn3);

        //Assert different pointers after deserialization
        UNIT_TEST_ASSERT(n1 != nD);
        UNIT_TEST_ASSERT(n2 != nD->edges->drain);
        UNIT_TEST_ASSERT(n3 != nD->edges->next->drain);

        //Assert correct serialization
        UNIT_TEST_ASSERT(n1->addr.u8[0] == ((p_node_t*)sn1)->addr.u8[0]);
        UNIT_TEST_ASSERT(n2->addr.u8[0] == ((p_node_t*)sn2)->addr.u8[0]);
        UNIT_TEST_ASSERT(n3->addr.u8[0] == ((p_node_t*)sn3)->addr.u8[0]);
        UNIT_TEST_ASSERT(((p_node_t*)sn2)->edges == NULL);
        UNIT_TEST_ASSERT(((p_node_t*)sn3)->edges == NULL);
        UNIT_TEST_ASSERT(((p_edge_t*)se2)->next == NULL);

        //Assert correct deserialisation
        UNIT_TEST_ASSERT(n1->addr.u8[0] == nD->addr.u8[0]);
        UNIT_TEST_ASSERT(n1->edges->drain->addr.u8[0] == nD->edges->drain->addr.u8[0]);
        UNIT_TEST_ASSERT(n1->edges->next->drain->addr.u8[0] == nD->edges->next->drain->addr.u8[0]);
        UNIT_TEST_ASSERT(nD->edges->drain->edges == NULL);
        UNIT_TEST_ASSERT(nD->edges->next->drain->edges == NULL);
        UNIT_TEST_ASSERT(nD->edges->next->next == NULL);

        UNIT_TEST_END();
        }

/**
 * initialize the graph
 * n1->n2,n1->n3,n2->n4
 * and then serialize and deserialize with k = 2
 */
UNIT_TEST(seriDeseri_2_hop)
        {
                p_node_t *n1, *n2, *n3, *n4;
        p_edge_t *e1, *e2, *e3;
        uint16_t maxNodes,  maxEdges;
        uint16_t *countNode, *countEdge;
        void *sn1, *sn2, *sn3, *sn4, *se1, *se2, *se3;
        void* testptr;
        p_node_t *nD;

        UNIT_TEST_BEGIN();

        n1 = (p_node_t*) malloc(sizeof(p_node_t));
        n2 = (p_node_t*) malloc(sizeof(p_node_t));
        n3 = (p_node_t*) malloc(sizeof(p_node_t));
        n4 = (p_node_t*) malloc(sizeof(p_node_t));

        e1 = (p_edge_t*) malloc(sizeof(p_edge_t));
        e2 = (p_edge_t*) malloc(sizeof(p_edge_t));
        e3 = (p_edge_t*) malloc(sizeof(p_edge_t));

        maxNodes = 4;
        maxEdges = 3;
        countNode = (uint16_t*) malloc(sizeof(uint16_t));
        countEdge = (uint16_t*) malloc(sizeof(uint16_t));

        //Give nodes IDs
        n1->addr.u8[0] = 0x01;
        n2->addr.u8[0] = 0x02;
        n3->addr.u8[0] = 0x03;
        n4->addr.u8[0] = 0x04;

        //Build Graph, root = n1
        //n1->n2,n1->n3,n2->n4
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

        //serialize
        testptr = serialize(n1,2, maxNodes,  maxEdges, countNode, countEdge);

        //get the serialized graph
        sn1 = testptr;
        se1 = (void*)testptr+(uint16_t)((p_node_t*)sn1)->edges;
        se2 = (void*)testptr+(uint16_t)((p_edge_t*)se1)->next;
        sn2 = (void*)testptr+(uint16_t)((p_edge_t*)se1)->drain;
        sn3 = (void*)testptr+(uint16_t)((p_edge_t*)se2)->drain;
        se3 = (void*)testptr+(uint16_t)((p_node_t*)sn2)->edges;
        sn4 = (void*)testptr+(uint16_t)((p_edge_t*)se3)->drain;

        //deserialize
        nD = deserialize(testptr);

        //Assert original unchanged
        UNIT_TEST_ASSERT(n1->addr.u8[0] == 1);
        UNIT_TEST_ASSERT(n1->edges->drain->addr.u8[0] == 2);
        UNIT_TEST_ASSERT(n1->edges->next->drain->addr.u8[0] == 3);
        UNIT_TEST_ASSERT(n1->edges->drain->edges->drain->addr.u8[0] == 4);

        //Assert correct count
        UNIT_TEST_ASSERT(*countNode == 4);
        UNIT_TEST_ASSERT(*countEdge == 3);

        //Assert different pointers after serialization
        UNIT_TEST_ASSERT(n1 != sn1);
        UNIT_TEST_ASSERT(n2 != sn2);
        UNIT_TEST_ASSERT(n3 != sn3);
        UNIT_TEST_ASSERT(n4 != sn4);

        //Assert different pointers after deserialization
        UNIT_TEST_ASSERT(n1 != nD);
        UNIT_TEST_ASSERT(n2 != nD->edges->drain);
        UNIT_TEST_ASSERT(n3 != nD->edges->next->drain);
        UNIT_TEST_ASSERT(n4 != nD->edges->drain->edges->drain);

        //Assert correct serialization
        UNIT_TEST_ASSERT(n1->addr.u8[0] == ((p_node_t*)sn1)->addr.u8[0]);
        UNIT_TEST_ASSERT(n2->addr.u8[0] == ((p_node_t*)sn2)->addr.u8[0]);
        UNIT_TEST_ASSERT(n3->addr.u8[0] == ((p_node_t*)sn3)->addr.u8[0]);
        UNIT_TEST_ASSERT(n4->addr.u8[0] == ((p_node_t*)sn4)->addr.u8[0]);
        UNIT_TEST_ASSERT(((p_node_t*)sn3)->edges == NULL);
        UNIT_TEST_ASSERT(((p_node_t*)sn4)->edges == NULL);
        UNIT_TEST_ASSERT(((p_edge_t*)se2)->next == NULL);
        UNIT_TEST_ASSERT(((p_edge_t*)se3)->next == NULL);

        //Assert correct deserialisation
        UNIT_TEST_ASSERT(n1->addr.u8[0] == nD->addr.u8[0]);
        UNIT_TEST_ASSERT(n1->edges->drain->addr.u8[0] == nD->edges->drain->addr.u8[0]);
        UNIT_TEST_ASSERT(n1->edges->next->drain->addr.u8[0] == nD->edges->next->drain->addr.u8[0]);
        UNIT_TEST_ASSERT(n1->edges->drain->edges->drain->addr.u8[0] == nD->edges->drain->edges->drain->addr.u8[0]);
        UNIT_TEST_ASSERT(nD->edges->next->drain->edges == NULL);
        UNIT_TEST_ASSERT(nD->edges->drain->edges->drain->edges == NULL);
        UNIT_TEST_ASSERT(nD->edges->next->next == NULL);
        UNIT_TEST_ASSERT(nD->edges->drain->edges->next == NULL);

        UNIT_TEST_END();
        }

/**
 * initialize the graph
 * n1->n2,n1->n3,n2->n4
 * and then serialize and deserialize with k = 0
 */
UNIT_TEST(seriDeseri_3_hop)
        {
                p_node_t *n1, *n2, *n3, *n4;
        p_edge_t *e1, *e2, *e3;
        uint16_t maxNodes,  maxEdges;
        uint16_t *countNode, *countEdge;
        void *sn1, *sn2, *sn3, *sn4, *se1, *se2, *se3;
        void* testptr;
        p_node_t *nD;

        UNIT_TEST_BEGIN();

        n1 = (p_node_t*) malloc(sizeof(p_node_t));
        n2 = (p_node_t*) malloc(sizeof(p_node_t));
        n3 = (p_node_t*) malloc(sizeof(p_node_t));
        n4 = (p_node_t*) malloc(sizeof(p_node_t));

        e1 = (p_edge_t*) malloc(sizeof(p_edge_t));
        e2 = (p_edge_t*) malloc(sizeof(p_edge_t));
        e3 = (p_edge_t*) malloc(sizeof(p_edge_t));

        maxNodes = 4;
        maxEdges = 3;
        countNode = (uint16_t*) malloc(sizeof(uint16_t));
        countEdge = (uint16_t*) malloc(sizeof(uint16_t));

        //Give nodes IDs
        n1->addr.u8[0] = 0x01;
        n2->addr.u8[0] = 0x02;
        n3->addr.u8[0] = 0x03;
        n4->addr.u8[0] = 0x04;

        //Build Graph, root = n1
        //n1->n2,n1->n3,n2->n4
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

        //serialize
        testptr = serialize(n1,3, maxNodes,  maxEdges, countNode, countEdge);

        //get the serialized graph
        sn1 = testptr;
        se1 = (void*)testptr+(uint16_t)((p_node_t*)sn1)->edges;
        se2 = (void*)testptr+(uint16_t)((p_edge_t*)se1)->next;
        sn2 = (void*)testptr+(uint16_t)((p_edge_t*)se1)->drain;
        sn3 = (void*)testptr+(uint16_t)((p_edge_t*)se2)->drain;
        se3 = (void*)testptr+(uint16_t)((p_node_t*)sn2)->edges;
        sn4 = (void*)testptr+(uint16_t)((p_edge_t*)se3)->drain;

        //deserialize
        nD = deserialize(testptr);

        //Assert original unchanged
        UNIT_TEST_ASSERT(n1->addr.u8[0] == 1);
        UNIT_TEST_ASSERT(n1->edges->drain->addr.u8[0] == 2);
        UNIT_TEST_ASSERT(n1->edges->next->drain->addr.u8[0] == 3);
        UNIT_TEST_ASSERT(n1->edges->drain->edges->drain->addr.u8[0] == 4);

        //Assert correct count
        UNIT_TEST_ASSERT(*countNode == 4);
        UNIT_TEST_ASSERT(*countEdge == 3);

        //Assert different pointers after serialization
        UNIT_TEST_ASSERT(n1 != sn1);
        UNIT_TEST_ASSERT(n2 != sn2);
        UNIT_TEST_ASSERT(n3 != sn3);
        UNIT_TEST_ASSERT(n4 != sn4);

        //Assert different pointers after deserialization
        UNIT_TEST_ASSERT(n1 != nD);
        UNIT_TEST_ASSERT(n2 != nD->edges->drain);
        UNIT_TEST_ASSERT(n3 != nD->edges->next->drain);
        UNIT_TEST_ASSERT(n4 != nD->edges->drain->edges->drain);

        //Assert correct serialization
        UNIT_TEST_ASSERT(n1->addr.u8[0] == ((p_node_t*)sn1)->addr.u8[0]);
        UNIT_TEST_ASSERT(n2->addr.u8[0] == ((p_node_t*)sn2)->addr.u8[0]);
        UNIT_TEST_ASSERT(n3->addr.u8[0] == ((p_node_t*)sn3)->addr.u8[0]);
        UNIT_TEST_ASSERT(n4->addr.u8[0] == ((p_node_t*)sn4)->addr.u8[0]);
        UNIT_TEST_ASSERT(((p_node_t*)sn3)->edges == NULL);
        UNIT_TEST_ASSERT(((p_node_t*)sn4)->edges == NULL);
        UNIT_TEST_ASSERT(((p_edge_t*)se2)->next == NULL);
        UNIT_TEST_ASSERT(((p_edge_t*)se3)->next == NULL);

        //Assert correct deserialisation
        UNIT_TEST_ASSERT(n1->addr.u8[0] == nD->addr.u8[0]);
        UNIT_TEST_ASSERT(n1->edges->drain->addr.u8[0] == nD->edges->drain->addr.u8[0]);
        UNIT_TEST_ASSERT(n1->edges->next->drain->addr.u8[0] == nD->edges->next->drain->addr.u8[0]);
        UNIT_TEST_ASSERT(n1->edges->drain->edges->drain->addr.u8[0] == nD->edges->drain->edges->drain->addr.u8[0]);
        UNIT_TEST_ASSERT(nD->edges->next->drain->edges == NULL);
        UNIT_TEST_ASSERT(nD->edges->drain->edges->drain->edges == NULL);
        UNIT_TEST_ASSERT(nD->edges->next->next == NULL);
        UNIT_TEST_ASSERT(nD->edges->drain->edges->next == NULL);

        UNIT_TEST_END();
        }

void main() {
    UNIT_TEST_RUN(seriDeseri_root);
    UNIT_TEST_RUN(seriDeseri_0_hop);
    UNIT_TEST_RUN(seriDeseri_1_hop);
    UNIT_TEST_RUN(seriDeseri_2_hop);
    UNIT_TEST_RUN(seriDeseri_3_hop);
}
