#ifndef _APP_H
#define _APP_H

#include "src/julia_set/julia_set.h"
#include <SDL3/SDL.h>
#include <stdint.h>

struct App {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Surface* surface;
  SDL_Texture* texture;
  SDL_Event event;
  uint8_t* pixel_array;
  struct JuliaSetArgs* args;
  bool is_running;
};

bool app_init(struct App* app);
void app_events(struct App* app);
void app_draw(struct App* app);
void app_run(struct App* app);
void app_free(struct App* app);

#endif