#include "app.h"
#include "constants.h"
#include "src/julia_set/julia_set.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static void app_events(struct App* app);
static void app_draw(struct App* app);
static bool allocate_pixel_array(struct App* app);
static bool allocate_julia_set_args(struct App* app);
static bool set_new_texture(struct App* app);

bool app_init(struct App* app) {
  if (!allocate_pixel_array(app) || !allocate_julia_set_args(app)) {
    fprintf(stderr, "Couldn't allocate needed memory for pixel array");
    return false;
  }
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "Couldn't initialize SDL3: %s", SDL_GetError());
    return false;
  }
  if (!SDL_CreateWindowAndRenderer(WINDOW_TITLE, WIDTH, HEIGHT, SDL_WINDOW_OPENGL, &app->window, &app->renderer)) {
    fprintf(stderr, "Couldn't initialize window or renderer: %s", SDL_GetError());
    return false;
  }
  app->texture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
  if (app->texture == NULL) {
    fprintf(stderr, "Couldn't create SDL3 texture: %s", SDL_GetError());
    return false;
  }
  SDL_SetTextureScaleMode(app->texture, SDL_SCALEMODE_NEAREST);
  return true;
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
  if (app->texture != NULL) {
    SDL_DestroyTexture(app->texture);
    app->texture = NULL;
  }
  if (app->pixel_array != NULL) {
    free(app->pixel_array);
    app->pixel_array = NULL;
  }
  if (app->julia_set_args != NULL) {
    free(app->julia_set_args);
    app->julia_set_args = NULL;
  }
  SDL_Quit();
}

void app_events(struct App* app) {
  bool update_required = false;
  while (SDL_PollEvent(&app->event)) {
    switch (app->event.type) {
      case SDL_EVENT_QUIT:
        app->is_running = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        switch (app->event.key.key) {
          case SDLK_ESCAPE:
            app->is_running = false;
            break;
          case SDLK_UP:
          case SDLK_K:
            app->julia_set_args->c_real += 0.005;
            update_required = true;
            break;
          case SDLK_DOWN:
          case SDLK_J:
            app->julia_set_args->c_real -= 0.005;
            update_required = true;
            break;
          case SDLK_RIGHT:
          case SDLK_L:
            app->julia_set_args->c_imag += 0.005;
            update_required = true;
            break;
          case SDLK_LEFT:
          case SDLK_H:
            app->julia_set_args->c_imag -= 0.005;
            update_required = true;
            break;
          case SDLK_D:
            app->julia_set_args->real_centre += 5.0;
            update_required = true;
            break;
          case SDLK_A:
            app->julia_set_args->real_centre -= 5.0;
            update_required = true;
            break;
          case SDLK_W:
            app->julia_set_args->imag_centre += 5.0;
            update_required = true;
            break;
          case SDLK_S:
            app->julia_set_args->imag_centre -= 5.0;
            update_required = true;
            break;
          default:
            break;
        }
      default:
        break;
    }
  }
  if (update_required) { set_new_texture(app); }
}

void app_draw(struct App* app) {
  SDL_RenderClear(app->renderer);
  SDL_RenderTexture(app->renderer, app->texture, NULL, NULL);
  SDL_RenderPresent(app->renderer);
}


bool allocate_pixel_array(struct App* app) {
  int pixel_array_size = WIDTH * HEIGHT * BYTES_PER_COLOR;
  uint8_t* pixel_array = malloc(pixel_array_size);
  if (pixel_array == NULL) { return false; }
  app->pixel_array = pixel_array;
  return true;
}

bool allocate_julia_set_args(struct App* app) {
  struct JuliaSetArgs* args = malloc(sizeof(struct JuliaSetArgs));
  if (args == NULL) { return false; }
  julia_set_args_init(args);
  app->julia_set_args = args;
  return true;
}

struct JuliaSetThreadArgs {
  struct JuliaSetArgs* julia_set_args;
  uint8_t* pixel_array;
  int first_row;
  int last_row;
};

bool set_new_texture(struct App* app) {
  int n_threads = 1;
  pthread_t threads[n_threads];
  struct JuliaSetThreadArgs thread_args[n_threads];
  int rows_per_thread = HEIGHT / n_threads;
  for (int i = 0; i < n_threads; ++i) {
    thread_args[i].julia_set_args = app->julia_set_args;
    thread_args[i].pixel_array = app->pixel_array;
    thread_args[i].first_row = i * rows_per_thread;
    thread_args[i].last_row = (i == n_threads - 1) ? HEIGHT : (i + 1) * rows_per_thread + 1;
    pthread_create(&threads[i], NULL, julia_set, &thread_args[i]);
  }
  for (int i = 0; i < n_threads; ++i) { pthread_join(threads[i], NULL); }
  return SDL_UpdateTexture(app->texture, NULL, app->pixel_array, WIDTH * BYTES_PER_COLOR);
}