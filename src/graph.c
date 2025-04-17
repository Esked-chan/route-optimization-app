#include "graph.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void init_graph(Graph* graph) {
  graph->node_count = 0;
  graph->selected_start = -1;
  graph->selected_end = -1;
  for (int i = 0; i < MAX_NODES; i++) {
    graph->nodes[i].id = -1;
    graph->nodes[i].connection_count = 0;
    graph->nodes[i].is_selected = false;
    graph->nodes[i].is_start = false;
    graph->nodes[i].is_end = false;
    graph->nodes[i].street_names = malloc(MAX_CONNECTIONS * sizeof(char*));
    graph->nodes[i].connections = malloc(MAX_CONNECTIONS * sizeof(int));
    graph->nodes[i].connection_lengths = malloc(MAX_CONNECTIONS * sizeof(float));
    graph->nodes[i].connection_friction = malloc(MAX_CONNECTIONS * sizeof(float));
    graph->nodes[i].connection_weights = malloc(MAX_CONNECTIONS * sizeof(float));
    if (
      !graph->nodes[i].connections || 
      !graph->nodes[i].connection_weights ||
      !graph->nodes[i].connection_lengths ||
      !graph->nodes[i].connection_friction ||
      !graph->nodes[i].street_names
    ) {
      SDL_Log("Failed to allocate memory for connections or weights");
      exit(EXIT_FAILURE);
    }
  }
}

void print_graph(const Graph* graph) {
  printf("Graph with %d nodes:\n", graph->node_count);
  printf("Start node: %d\n", graph->selected_start);
  printf("End node: %d\n", graph->selected_end);

  for (int i = 0; i < graph->node_count; i++) {
    const Node* node = &graph->nodes[i];
    printf("Node %d: position=(%.2f, %.2f), connections=[", node->id, node->position.x, node->position.y);
    for (int j = 0; j < node->connection_count; j++) {
      printf("%d (%.2f)", node->connections[j], node->connection_weights[j]);
      if (j < node->connection_count - 1) {
        printf(", ");
      }
    }
    printf("]\n");
  }
}

int add_node(Graph* graph, SDL_FPoint position) {
  if (graph->node_count >= MAX_NODES) {
    return -1;
  }

  Node* node = &graph->nodes[graph->node_count];

  node->position = position;
  node->id = graph->node_count;
  node->connection_count = 0;
  node->is_selected = false;
  node->is_start = false;
  node->is_end = false;
  node->street_names = malloc(MAX_CONNECTIONS * sizeof(char*));
  if (!node->street_names) {
    SDL_Log("Failed to allocate memory for street names");
    return -1;
  }

  return graph->node_count++;
}

bool add_connection(Graph* graph, int from, int to, float distance, float friction, const char* street_name) {
  if (from < 0 || from >= graph->node_count || to < 0 || to >= graph->node_count) {
    SDL_Log("Invalid node IDs: %d, %d", from, to);
    return false;
  }

  Node *from_node = &graph->nodes[from];
  Node *to_node = &graph->nodes[to];

  for (int i = 0; i < from_node->connection_count; i++) {
    if (from_node->connections[i] == to) {
      SDL_Log("Connection already exists from %d to %d", from, to);
      return true;
    }
  }
  
  if (from_node->connection_count >= MAX_CONNECTIONS) {
    SDL_Log("Max connections reached for node %d", from);
    return false;
  }

  if (to_node->connection_count >= MAX_CONNECTIONS) {
    SDL_Log("Max connections reached for node %d", to);
    return false;
  }
  
  float weight = distance * (1 / friction);

  from_node->connections[from_node->connection_count] = to;
  from_node->connection_weights[from_node->connection_count] = weight;
  from_node->connection_lengths[from_node->connection_count] = distance;
  from_node->connection_friction[from_node->connection_count] = friction;
  from_node->street_names[from_node->connection_count] = strdup(street_name);
  from_node->connection_count++;

  to_node->connections[to_node->connection_count] = from;
  to_node->connection_weights[to_node->connection_count] = weight;
  to_node->connection_lengths[to_node->connection_count] = distance;
  to_node->connection_friction[to_node->connection_count] = friction;
  to_node->street_names[to_node->connection_count] = strdup(street_name);
  to_node->connection_count++;

  return true;
}

Node* get_node_from_pos(Graph* graph, SDL_FPoint position) {
  for (int i = 0; i < graph->node_count; i++) {
    Node* node = &graph->nodes[i];
    float dx = node->position.x - position.x;
    float dy = node->position.y - position.y;
    float distance = sqrtf(dx * dx + dy * dy);
    
    if (distance <= NODE_RADIUS - 1.f) {
      return node;
    }
  }

  return NULL;
}

void update_node_pos(Graph* graph, int id, SDL_FPoint position) {
  if (id >= 0 && id < graph->node_count) {
    graph->nodes[id].position = position;
  }
}

void clear_graph(Graph* graph) {
  for (int i = 0; i < graph->node_count; i++) {
    for (int j = 0; j < graph->nodes[i].connection_count; j++) {
      free(graph->nodes[i].street_names[j]);
    }
    free(graph->nodes[i].street_names);
    free(graph->nodes[i].connections);
    free(graph->nodes[i].connection_weights);
    free(graph->nodes[i].connection_lengths);
    free(graph->nodes[i].connection_friction);
  }
  init_graph(graph);
}

void remove_node(Graph* graph, int id) {
  if (id < 0 || id >= graph->node_count) {
    return;
  }

  for (int j = 0; j < graph->nodes[id].connection_count; j++) {
    free(graph->nodes[id].street_names[j]);
  }
  free(graph->nodes[id].street_names); 

  for (int i = id; i < graph->node_count - 1; i++) {
    graph->nodes[i] = graph->nodes[i + 1];
    graph->nodes[i].id = i;

    for (int j = 0; j < graph->node_count; j++) {
      Node* node = &graph->nodes[j];
      for (int k = 0; k < node->connection_count; k++) {
        if (node->connections[k] > id) {
          node->connections[k]--;
        }
      }
    }
  }

  graph->node_count--;

  if (graph->selected_start == id) {
    graph->selected_start = -1;
  } else if (graph->selected_start > id) {
    graph->selected_start--;
  }

  if (graph->selected_end == id) {
    graph->selected_end = -1;
  } else if (graph->selected_end > id) {
    graph->selected_end--;
  }
}

void remove_connection(Graph* graph, int from, int to) {
  if (from < 0 || from >= graph->node_count || to < 0 || to >= graph->node_count) {
    return;
  }

  Node* from_node = &graph->nodes[from];
  Node* to_node = &graph->nodes[to];

  for (int i = 0; i < from_node->connection_count; i++) {
    if (from_node->connections[i] == to) {
      for (int j = i; j < from_node->connection_count - 1; j++) {
        from_node->connections[j] = from_node->connections[j + 1];
        from_node->connection_weights[j] = from_node->connection_weights[j + 1];
      }
      from_node->connection_count--;
      break;
    }
  }

  for (int i = 0; i < to_node->connection_count; i++) {
    if (to_node->connections[i] == from) {
      for (int j = i; j < to_node->connection_count - 1; j++) {
        to_node->connections[j] = to_node->connections[j + 1];
        to_node->connection_weights[j] = to_node->connection_weights[j + 1];
      }
      to_node->connection_count--;
      break;
    }
  }
}