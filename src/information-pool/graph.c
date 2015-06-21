#include "contiki.h"
#include "net/rime.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "graph.h"
#include "memory.h"
#include <string.h>

p_node_t* iterateUpdateNode(p_graph_t* graph, p_node_t* node, uint8_t current_k);
p_edge_t* iterateUpdateEdge(p_graph_t* graph, p_edge_t * p_edge, uint8_t current_k, bool alloc_required);
void updateNeighbour(p_graph_t* graph, p_node_t * p_node);
void updateEdgeList(p_edge_t * edgelist_elem, p_edge_t * new_edge);
p_node_t* handleExistingNode(p_graph_t* graph, p_node_t * old_p_node, p_node_t * new_p_node, uint8_t current_k);
void handleExistingNode_Edgelist(p_graph_t* graph, p_edge_t * edgelist_elem_MEMB, p_edge_t * edgelist_elem_UPDATE, p_node_t * source_node, uint8_t current_k);
p_node_t* findNodeAddr_Node(p_node_t * p_node, rimeaddr_t addr, uint8_t current_k);
p_node_t* findNodeAddr_Edge(p_edge_t * p_edge, rimeaddr_t addr, uint8_t current_k);
p_edge_t* edgeAlreadyExists(p_edge_t * p_edge, p_node_t * source_node);
p_edge_t* edgeAlreadyExists_EdgeList(p_edge_t * p_edge, p_edge_t * edgelist_elem);
void handleGraphTimeoutNode(p_graph_t* graph, p_node_t* p_node, uint8_t current_k);
void handleGraphTimeoutEdge(p_graph_t* graph, p_node_t* source_node, p_edge_t* p_edge, uint8_t current_k);
void removeEdgeFromEdgelist(p_graph_t* graph, p_node_t* source_node, p_edge_t* p_edge, uint8_t current_k);
void removeEdgeFromEdgelist_Recursive(p_graph_t* graph, p_edge_t* previous_edge, p_edge_t* p_edge);
void removeNodeWithoutIncomingEdge(p_graph_t* graph, p_node_t* p_node, uint8_t current_k);
uint8_t clocktimeAsMinutes();


/**
 * Updates Node info from a deserialized graph
 * graph: The graph which will be iterated
 * other: pointer to the deserialized space
 */
void updateGraph(p_graph_t* graph, p_node_t* other){

	//current_k = 1, because the first edge will be created by updateNeighbour
	p_node_t *this_node = iterateUpdateNode(graph, other, 1);

	//the first node is the neighbour and it's treated differently
	updateNeighbour(graph, this_node);
}

/**
 * iterate over the nodes of the update and adjust the MEMB when necessary
 * graph: The graph which will be iterated
 * p_node: pointer to the current node in the deserialized update
 * current_k: the current depth of the update tree
 * return: the address of the node inside MEMB
 */
p_node_t* iterateUpdateNode(p_graph_t* graph, p_node_t* p_node, uint8_t current_k){
	p_node_t *this_node;

	//only save a maximum of K edges from the start node down into the tree. At the very least always save the direct neighbours (first node)
	if(current_k < K){

		//check if the node already exists in the local tree.
		//if it does, then merge the information, if it does not, then
		p_node_t *search_node = findNodeByAddr(graph->root, p_node->addr);
		if(search_node != NULL){
			this_node = handleExistingNode(graph, search_node, p_node, current_k);
		} else {
			//allocate space in MEMB and copy the contents
			this_node = new_node(&(graph->num_nodes));
			//get the correct pointer to the next edge, if it isn't NULL, and go further down the tree
			if(p_node->edges != NULL){
				p_node->edges = iterateUpdateEdge(graph, p_node->edges, current_k+1, true);
			}
			memcpy(this_node, p_node, sizeof(p_node_t));
		}
	} else {
		//Don't save outgoing edges from the K nodes
		//If the node already exists, just let handleExistingNode do everything, if not make a new node and set edges = NULL
		p_node_t *search_node = findNodeByAddr(graph->root, p_node->addr);
		if(search_node != NULL){
			this_node = handleExistingNode(graph, search_node, p_node, current_k);
		} else {
			this_node = new_node(&(graph->num_nodes));
			memcpy(this_node, p_node, sizeof(p_node_t));
			this_node->edges = NULL;
		}
	}

	return this_node;
}

