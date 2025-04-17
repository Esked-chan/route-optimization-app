#ifndef GRAPH_H
#define GRAPH_H

#define MAX_NODES 3000
#define MAX_CONNECTIONS 50
#define NODE_RADIUS 5.f

#include <SDL3/SDL.h>

typedef struct {
    SDL_FPoint position;
    int id;
    int connections[MAX_CONNECTIONS];
    int connection_count;
    float connection_weights[MAX_CONNECTIONS];
    bool is_selected;
    bool is_start;
    bool is_end;
    char** street_names;
} Node;

typedef struct {
  Node nodes[MAX_NODES];
  int node_count;
  int selected_start;
  int selected_end;
} Graph;

void init_graph(Graph* graph);
int add_node(Graph* graph, SDL_FPoint position);
bool add_connection(Graph* graph, int from, int to, float weight, const char* street_name);
Node* get_node_from_pos(Graph* graph, SDL_FPoint position);
void update_node_pos(Graph* graph, int id, SDL_FPoint position);
void clear_graph(Graph* graph);
void remove_node(Graph* graph, int id);
void remove_connection(Graph* graph, int from, int to);
void print_graph(const Graph* graph);

#endif // GRAPH_H