#ifndef RENDER_H
#define RENDER_H

#include "graph.h"
#include "pathfinding.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
  SDL_Window* window;
  SDL_Renderer* renderer;
  TTF_Font* font;
  int width;
  int height;
  float camera_x;
  float camera_y;
  float zoom;
} RenderContext;

typedef struct {
  SDL_FRect rect;
  const char* text;
  bool active;
  SDL_Color color;
} UIButton;

#define CONNECTION_COLOR (SDL_Color){100, 100, 100, 255}
#define LOW_FRICTION_COLOR (SDL_Color){227, 75, 53, 255}
#define HIGH_FRICTION_COLOR (SDL_Color){92, 183, 68, 255}
#define PATH_COLOR (SDL_Color){65, 182, 245, 255}

void init_render_context(RenderContext* ctx, const char* title, int width, int height);
void cleanup_render_context(RenderContext* ctx);
void render_graph(RenderContext* ctx, const Graph* graph, const Path* path, int hovered_node_id, bool show_nodes);
SDL_FPoint transform_point(RenderContext* ctx, SDL_FPoint point);

#endif // RENDER_H