/**
 * iterate over the edges of the update and adjust the MEMB when necessary
 * graph: The graph which will be iterated
 * p_edge: pointer to the current node in the deserialized update
 * current_k: the current depth of the update tree
 * alloc_required: bool that decides if memory is allocated for the edge with new_edge(&(graph->num_edges))
 * return: the address of the edge inside MEMB
 */
p_edge_t* iterateUpdateEdge(p_graph_t* graph, p_edge_t * p_edge, uint8_t current_k, bool alloc_required){
	p_edge_t *this_edge;

	//allocate memory if required
	if(alloc_required){
		this_edge = new_edge(&(graph->num_edges));
		memcpy(this_edge, p_edge, sizeof(p_edge_t));
	}

	//update the pointers to the new positions inside MEMB inside of the allocated memory of deserialize
	if(alloc_required){
		this_edge->drain = iterateUpdateNode(graph, p_edge->drain, current_k);
		if(p_edge->next != NULL){
			this_edge->next = iterateUpdateEdge(graph, p_edge->next, current_k, alloc_required);
		} else {
			this_edge->next = NULL;
		}

	} else {
		p_edge->drain = iterateUpdateNode(graph, p_edge->drain, current_k);
		if(p_edge->next != NULL){
			p_edge->next = iterateUpdateEdge(graph, p_edge->next, current_k, alloc_required);
		} else {
			this_edge->next = NULL;
		}
		this_edge = p_edge;
	}

	return this_edge;
}

/**
 * the direct neighbours of root need to be handled differently to create new edges and update last_seen
 * graph: The graph which will be iterated
 * p_node: pointer to the current node in the deserialized update
 */
void updateNeighbour(p_graph_t* graph, p_node_t * p_node){

	//create a new edge, allocate memory and fill it
	p_edge_t *new_p_edge;
	new_p_edge = new_edge(&(graph->num_edges));
	new_p_edge->drain = p_node;
	new_p_edge->next = NULL;
	//TODO: fix overflow of the ttl for long times
	new_p_edge->ttl = EDGE_TIMEOUT + clocktimeAsMinutes();


	p_edge_t *search_edge = edgeAlreadyExists(new_p_edge, graph->root);
	if(graph->root->edges == NULL){
		graph->root->edges = new_p_edge;
	} else if(search_edge == NULL){
		//if the edge does not already exist...
		updateEdgeList(graph->root->edges, new_p_edge);
	} else {
		//Could be solved more elegantly than allocating space in edge_memory and freeing it immediately again if unneeded

		search_edge->ttl = new_p_edge->ttl;
		free_edge(new_p_edge, &(graph->num_edges));
	}
}

/**
 * put the edge at the end of the list
 * edgelist_elem: the current edge
 * new_edge: the edge to be inserted
 */
void updateEdgeList(p_edge_t * edgelist_elem, p_edge_t * new_p_edge){
	if(edgelist_elem->next == NULL){
		edgelist_elem->next = new_p_edge;
	} else {
		updateEdgeList(edgelist_elem->next, new_p_edge);
	}
}

/*********************************************************************************/
/*****************************Handle Merging**************************************/
/*********************************************************************************/

/**
 * if a node already exists then the update can't be just thrown into MEMB
 * this function handles this problem
 * old_p_node: the pointer to the node which was found in MEMB
 * new_p_node: the node inside the update
 * current_k: the current depth of the update tree
 */
p_node_t* handleExistingNode(p_graph_t* graph, p_node_t * old_p_node, p_node_t * new_p_node, uint8_t current_k){

	//handle the outgoing edges if the new node isn't in K depth (no new edges will be saved for them)
	//both edgelists empty -> do nothing
	//no new edges -> do nothing
	if(current_k < K){
		if(old_p_node->edges == NULL && new_p_node->edges != NULL){
			//the node does currently not have edges -> all edges have to be allocated
			old_p_node->edges = iterateUpdateEdge(graph, new_p_node->edges, current_k+1, true);
		} else if(old_p_node->edges != NULL && new_p_node->edges != NULL) {
			//merge lists
			handleExistingNode_Edgelist(graph, old_p_node->edges, new_p_node->edges, old_p_node, current_k+1);
		}
	}

	return old_p_node;
}

