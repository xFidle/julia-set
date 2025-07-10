#include "app.h"
#include "constants.h"
#include "src/julia_set/julia_set.h"
#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool allocate_pixel_array(struct App* app);
static bool allocate_julia_set_args(struct App* app);
static bool set_new_texture(struct App* app);

bool app_init(struct App* app) {
  if (!allocate_pixel_array(app) || !allocate_julia_set_args(app)) {
    fprintf(stderr, "Couldn't allocate needed memory for pixel array");
    return false;
  }
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "Couldn't initialize sdl3: %s", SDL_GetError());
    return false;
  }
  if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, WIDTH, HEIGHT, SDL_WINDOW_OPENGL, &app->window, &app->renderer)) {
    fprintf(stderr, "Couldn't initialize window or renderer: %s", SDL_GetError());
    return false;
  }
  return app;
}

void app_events(struct App* app) {
  while (SDL_PollEvent(&app->event)) {
    switch (app->event.type) {
      case SDL_EVENT_QUIT:
        app->is_running = false;
        break;
    }
  }
}

void app_draw(struct App* app) {
  SDL_RenderClear(app->renderer);
  SDL_RenderTexture(app->renderer, app->texture, NULL, NULL);
  SDL_RenderPresent(app->renderer);
}

void app_run(struct App* app) {
  set_new_texture(app);
  while (app->is_running) {
    app_events(app);
    app_draw(app);
  }
}

void app_free(struct App* app) {
  if (app->window != NULL) {
    SDL_DestroyWindow(app->window);
    app->window = NULL;
  }
  if (app->renderer != NULL) {
    SDL_DestroyRenderer(app->renderer);
    app->renderer = NULL;
  }
  if (app->surface != NULL) {
    SDL_DestroySurface(app->surface);
    app->surface = NULL;
  }
  if (app->texture != NULL) {
    SDL_DestroyTexture(app->texture);
    app->texture = NULL;
  }
  if (app->pixel_array != NULL) {
    free(app->pixel_array);
    app->pixel_array = NULL;
  }
  if (app->args != NULL) {
    free(app->args);
    app->args = NULL;
  }
  SDL_Quit();
}

bool allocate_pixel_array(struct App* app) {
  int pixel_array_size = WIDTH * HEIGHT * COLOR_BYTES_SIZE;
  uint8_t* pixel_array = malloc(pixel_array_size);
  if (pixel_array == NULL) {
    return false;
  }
  app->pixel_array = pixel_array;
  return true;
}

bool allocate_julia_set_args(struct App* app) {
  struct JuliaSetArgs* args = malloc(sizeof(struct JuliaSetArgs));
  if (args == NULL) {
    return false;
  }
  julia_set_args_init(args);
  app->args = args;
  return true;
}

bool set_new_texture(struct App* app) {
  julia_set(app->args, app->pixel_array);
  app->surface =
          SDL_CreateSurfaceFrom(WIDTH, HEIGHT, SDL_PIXELFORMAT_RGB24, app->pixel_array, WIDTH * COLOR_BYTES_SIZE);
  app->texture = SDL_CreateTextureFromSurface(app->renderer, app->surface);
}