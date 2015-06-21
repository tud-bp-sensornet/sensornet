#include "contiki.h"
#include "graph.h"

#include <stdlib.h>
#include <stdio.h>

#include "serialize.h"

PROCESS(example_broadcast_process, "Broadcast example");
AUTOSTART_PROCESSES(&example_broadcast_process);

PROCESS_THREAD(example_broadcast_process, ev, data) {
	
	PROCESS_BEGIN();
	
	p_node_t *n1, *n2, *n3, *n4;
	p_edge_t *e1, *e2, *e3;
	uint16_t maxNodes,  maxEdges;
	uint16_t *countNode, *countEdge;
	
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
	void* testptr = serialize(n1,2, maxNodes,  maxEdges, countNode, countEdge);

	//Check if the entire Graph was iterated and counted correctly
	printf("%d:%d;%d:%d\n", maxNodes, *countNode, maxEdges, *countEdge);

	//get the serialized graph
	void* sn1 = testptr;
	void* se1 = (void*)testptr+(uint16_t)((p_node_t*)sn1)->edges;
	void* se2 = (void*)testptr+(uint16_t)((p_edge_t*)se1)->next;
	void* sn2 = (void*)testptr+(uint16_t)((p_edge_t*)se1)->drain;
	void* sn3 = (void*)testptr+(uint16_t)((p_edge_t*)se2)->drain;
	void* se3 = (void*)testptr+(uint16_t)((p_node_t*)sn2)->edges;
	void* sn4 = (void*)testptr+(uint16_t)((p_edge_t*)se3)->drain;

	//deserialize
	p_node_t *nD = deserialize(testptr);

	printf("Deserialized:Serialized\n");
	//memory addresses
	printf("%d:%d\n", nD, sn1);
	//node1 id
	printf("node1: %d:%d\n", nD->addr.u8[0], ((p_node_t*)sn1)->addr.u8[0]);
	//node2 id
	printf("node2: %d:%d\n", nD->edges->drain->addr.u8[0], ((p_node_t*)sn2)->addr.u8[0]);
	//node3 id
	printf("node3: %d:%d\n", nD->edges->next->drain->addr.u8[0], ((p_node_t*)sn3)->addr.u8[0]);
	//node4 id
	printf("node4: %d:%d\n", nD->edges->drain->edges->drain->addr.u8[0], ((p_node_t*)sn4)->addr.u8[0]);

	printf("Normal:Serialized\n");
	//memory addresses
	printf("%d:%d\n", n1, sn1);
	//node1 id
	printf("node1: %d:%d\n", n1->addr.u8[0], ((p_node_t*)sn1)->addr.u8[0]);
	//node2 id
	printf("node2: %d:%d\n", n1->edges->drain->addr.u8[0], ((p_node_t*)sn2)->addr.u8[0]);
	//node3 id
	printf("node3: %d:%d\n", n1->edges->next->drain->addr.u8[0], ((p_node_t*)sn3)->addr.u8[0]);
	//node4 id
	printf("node4: %d:%d\n", n1->edges->drain->edges->drain->addr.u8[0], ((p_node_t*)sn4)->addr.u8[0]);

	printf("%d\n", rimeaddr_node_addr.u8[0]);
	printf("%d:%d\n", n1->addr.u8[0], n1->addr.u8[1]);
	PROCESS_END();
}