/**
 * called by handleExistingNode and merges the new and the old edgelist. also calls iterateUpdateEdge for every edge
 * in the new list
 * edgelist_elem_MEMB: the first element of old edgelist
 * edgelist_elem_UPDATE: the current element of the new edgelist (first when called by handleExistingNode)
 * source_node: pointer to the source of the edge
 * current_k: the current depth of the update tree
 */
void handleExistingNode_Edgelist(p_graph_t* graph, p_edge_t * edgelist_elem_MEMB, p_edge_t * edgelist_elem_UPDATE, p_node_t * source_node, uint8_t current_k){
	p_edge_t *this_edge;

	//check if the current list element of the update is already known
	p_edge_t *search_edge = edgeAlreadyExists(edgelist_elem_UPDATE, source_node);
	if(search_edge != NULL){
		/**
		 * the edge is already known -> don't allocate memory or touch the known edge list.
		 * However, the edge points to a node that needs to be updated and merged -> call
		 * iterateUpdateEdge. Since the edge already exists, the drain Node also already exists
		 * so no allocation is required and the pointer is already correctly set, so no need for the
		 * return of iterateUpdateEdge.
		 */
		if(search_edge->ttl < edgelist_elem_UPDATE->ttl){
			search_edge->ttl = edgelist_elem_UPDATE->ttl;
		}
		//temporarily allocate a bit of memory and copy edgelist_elem_UPDATE, because the edge needs to have NULL
		//as next, but edgelist_elem_UPDATE needs the correct next element to recursively call handleExistingNode_Edgelist later
		this_edge = malloc(sizeof(p_edge_t));
		memcpy(this_edge, edgelist_elem_UPDATE, sizeof(p_edge_t));
		this_edge->next = NULL;
		iterateUpdateEdge(graph, this_edge, current_k, false);
		free(this_edge);
	} else {
		//the edge isn't known -> allocate memory and append at the end of the known edgelist after initiating
		//the update of the drain Node
		this_edge = new_edge(&(graph->num_edges));
		memcpy(this_edge, edgelist_elem_UPDATE, sizeof(p_edge_t));
		this_edge->next = NULL;
		iterateUpdateEdge(graph, this_edge, current_k, false);
		updateEdgeList(edgelist_elem_MEMB, this_edge);
	}

	//repeat recursively until the end of the list
	if(edgelist_elem_UPDATE->next != NULL){
		handleExistingNode_Edgelist(graph, edgelist_elem_MEMB, edgelist_elem_UPDATE->next, source_node, current_k);
	}

}

/*********************************************************************************/
/***********************Find Node by Rime address*********************************/
/*********************************************************************************/

/**
 * finds and returns the node which has the specified rimeaddr if it exists in MEMB. if it doesn't exist, return NULL
 * addr: the rimeaddr used to search for the node
 * return: pointer to the found node, if nothing was found NULL
 */
p_node_t* findNodeByAddr(p_node_t* root, rimeaddr_t addr){
	p_node_t* found_node;

	//start at root with k=0
	found_node = findNodeAddr_Node(root, addr, 0);

	return found_node;
}

/**
 * called by findNodeAddr to recursively crawl the nodes
 * p_node: the current node
 * addr: the rimeaddr used to search for the node
 * current_k: the current depth of the update tree
 * return: pointer to the found node, if nothing was found NULL
 */
p_node_t* findNodeAddr_Node(p_node_t * p_node, rimeaddr_t addr, uint8_t current_k){
	p_node_t *found_node;

	if(rimeaddr_cmp(&(p_node->addr), &addr)){		//is p_node the searched node?
		found_node = p_node;
	} else if (current_k < K){		//is it ok to go deeper into the tree?
		if(p_node->edges != NULL){
			found_node = findNodeAddr_Edge(p_node->edges, addr, current_k+1);
		} else {
			found_node = NULL;
		}
	} else {
		found_node = NULL;	//reached a leaf node that wasn't the searched node
	}
	return found_node;
}

