#ifndef _APP_H
#define _APP_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <stdint.h>

struct App {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Surface* surface;
  SDL_Texture* texture;
  uint8_t* pixel_array;
  bool is_running;
};

bool app_init(struct App* app);
void app_free(struct App* app);
void app_events(struct App* app);
void app_draw(struct App* app);
void app_run(struct App* app);

#endif