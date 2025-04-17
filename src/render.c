#include "render.h"
#include <stdio.h>
#include <math.h>

void init_render_context(RenderContext* ctx, const char* title, int width, int height) {
  ctx->width = width;
  ctx->height = height;
  ctx->camera_x = width / 2;
  ctx->camera_y = height / 2;
  ctx->zoom = 1.0f;

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL_Init Error: %s\n", SDL_GetError());
    exit(1);
  }

  if (!TTF_Init()) {
    SDL_Log("TTF_Init Error: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
  }

  if (!SDL_CreateWindowAndRenderer(title, width, height, SDL_WINDOW_RESIZABLE, &ctx->window, &ctx->renderer)) {
    SDL_Log("SDL_CreateWindowAndRenderer Error: %s\n", SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    exit(1);
  }

  if (!SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND)) {
    SDL_Log("SDL_SetRenderDrawBlendMode Error: %s\n", SDL_GetError());
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    TTF_Quit();
    SDL_Quit();
    exit(1);
  }

  ctx->font = TTF_OpenFont("assets/font.ttf", 24);
  if (!ctx->font) {
    SDL_Log("TTF_OpenFont Error: %s\n", SDL_GetError());
    SDL_DestroyRenderer(ctx->renderer);
    SDL_DestroyWindow(ctx->window);
    TTF_Quit();
    SDL_Quit();
    exit(1);
  }
}

SDL_FPoint transform_point(RenderContext* ctx, SDL_FPoint point) {
  SDL_FPoint transformed_point;
  transformed_point.x = (point.x - ctx->camera_x) * ctx->zoom + ctx->width / 2;
  transformed_point.y = (point.y - ctx->camera_y) * ctx->zoom + ctx->height / 2;
  return transformed_point;
}

void cleanup_render_context(RenderContext* ctx) {
  if (ctx->font) {
    TTF_CloseFont(ctx->font);
  }
  if (ctx->renderer) {
    SDL_DestroyRenderer(ctx->renderer);
  }
  if (ctx->window) {
    SDL_DestroyWindow(ctx->window);
  }
  TTF_Quit();
  SDL_Quit();
}

void draw_thick_line(SDL_Renderer* renderer, float x1, float y1, float x2, float y2, float thickness, SDL_Color color) {
  float dx = x2 - x1;
  float dy = y2 - y1;
  float length = sqrtf(dx * dx + dy * dy);
  
  if (length == 0) return;

  dx /= length;
  dy /= length;

  float px = -dy * thickness / 2;
  float py = dx * thickness / 2;

  SDL_FColor fcolor = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f };

  SDL_Vertex vertices[4] = {
    { {x1 + px, y1 + py}, fcolor, {0, 0} },
    { {x1 - px, y1 - py}, fcolor, {0, 0} },
    { {x2 - px, y2 - py}, fcolor, {0, 0} },
    { {x2 + px, y2 + py}, fcolor, {0, 0} }
  };

  const int indices[6] = { 0, 1, 2, 0, 2, 3 };
  SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
}