/**
 * called by findNodeAddr to recursively crawl the edges
 * p_edge: the current edge
 * addr: the rimeaddr used to search for the node
 * current_k: the current depth of the update tree
 * return: pointer to the found node, if nothing was found NULL
 */
p_node_t* findNodeAddr_Edge(p_edge_t * p_edge, rimeaddr_t addr, uint8_t current_k){
	p_node_t *found_node;

	found_node = findNodeAddr_Node(p_edge->drain, addr, current_k);

	if(found_node == NULL){	//still not found? try the other edges
		if(p_edge->next != NULL){
			found_node = findNodeAddr_Edge(p_edge->next, addr, current_k);
		}
	}

	return found_node;
}

/*********************************************************************************/
/**********************Find if Edge already exists********************************/
/*********************************************************************************/

/**
 * find out if an edge between two nodes already exists
 * p_edge: the edge to check
 * source_node: pointer to the source of the edge
 * return: the pointer to the edge in MEMB if it was found, or NULL if it wasn't
 */
p_edge_t* edgeAlreadyExists(p_edge_t * p_edge, p_node_t * source_node){

	//if the source node does not exist yet (it will after the update), then the edge also does not exist yet
	if(source_node != NULL){
		//check if the node even has outgoing edges
		if(source_node->edges != NULL){
			//check if the first edge in the list points to the same drain
			//if(source_node->edges->drain == p_edge->drain){
			if(rimeaddr_cmp(&(source_node->edges->drain->addr), &(p_edge->drain->addr))){
				return source_node->edges;
			} else {
				return edgeAlreadyExists_EdgeList(p_edge, source_node->edges);
			}
		}
	}

	return NULL;
}

/**
 * called by edgeAlreadyExists to examine every edge in the list of the source node
 * p_edge: the edge to check
 * edgelist_elem: the current element of the edgelist of the source node
 * return: the pointer to the edge in MEMB if it was found, or NULL if it wasn't
 */
p_edge_t* edgeAlreadyExists_EdgeList(p_edge_t * p_edge, p_edge_t * edgelist_elem){

	//check if this edge in the list points to the same drain
	//if(edgelist_elem->drain == p_edge->drain){
	if(rimeaddr_cmp(&(edgelist_elem->drain->addr), &(p_edge->drain->addr))){
		return edgelist_elem;
	} else if(edgelist_elem->next != NULL) {	//does the list continue?
		return edgeAlreadyExists_EdgeList(p_edge, edgelist_elem->next);
	} else {
		return NULL;
	}
}

/*********************************************************************************/
/**********************Handle timeouts of the Graph*******************************/
/*********************************************************************************/

/**
 * Iterates the entire Graph and removes edges that timed out. If a node has no incoming edges anymore, it will also be removed and every outgoing edge from it.
 * graph: The graph which will be iterated
 */
void handleGraphTimeout(p_graph_t* graph){
	if(graph->root->edges != NULL){
		handleGraphTimeoutEdge(graph, graph->root, graph->root->edges, 1);
	}
}

/**
 * Iterates the current node. If it is already in depth K, the recursive function stops, otherwise handleGraphTimeoutEdge is called
 * graph: The graph which will be iterated
 * p_node: The current node that is being looked at during the recursive algorithm
 * current_k: the current depth of the recursive algorithm inside the graph
 */
void handleGraphTimeoutNode(p_graph_t* graph, p_node_t* p_node, uint8_t current_k){
	if(current_k < K){
		if(p_node->edges != NULL){
			handleGraphTimeoutEdge(graph, p_node, p_node->edges, current_k+1);
		}
	}
}

/**
 * Iterates the current edge. If the Edge timed out removeEdgeFromEdgelist is called, otherwise just the rest of the Graph will be iterated.
 * graph: The graph which will be iterated
 * source_node: The source node of p_edge
 * p_edge: The current edge that is being looked at during the recursive algorithm
 * current_k: The current depth of the recursive algorithm inside the graph
 */
