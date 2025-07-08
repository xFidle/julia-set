#include "julia_set.h"
#include "constants.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

JuliaSetArgs* prepare_start_arguments();

int main(int argc, char* argv[]) {
  SDL_Window* window;
  SDL_Renderer* renderer;
  SDL_Surface* surface;
  SDL_Texture* texture;
  int pixel_array_size = WIDTH * HEIGHT * COLOR_BYTES_SIZE;
  uint8_t* pixel_array = malloc(pixel_array_size);
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL3: %s", SDL_GetError());
    return -1;
  }
  if (!SDL_CreateWindowAndRenderer("Interactive Julia Set", WIDTH, HEIGHT, SDL_WINDOW_OPENGL, &window, &renderer)) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize window or renderer: %s", SDL_GetError());
    return -1;
  }
  JuliaSetArgs* args = prepare_start_arguments();
  julia_set(args, pixel_array);
  surface = SDL_CreateSurfaceFrom(WIDTH, HEIGHT, SDL_PIXELFORMAT_RGB24, pixel_array, WIDTH * COLOR_BYTES_SIZE);
  if (surface == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create SDL surface: %s", SDL_GetError());
    return -1;
  }
  texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from given surface: %s", SDL_GetError());
    return -1;
  }
  bool done = false;
  while (!done) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        done = true;
      }
      SDL_RenderClear(renderer);
      SDL_RenderTexture(renderer, texture, NULL, NULL);
      SDL_RenderPresent(renderer);
    }
  }
  SDL_DestroySurface(surface);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  free(pixel_array);
  free(args);
  return 0;
}
