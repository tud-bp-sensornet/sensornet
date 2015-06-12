#include "contiki.h"

#include <stdlib.h>
#include <stdio.h>

#include "graph.h"
#include "serialize.h"

p_graph_t* test_graph(uint16_t neighbors, uint16_t* bytes)
{
	p_graph_t* graph = malloc(sizeof *graph);
	p_node_t* root = malloc(sizeof *root);

	p_edge_t** pivot = &(root->edges);

	int i;
	for (i = 0; i < neighbors; i++)
	{
		p_edge_t* edge = malloc(sizeof *edge);
		edge->next = NULL;
		*pivot = edge;

		p_node_t* neighbor = malloc(sizeof *neighbor);
		neighbor->addr = rimeaddr_null;
		neighbor->edges = NULL;
		edge->drain = neighbor;

		pivot = &(edge->next);
	}

	graph->root = root;
	graph->num_edges = neighbors;
	graph->num_nodes = neighbors + 1;

	*bytes = graph->num_edges * sizeof(p_edge_t) + graph->num_nodes * sizeof(p_node_t);

	return graph;
}

void free_test_graph(p_graph_t* graph)
{
	// free all neighbors first
	p_edge_t* pivot = graph->root->edges;
	while (pivot != NULL)
	{
		p_edge_t* next = pivot->next;

		free(pivot->drain);
		free(pivot);

		pivot = next;
	}

	free(graph->root);
	free(graph);
}

PROCESS(stress_test, "Stress test process");
AUTOSTART_PROCESSES(&stress_test);

PROCESS_THREAD(stress_test, ev, data)
{
	PROCESS_BEGIN();

	uint16_t bytes;
	uint16_t neighbors = 100;

	for (neighbors = 0; neighbors < 1000; neighbors += 1)
	{
		p_graph_t* graph = test_graph(neighbors, &bytes);

		printf("Allocated graph with %d neighbors (%d bytes)\n", neighbors, bytes);

		size_t sbytes;
		void* serialized = serialize(graph, 42, &sbytes);
		printf("Serialized graph (%d bytes)\n", (int)sbytes);

		deserialize(serialized);
		printf("Deserialized graph\n");

		free(serialized);
		printf("Free'd serialization\n");

		free_test_graph(graph);

		printf("Free'd graph\n");

		printf("----------------------------------------\n");
	}

	PROCESS_END();
}
