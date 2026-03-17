#ifndef APP_H
#define APP_H

#include "counter.h"
#include "julia_set.h"
#include "tpool.h"

#include <SDL3/SDL.h>
#include <stdint.h>

struct App {
  struct JuliaSetArgs julia_set_args;
  struct FPSCounter counter;
  struct TPool* tpool;
  uint8_t* pixel_array;
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Texture* texture;
  SDL_Event event;
  bool is_running;
};

bool app_init(struct App* a);
void app_run(struct App* a);
void app_free(struct App* a);

#endif
