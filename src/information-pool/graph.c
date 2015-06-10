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
bool edgeAlreadyExists(p_edge_t * p_edge, p_node_t * source_node);
bool edgeAlreadyExists_EdgeList(p_edge_t * p_edge, p_edge_t * edgelist_elem);


/**
 * Updates Node info from a deserialized graph
 * root: pointer to the root p_node_t
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
 * p_node: pointer to the current node in the deserialized update
 */
void updateNeighbour(p_graph_t* graph, p_node_t * p_node){

	//create a new edge, allocate memory and fill it
	p_edge_t *new_p_edge;
	new_p_edge = new_edge(&(graph->num_edges));
	new_p_edge->drain = p_node;
	new_p_edge->next = NULL;

	if(graph->root->edges == NULL){
		graph->root->edges = new_p_edge;
	} else if(edgeAlreadyExists(new_p_edge, graph->root) == false){
		//if the edge does not already exist...
		updateEdgeList(graph->root->edges, new_p_edge);
	} else {
		//Could be solved more elegantly than allocating space in edge_memory and freeing it immediately again if unneeded
		//reset the timeout of the neighbour node
		//TODO: reset the timeout
		//p_node->last_seen = ??;
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

	//save the better timeout time
	//TODO: implement timeout updates, if the timeout is handled by edges, use handleExistingNode_Edgelist
	/*
	if(old_p_node->last_seen < new_p_node->last_seen){
		old_p_node->last_seen = new_p_node->last_seen;
	}*/

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
	if(edgeAlreadyExists(edgelist_elem_UPDATE, source_node)){
		/**
		 * the edge is already known -> don't allocate memory or touch the known edge list.
		 * However, the edge points to a node that needs to be updated and merged -> call
		 * iterateUpdateEdge. Since the edge already exists, the drain Node also already exists
		 * so no allocation is required and the pointer is already correctly set, so no need for the
		 * return of iterateUpdateEdge.
		 */
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
 * return: true if the edge already exists, false otherwise
 */
bool edgeAlreadyExists(p_edge_t * p_edge, p_node_t * source_node){
	bool eval = false;

	//if the source node does not exist yet (it will after the update), then the edge also does not exist yet
	if(source_node != NULL){
		//check if the node even has outgoing edges
		if(source_node->edges != NULL){
			//check if the first edge in the list points to the same drain
			//if(source_node->edges->drain == p_edge->drain){
			if(rimeaddr_cmp(&(source_node->edges->drain->addr), &(p_edge->drain->addr))){
				eval = true;
			} else {
				eval = edgeAlreadyExists_EdgeList(p_edge, source_node->edges);
			}
		}
	}

	return eval;
}

/**
 * called by edgeAlreadyExists to examine every edge in the list of the source node
 * p_edge: the edge to check
 * edgelist_elem: the current element of the edgelist of the source node
 * return: true if found, false otherwise
 */
bool edgeAlreadyExists_EdgeList(p_edge_t * p_edge, p_edge_t * edgelist_elem){
	bool eval = false;

	//check if this edge in the list points to the same drain
	//if(edgelist_elem->drain == p_edge->drain){
	if(rimeaddr_cmp(&(edgelist_elem->drain->addr), &(p_edge->drain->addr))){
		eval = true;
	} else if(edgelist_elem->next != NULL) {	//does the list continue?
		eval = edgeAlreadyExists_EdgeList(p_edge, edgelist_elem->next);
	}

	return eval;
}
