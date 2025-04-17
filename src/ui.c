#include "ui.h"

#include <math.h>
#include "SDL3/SDL_mouse.h"

void handle_events(SDL_Event *event, Graph* graph, UIState* ui_state, RenderContext* ctx) {
  switch (event->type) {
    case SDL_EVENT_QUIT:
      SDL_Quit();
      exit(0);
      break;

    case SDL_EVENT_KEY_DOWN:
      switch (event->key.key) {
        case SDLK_1:
          ui_state->mode = MODE_ADD_NODES;
          SDL_Log("Mode: Add Nodes");
          break;
        case SDLK_2:
          ui_state->mode = MODE_ADD_CONNECTIONS;
          SDL_Log("Mode: Add Connections");
          break;
        case SDLK_3:
          ui_state->mode = MODE_SELECT_START;
          SDL_Log("Mode: Select Start Node");
          break;
        case SDLK_4:
          ui_state->mode = MODE_SELECT_END;
          SDL_Log("Mode: Select End Node");
          break;
        case SDLK_5:
          ui_state->mode = MODE_MOVE_NODES;
          SDL_Log("Mode: Move Nodes");
          break;
        case SDLK_6:
          ui_state->mode = MODE_DELETE_NODES;
          SDL_Log("Mode: Delete Nodes");
          break;
        case SDLK_P:
          ui_state->show_path = !ui_state->show_path;
          SDL_Log("Show Path: %s", ui_state->show_path ? "ON" : "OFF");
          if (ui_state->show_path) {
            if (!dijkstra(graph, graph->selected_start, graph->selected_end, &ui_state->current_path)) {
              SDL_Log("Failed to find path from %d to %d", graph->selected_start, graph->selected_end);
            }
          }
          break;
        case SDLK_C:
          clear_graph(graph);
          SDL_Log("Graph cleared");
          break;
      }
    break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      if (event->button.button == SDL_BUTTON_LEFT) {
        float mouse_x = event->button.x;
        float mouse_y = event->button.y;

        if (mouse_x > ctx->width - 180 && mouse_x < ctx->width - 20) {
          if (mouse_y >= 120 && mouse_y <= 160) {
            ui_state->show_path = !ui_state->show_path;
            SDL_Log("Show Path: %s", ui_state->show_path ? "ON" : "OFF");
          } else if (mouse_y >= 170 && mouse_y <= 210) {
            ui_state->mode = MODE_SELECT_START;
            SDL_Log("Mode: Select Start Node");
          } else if (mouse_y >= 220 && mouse_y <= 260) {
            ui_state->mode = MODE_SELECT_END;
            SDL_Log("Mode: Select End Node");
          } else if (mouse_y >= 270 && mouse_y <= 310) {
            ui_state->show_nodes = !ui_state->show_nodes;
            SDL_Log("Show Nodes: %s", ui_state->show_nodes ? "ON" : "OFF");
          }
        }

        Node* clicked_node = get_node_from_pos(graph, (SDL_FPoint){mouse_x, mouse_y}, ctx->camera_x, ctx->camera_y, ctx->zoom, ctx->width, ctx->height);
        if (clicked_node) {
          for (int i = 0; i < clicked_node->connection_count; i++) {
            SDL_Log(clicked_node->street_names[i]);
          }
        }

        switch (ui_state->mode) {
          case MODE_ADD_NODES:
            if (!clicked_node) {
              int new_node_id = add_node(graph, (SDL_FPoint){mouse_x, mouse_y});
              SDL_Log("Node added at (%f, %f) with ID %d", mouse_x, mouse_y, new_node_id);
            }
            break;
          
          case MODE_ADD_CONNECTIONS:
            if (clicked_node) {
              if (!ui_state->connection_start) {
                ui_state->connection_start = clicked_node;
                clicked_node->is_selected = true;
              } else if (ui_state->connection_start != clicked_node) {
                float dx = clicked_node->position.x - ui_state->connection_start->position.x;
                float dy = clicked_node->position.y - ui_state->connection_start->position.y;
                float distance = sqrtf(dx * dx + dy * dy);

                float friction = 1.f;

                if (add_connection(graph, ui_state->connection_start->id, clicked_node->id, distance, friction, "MANUAL STREET NAME\0")) {
                  SDL_Log("Connection added from node %d to node %d with weight %f", ui_state->connection_start->id, clicked_node->id, distance);
                } else {
                  SDL_Log("Failed to add connection from node %d to node %d", ui_state->connection_start->id, clicked_node->id);
                }

                ui_state->connection_start->is_selected = false;
                ui_state->connection_start = NULL;
              }
            }
          break;

          case MODE_SELECT_START:
            if (clicked_node) {
              if (graph->selected_start >= 0) {
                graph->nodes[graph->selected_start].is_start = false;
              }
              graph->selected_start = clicked_node->id;
              clicked_node->is_start = true;
              SDL_Log("Start node set to %d", graph->selected_start);

              if (graph->selected_end >= 0 && ui_state->show_path) {
                if (!dijkstra(graph, graph->selected_start, graph->selected_end, &ui_state->current_path))
                  SDL_Log("Failed to find path from %d to %d", graph->selected_start, graph->selected_end);
              }
            }
          break;

          case MODE_SELECT_END:
            if (clicked_node) {
              if (graph->selected_end >= 0) {
                graph->nodes[graph->selected_end].is_end = false;
              }
              graph->selected_end = clicked_node->id;
              clicked_node->is_end = true;
              SDL_Log("End node set to %d", graph->selected_end);

              if (graph->selected_start >= 0 && ui_state->show_path) {
                if (!dijkstra(graph, graph->selected_start, graph->selected_end, &ui_state->current_path))
                  SDL_Log("Failed to find path from %d to %d", graph->selected_start, graph->selected_end);
              }
            }
          break;

          case MODE_MOVE_NODES:
            if (clicked_node) {
              ui_state->selected_node = clicked_node;
              SDL_Log("Node %d selected for moving", clicked_node->id);
            }
          break;

          case MODE_DELETE_NODES:
            if (clicked_node) {
              remove_node(graph, clicked_node->id);
              SDL_Log("Node %d deleted", clicked_node->id);
              if (ui_state->show_path) {
                for (int i = 0; i < ui_state->current_path.length; i++) {
                  if (ui_state->current_path.nodes[i] == clicked_node->id) {
                    ui_state->show_path = false;
                    SDL_Log("Path invalidated due to node deletion");
                    break;
                  }
                }
              }
            }
          break;
        }
      }
      if (event->button.button == SDL_BUTTON_RIGHT) {
        ui_state->panning = true;
        SDL_SetWindowRelativeMouseMode(ctx->window, true);
      }
    break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
      if (event->button.button == SDL_BUTTON_LEFT) {
        if (ui_state->mode == MODE_MOVE_NODES && ui_state->selected_node) {
          ui_state->selected_node->is_selected = false;
          ui_state->selected_node = NULL;
        }

        if (ui_state->show_path && graph->selected_start >= 0 && graph->selected_end >= 0) {
          if (!dijkstra(graph, graph->selected_start, graph->selected_end, &ui_state->current_path))
            SDL_Log("Failed to find path from %d to %d", graph->selected_start, graph->selected_end);
        }
      }
      if (event->button.button == SDL_BUTTON_RIGHT) {
        ui_state->panning = false;
        SDL_SetWindowRelativeMouseMode(ctx->window, false);
      }
    break;

    case SDL_EVENT_MOUSE_MOTION:
      ui_state->hovered_node_id = -1;
      Node* hovered_node = get_node_from_pos(graph, (SDL_FPoint){event->motion.x, event->motion.y}, ctx->camera_x, ctx->camera_y, ctx->zoom, ctx->width, ctx->height);
      if (hovered_node) {
        ui_state->hovered_node_id = hovered_node->id;
      }

      if (ui_state->mode == MODE_MOVE_NODES && ui_state->selected_node) {
        update_node_pos(graph, ui_state->selected_node->id, (SDL_FPoint){event->motion.x, event->motion.y});
        
        Node* node = &graph->nodes[ui_state->selected_node->id];
        for (int i = 0; i < node->connection_count; i++) {
          Node* connected_node = &graph->nodes[node->connections[i]];
          float dx = connected_node->position.x - node->position.x;
          float dy = connected_node->position.y - node->position.y;
          float distance = sqrtf(dx * dx + dy * dy);
          node->connection_weights[i] = distance;
  
          for (int j = 0; j < connected_node->connection_count; j++) {
            if (connected_node->connections[j] == node->id) {
              connected_node->connection_weights[j] = distance;
              break;
            }
          }
        }
      }
      
      if (ui_state->panning) {
        ctx->camera_x -= event->motion.xrel / ctx->zoom;
        ctx->camera_y -= event->motion.yrel / ctx->zoom;
      }
    break;

    case SDL_EVENT_MOUSE_WHEEL:
      float zoom_factor = (event->wheel.y > 0) ? 1.1f : 0.9f;
      float old_zoom = ctx->zoom;
      ctx->zoom *= zoom_factor;

      float mouse_x, mouse_y;
      SDL_GetMouseState(&mouse_x, &mouse_y);

      float world_mouse_x = (mouse_x - ctx->width / 2) / old_zoom + ctx->camera_x;
      float world_mouse_y = (mouse_y - ctx->height / 2) / old_zoom + ctx->camera_y;

      ctx->camera_x = world_mouse_x - (mouse_x - ctx->width / 2) / ctx->zoom;
      ctx->camera_y = world_mouse_y - (mouse_y - ctx->height / 2) / ctx->zoom;

    break;
  }
}