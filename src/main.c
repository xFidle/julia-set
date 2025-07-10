#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "app/app.h"

int main(void) {
  bool exit_status = EXIT_FAILURE;
  struct App app;
  if (app_init(&app)) {
    app_run(&app);
    exit_status = EXIT_SUCCESS;
  }
  app_free(&app);
  return exit_status;
  // struct RendererContext renderer_context;
  // struct JuliaSetArgs args;
  // julia_set(args, renderer_resources->pixel_array);
  // surface = SDL_CreateSurfaceFrom(WIDTH, HEIGHT, SDL_PIXELFORMAT_RGB24, renderer_resources->pixel_array,
  //                                 WIDTH * COLOR_BYTES_SIZE);
  // if (surface == NULL) {
  //   SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create SDL surface: %s", SDL_GetError());
  //   return -1;
  // }
  // texture = SDL_CreateTextureFromSurface(renderer, surface);
  // if (texture == NULL) {
  //   SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from given surface: %s", SDL_GetError());
  //   return -1;
  // }
  // bool done = false;
  // while (!done) {
  //   SDL_Event event;
  //   while (SDL_PollEvent(&event)) {
  //     if (event.type == SDL_EVENT_QUIT) {
  //       done = true;
  //     }
  //     SDL_RenderClear(renderer);
  //     SDL_RenderTexture(renderer, texture, NULL, NULL);
  //     SDL_RenderPresent(renderer);
  //   }
  // }
  // delete_renderer_resources(renderer_resources);
  // return 0;
}