void handleGraphTimeoutEdge(p_graph_t* graph, p_node_t* source_node, p_edge_t* p_edge, uint8_t current_k){
	//maybe it's a good idea to temporarily save the edge in a new malloc since it gets freed, but the temporary copy can only be freed after recursively going into the tree -> possibly a lot of temporary edges -> memory overflow
	//therefore I decided against allocating new memory. handleGraphTimeout shouldn't allocate new memory anyways, so it doesn't destroy the information and after handleGraphTimeout is finished, it doesn't matter anymore. Problems with protothreads, though?

	//TODO: fix overflow of the ttl for long times
	if(p_edge->ttl <= clocktimeAsMinutes()){
		//remove the edge from the edgelist if it timed out
		removeEdgeFromEdgelist(graph, source_node, p_edge, current_k);
		//if there are more edges, handle them
		if(p_edge->next != NULL){
			handleGraphTimeoutEdge(graph, source_node, p_edge->next, current_k);
		}
	} else {
		//keep the edge and just iterate the graph
		//if there are more edges, handle them
		if(p_edge->next != NULL){
			handleGraphTimeoutEdge(graph, source_node, p_edge->next, current_k);
		}
		handleGraphTimeoutNode(graph, p_edge->drain, current_k);
	}
}

/**
 * Removes and frees the edge p_edge from the Edgelist of the source node and calls removeNodeWithoutIncomingEdge for the drain node of the Edge, if there aren't any other Edges to reach the Node from root
 * graph: The graph which will be iterated
 * source_node: The source node of p_edge
 * p_edge: The current edge that is being looked at during the recursive algorithm
 * current_k: The current depth of the recursive algorithm inside the graph
 */
void removeEdgeFromEdgelist(p_graph_t* graph, p_node_t* source_node, p_edge_t* p_edge, uint8_t current_k){
	//remove the edge from the nodes edgelist
	if(source_node->edges != NULL){
		//check if the first edge of the edgelist is already the edge that should be removed
		if(source_node->edges == p_edge){
			//handle the freeing of the edge directly
			source_node->edges = p_edge->next;
	        free_edge(p_edge, &(graph->num_edges));
		} else {
			//handle the freeing of the edge recursively
			removeEdgeFromEdgelist_Recursive(graph, source_node->edges, p_edge);
		}
	}

	//check if the Node can't be found in the Graph anymore. If it can't, remove it
	if(findNodeByAddr(graph->root, p_edge->drain->addr) == NULL){
		removeNodeWithoutIncomingEdge(graph, p_edge->drain, current_k);
	}
}

/**
 * Removes and frees the edge p_edge from the Edgelist
 * graph: The graph which will be iterated
 * previous_edge: the edge in the edgelist that is just before p_edge
 * p_edge: the edge to be removed
 */
void removeEdgeFromEdgelist_Recursive(p_graph_t* graph, p_edge_t* previous_edge, p_edge_t* p_edge){
	if(previous_edge->next != NULL){
		if(previous_edge->next == p_edge){
			//found the edge, so remove it from the list
			previous_edge->next = p_edge->next;
	        free_edge(p_edge, &(graph->num_edges));

		} else {
			//didn't find the edge, so search the rest of the list
			removeEdgeFromEdgelist_Recursive(graph, previous_edge->next, p_edge);
		}
	}
}

/**
 * If a node has no incoming edges it should be freed and removeEdgeFromEdgelist should be called for all outgoing edges
 * graph: The graph which will be iterated
 * p_node: the node to be removed
 * current_k: The current depth of the recursive algorithm inside the graph
 */
void removeNodeWithoutIncomingEdge(p_graph_t* graph, p_node_t* p_node, uint8_t current_k){
	//the root should never be removed
	if(graph->root != p_node){
		//remove all edges
		while(p_node->edges != NULL){
			removeEdgeFromEdgelist(graph, p_node, p_node->edges, current_k+1);
		}
		//free the node
    	free_node(p_node, &(graph->num_nodes));
	}
}

/*********************************************************************************/
/********************************Time function************************************/
/*********************************************************************************/

/**
 * Returns the system time in minutes
 */
uint8_t clocktimeAsMinutes(){
	//TODO: fix overflow of the ttl for long times
	return (uint8_t)(clock_time()/60);
}