void render_graph(RenderContext* ctx, const Graph* graph, const Path* path, int hovered_node_id, bool show_nodes) {
  // Draw connections
  for (int i = 0; i < graph->node_count; i++) {
    const Node* node = &graph->nodes[i];
    for (int j = 0; j < node->connection_count; j++) {
      int connected_id = node->connections[j];

      if (connected_id > i) {
        const Node* connected_node = &graph->nodes[connected_id];
        SDL_Color color = {
          // Interpolate color
          (HIGH_FRICTION_COLOR.r - LOW_FRICTION_COLOR.r) * graph->nodes[i].connection_friction[j] + LOW_FRICTION_COLOR.r,
          (HIGH_FRICTION_COLOR.g - LOW_FRICTION_COLOR.g) * graph->nodes[i].connection_friction[j] + LOW_FRICTION_COLOR.g,
          (HIGH_FRICTION_COLOR.b - LOW_FRICTION_COLOR.b) * graph->nodes[i].connection_friction[j] + LOW_FRICTION_COLOR.b,
          255
        };
        
        SDL_FPoint start = transform_point(ctx, node->position);
        SDL_FPoint end = transform_point(ctx, connected_node->position);

        draw_thick_line(ctx->renderer,
          start.x, start.y,
          end.x, end.y, 3.f, color);
      }
    }
  }
  
  // Draw path
  if (path && path->length > 1) {
    for (int i = 0; i < path->length - 1; i++) {
      const Node* from = &graph->nodes[path->nodes[i]];
      const Node* to = &graph->nodes[path->nodes[i + 1]];

      SDL_FPoint start = transform_point(ctx, from->position);
      SDL_FPoint end = transform_point(ctx, to->position);

      draw_thick_line(ctx->renderer,
        start.x, start.y,
        end.x, end.y, 5.f, PATH_COLOR);
    }
  }

  // Draw nodes
  for (int i = 0; i < graph->node_count; i++) {
    const Node* node = &graph->nodes[i];

    bool to_render = node->is_start || node->is_end || node->is_selected || node->id == hovered_node_id || show_nodes;

    if (to_render) {
      if (node->id == hovered_node_id) {
        SDL_SetRenderDrawColor(ctx->renderer, 0, 0, 200, 255);
      } else if (node->is_end) {
        SDL_SetRenderDrawColor(ctx->renderer, 200, 0, 0, 255);
      } else if (node->is_selected) {
        SDL_SetRenderDrawColor(ctx->renderer, 200, 200, 0, 255);
      } else if (node->is_start) {
        SDL_SetRenderDrawColor(ctx->renderer, 0, 200, 0, 255);
      } else if (show_nodes) {
        SDL_SetRenderDrawColor(ctx->renderer, 10, 10, 50, 50);
      }

      SDL_FPoint transformed_pos = transform_point(ctx, node->position);

      SDL_FRect node_rect = {
        transformed_pos.x - NODE_RADIUS * ctx->zoom,
        transformed_pos.y - NODE_RADIUS * ctx->zoom,
        NODE_RADIUS * 2 * ctx->zoom,
        NODE_RADIUS * 2 * ctx->zoom
      };
      SDL_RenderFillRect(ctx->renderer, &node_rect);

      if (ctx->font) {
        if (node->id == hovered_node_id) {
            SDL_Color text_color = { 30, 30, 30, 255 };
            char combined_street_names[1024] = {0};
            for (int k = 0; node->street_names[k] != NULL; k++) {
              bool is_duplicate = false;
              for (int l = 0; l < k; l++) {
                if (strcmp(node->street_names[k], node->street_names[l]) == 0) {
                  is_duplicate = true;
                  break;
                }
              }
              if (!is_duplicate) {
                strncat(combined_street_names, node->street_names[k], sizeof(combined_street_names) - strlen(combined_street_names) - 1);
                if (node->street_names[k + 1] != NULL) {
                  strncat(combined_street_names, "\n", sizeof(combined_street_names) - strlen(combined_street_names) - 1);
                }
              }
            }
            SDL_Surface* text_surface = TTF_RenderText_Blended_Wrapped(ctx->font, combined_street_names, 0, text_color, 0);
            if (text_surface) {
              SDL_Texture* text_texture = SDL_CreateTextureFromSurface(ctx->renderer, text_surface);
              if (text_texture) {
                int text_width = text_surface->w;
                int text_height = text_surface->h;
                SDL_FRect text_rect = {
                  20,
                  ctx->height - 20 - text_height,
                  text_width,
                  text_height
                };
                SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 200);
                SDL_FRect background_rect = {
                  text_rect.x - 5,
                  text_rect.y - 5,
                  text_rect.w + 10,
                  text_rect.h + 10
                };
                SDL_RenderFillRect(ctx->renderer, &background_rect);
                SDL_RenderTexture(ctx->renderer, text_texture, NULL, &text_rect);
                SDL_DestroyTexture(text_texture);
              }
              SDL_DestroySurface(text_surface);
            }
        }
      }
    }
    
  }
}