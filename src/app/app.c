#include "app.h"
#include "constants.h"
#include "julia_set.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void app_events(struct App* a);
static void app_draw(struct App* a);
static bool allocate_pixel_array(struct App* a);
static bool allocate_julia_set_args(struct App* a);
static bool set_new_texture(struct App* a);

bool app_init(struct App* a) {
  if (!allocate_pixel_array(a) || !allocate_julia_set_args(a)) {
    fprintf(stderr, "Couldn't allocate needed memory for pixel array");
    return false;
  }
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "Couldn't initialize SDL3: %s", SDL_GetError());
    return false;
  }
  if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, WIDTH, HEIGHT, SDL_WINDOW_OPENGL, &a->window, &a->renderer)) {
    fprintf(stderr, "Couldn't initialize window or renderer: %s", SDL_GetError());
    return false;
  }
  a->texture = SDL_CreateTexture(a->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
  if (a->texture == NULL) {
    fprintf(stderr, "Couldn't create SDL3 texture: %s", SDL_GetError());
    return false;
  }
  return true;
}

void app_run(struct App* a) {
  set_new_texture(a);
  while (a->is_running) {
    app_events(a);
    app_draw(a);
  }
}

void app_free(struct App* a) {
  if (a->window != NULL) {
    SDL_DestroyWindow(a->window);
    a->window = NULL;
  }
  if (a->renderer != NULL) {
    SDL_DestroyRenderer(a->renderer);
    a->renderer = NULL;
  }
  if (a->texture != NULL) {
    SDL_DestroyTexture(a->texture);
    a->texture = NULL;
  }
  if (a->pixel_array != NULL) {
    free(a->pixel_array);
    a->pixel_array = NULL;
  }
  if (a->julia_set_args != NULL) {
    free(a->julia_set_args);
    a->julia_set_args = NULL;
  }
  SDL_Quit();
}

void app_events(struct App* a) {
  bool update_required = false;
  while (SDL_PollEvent(&a->event)) {
    switch (a->event.type) {
      case SDL_EVENT_QUIT:
        a->is_running = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        switch (a->event.key.key) {
          case SDLK_ESCAPE:
            a->is_running = false;
            break;
          case SDLK_UP:
          case SDLK_K:
            a->julia_set_args->c_real += 0.005;
            update_required = true;
            break;
          case SDLK_DOWN:
          case SDLK_J:
            a->julia_set_args->c_real -= 0.005;
            update_required = true;
            break;
          case SDLK_RIGHT:
          case SDLK_L:
            a->julia_set_args->c_imag += 0.005;
            update_required = true;
            break;
          case SDLK_LEFT:
          case SDLK_H:
            a->julia_set_args->c_imag -= 0.005;
            update_required = true;
            break;
          case SDLK_D:
            a->julia_set_args->real_centre += 5.0;
            update_required = true;
            break;
          case SDLK_A:
            a->julia_set_args->real_centre -= 5.0;
            update_required = true;
            break;
          case SDLK_W:
            a->julia_set_args->imag_centre += 5.0;
            update_required = true;
            break;
          case SDLK_S:
            a->julia_set_args->imag_centre -= 5.0;
            update_required = true;
            break;
          default:
            break;
        }
      default:
        break;
    }
  }
  if (update_required) { set_new_texture(a); }
}

void app_draw(struct App* a) {
  SDL_RenderClear(a->renderer);
  SDL_RenderTexture(a->renderer, a->texture, NULL, NULL);
  SDL_RenderPresent(a->renderer);
}


bool allocate_pixel_array(struct App* a) {
  int pixel_array_size = WIDTH * HEIGHT * BYTES_PER_COLOR;
  uint8_t* pixel_array = aligned_alloc(8, pixel_array_size);
  if (pixel_array == NULL) { return false; }
  a->pixel_array = pixel_array;
  return true;
}

bool allocate_julia_set_args(struct App* a) {
  struct JuliaSetArgs* args = malloc(sizeof(struct JuliaSetArgs));
  if (args == NULL) { return false; }
  julia_set_args_init(args);
  a->julia_set_args = args;
  return true;
}

struct JuliaSetThreadArgs {
  struct JuliaSetArgs* julia_set_args;
  uint8_t* pixel_array;
  int first_row;
  int last_row;
};

bool set_new_texture(struct App* a) {
  int n_threads = SDL_GetNumLogicalCPUCores();
  pthread_t threads[n_threads];
  struct JuliaSetThreadArgs thread_args[n_threads];
  int rows_per_thread = HEIGHT / n_threads;
  for (int i = 0; i < n_threads; ++i) {
    thread_args[i].julia_set_args = a->julia_set_args;
    thread_args[i].pixel_array = a->pixel_array;
    thread_args[i].first_row = i * rows_per_thread;
    thread_args[i].last_row = (i == n_threads - 1) ? HEIGHT : (i + 1) * rows_per_thread + 1;
    pthread_create(&threads[i], NULL, julia_set, &thread_args[i]);
  }
  for (int i = 0; i < n_threads; ++i) { pthread_join(threads[i], NULL); }
  return SDL_UpdateTexture(a->texture, NULL, a->pixel_array, WIDTH * BYTES_PER_COLOR);
}