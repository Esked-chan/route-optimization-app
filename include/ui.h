#ifndef UI_H
#define UI_H

#include "graph.h"
#include "pathfinding.h"
#include "render.h"
#include <SDL3/SDL.h>

typedef enum {
  MODE_ADD_NODES,
  MODE_ADD_CONNECTIONS,
  MODE_SELECT_START,
  MODE_SELECT_END,
  MODE_MOVE_NODES,
  MODE_DELETE_NODES
} ToolMode;

typedef struct {
  ToolMode mode;
  Node* selected_node;
  int hovered_node_id;
  Node* connection_start;
  Path current_path;
  bool show_path;
  bool panning;
  bool show_nodes;
} UIState;

void handle_events(SDL_Event* event, Graph* graph, UIState* ui_state, RenderContext* ctx);

#endif // UI_H