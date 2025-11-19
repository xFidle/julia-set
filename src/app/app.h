#ifndef APP_H
#define APP_H

#include "julia_set.h"
#include "tpool.h"

#include <SDL3/SDL.h>
#include <stdint.h>

struct App {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  SDL_Event event;
  uint8_t* pixel_array;
  struct JuliaSetArgs* julia_set_args;
  struct TPool* tpool;
  bool is_running;
};

bool app_init(struct App* a);
void app_run(struct App* a);
void app_free(struct App* a);

#endif
