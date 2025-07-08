#include "app.h"

#include "src/julia_set/julia_set.h"
#include "src/constants.h"
#include <stdlib.h>

bool app_init(struct App* app) {
  int pixel_array_size = WIDTH * HEIGHT * COLOR_BYTES_SIZE;
  uint8_t* pixel_array = malloc(pixel_array_size);
  app->pixel_array = pixel_array;
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "couldn't initialize sdl3: %s", SDL_GetError());
    return NULL;
  }
  if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, WIDTH, HEIGHT, SDL_WINDOW_OPENGL, &app->window, &app->renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "couldn't initialize window or renderer: %s", SDL_GetError());
    return NULL;
  }
  return app;
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
  free(app->pixel_array);
  SDL_Quit();
}

void app_run(struct App* app) {
}