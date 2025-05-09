#include "ui_panel.h"

#include <stdio.h>

void render_ui_panel(RenderContext* ctx, const UIState* ui_state) {
  SDL_FRect panel = {
    ctx->width - 200, 0,
    200, ctx->height
  };
  SDL_SetRenderDrawColor(ctx->renderer, 50, 50, 60, 255);
  SDL_RenderFillRect(ctx->renderer, &panel);

  if (ctx->font) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* title_surface = TTF_RenderText_Solid(ctx->font, "Graph path finding\0", 0, white);
    SDL_Texture* title_texture = SDL_CreateTextureFromSurface(ctx->renderer, title_surface);
    SDL_FRect title_rect = {
      ctx->width - 200 + (200 - title_surface->w) / 2,
      20,
      title_surface->w, title_surface->h
    };
    SDL_RenderTexture(ctx->renderer, title_texture, NULL, &title_rect);
    SDL_DestroySurface(title_surface);
    SDL_DestroyTexture(title_texture);


    char travel_info_text[200];
    snprintf(travel_info_text, sizeof(travel_info_text), 
      "Travel distance\n%1.f metres\nWeighted distance\n%1.f units (metres)\nTravel time\n%.1f minutes", 
      ui_state->current_path.distance, ui_state->current_path.total_cost, ui_state->current_path.time);
    SDL_Surface* travel_info_surface = TTF_RenderText_Solid_Wrapped(ctx->font, travel_info_text, 0, white, 0);
    SDL_Texture* travel_info_texture = SDL_CreateTextureFromSurface(ctx->renderer, travel_info_surface);
    SDL_FRect travel_info_rect = {
      ctx->width - 200 + (200 - travel_info_surface->w) / 2,
      ctx->height - 100 - travel_info_surface->h,
      travel_info_surface->w, travel_info_surface->h
    };
    SDL_RenderTexture(ctx->renderer, travel_info_texture, NULL, &travel_info_rect);
    SDL_DestroySurface(travel_info_surface);
    SDL_DestroyTexture(travel_info_texture);
  }
}

void render_button(RenderContext* ctx, UIButton* button) {
  if (button->active) {
    SDL_SetRenderDrawColor(ctx->renderer, button->color.r, button->color.g, button->color.b, 255);
  } else {
    SDL_SetRenderDrawColor(ctx->renderer, button->color.r / 2, button->color.g / 2, button->color.b / 2, 150);
  }
  SDL_RenderFillRect(ctx->renderer, &button->rect);

  SDL_SetRenderDrawColor(ctx->renderer, 255, 255, 255, 255);
  SDL_RenderRect(ctx->renderer, &button->rect);

  if (ctx->font) {
    SDL_Color text_color = {255, 255, 255, 255};
    SDL_Surface* text_surface = TTF_RenderText_Blended(ctx->font, button->text, 0, text_color);
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(ctx->renderer, text_surface);
    SDL_FRect text_rect = {
      button->rect.x + (button->rect.w - text_surface->w) / 2,
      button->rect.y + (button->rect.h - text_surface->h) / 2,
      text_surface->w, text_surface->h
    };

    SDL_RenderTexture(ctx->renderer, text_texture, NULL, &text_rect);
    SDL_DestroySurface(text_surface);
    SDL_DestroyTexture(text_texture);
  }
}