#include "app.h"

#include "src/julia_set/julia_set.h"
#include "constants.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool allocate_pixel_array(struct App* app, int window_width, int window_height, int color_size);

bool app_init(struct App* app) {
  if (!allocate_pixel_array(app, WIDTH, HEIGHT, COLOR_BYTES_SIZE)) {
    fprintf(stderr, "Couldn't allocate memory for pixel array");
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
  SDL_Quit();
}

void app_run(struct App* app) {
}

bool allocate_pixel_array(struct App* app, int window_width, int window_height, int color_size) {
  int pixel_array_size = WIDTH * HEIGHT * COLOR_BYTES_SIZE;
  uint8_t* pixel_array = malloc(pixel_array_size);
  if (pixel_array == NULL) {
    return false;
  }
  app->pixel_array = pixel_array;
  return true;
}