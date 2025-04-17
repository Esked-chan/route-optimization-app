#include "graph.h"
#include "pathfinding.h"
#include "render.h"
#include "ui.h"
#include "import.h"
#include "ui_panel.h"

#include "SDL3/SDL.h"
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>

void signal_handler(int sig) {
  fprintf(stderr, "Error: signal %d\n", sig);
  exit(1);
}

int main(int argc, char* argv[]) {
  printf("Program starting...\n");

  signal(SIGSEGV, signal_handler);
  signal(SIGABRT, signal_handler);
  
  srand((unsigned int)time(NULL));

  RenderContext ctx;
  init_render_context(&ctx, "Graph path finding", 1600, 720);
  printf("Render context initialized.\n");

  Graph graph;
  init_graph(&graph);
  printf("Graph initialized\n");

  UIState ui_state = {
    .mode = MODE_SELECT_START,
    .selected_node = NULL,
    .hovered_node_id = -1,
    .connection_start = NULL,
    .current_path = { .length = 0 },
    .show_path = true
  };

  if (!load_nodes_from_file(&graph, "assets/nodes.csv")) {
    fprintf(stderr, "Failed to load nodes from file\n");
    cleanup_render_context(&ctx);
    return 1;
  }

  if (!load_connections_from_file(&graph, "assets/edges.csv")) {
    fprintf(stderr, "Failed to load connections from file\n");
    cleanup_render_context(&ctx);
    return 1;
  }
  printf("Graph loaded from files.\n");

  printf("Entering main loop...\n");
  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        printf("Quit event received.\n");
        running = false;
      }
      handle_events(&event, &graph, &ui_state, &ctx);
    }

    SDL_SetRenderDrawColor(ctx.renderer, 230, 230, 230, 255);
    SDL_RenderClear(ctx.renderer);

    const Path* path_to_render = ui_state.show_path && ui_state.current_path.length > 0 ? &ui_state.current_path : NULL;
    render_graph(&ctx, &graph, path_to_render, ui_state.hovered_node_id);

    render_ui_panel(&ctx, &ui_state);

    UIButton buttons[] = {
      {{ctx.width - 180, 120, 160, 40}, "Show path\0", ui_state.show_path, {50, 200, 0, 255}},
      {{ctx.width - 180, 170, 160, 40}, "Set start\0", ui_state.mode == MODE_SELECT_START, {200, 50, 50, 255}},
      {{ctx.width - 180, 220, 160, 40}, "Set end\0", ui_state.mode == MODE_SELECT_END, {50, 50, 200, 255}},
    };

    for (int i = 0; i < sizeof(buttons) / sizeof(buttons[0]); i++) {
      render_button(&ctx, &buttons[i]);
    }
    SDL_RenderPresent(ctx.renderer);
    SDL_Delay(16);
  }

  cleanup_render_context(&ctx);
  printf("Program exited normally\n");
  return 0;
}