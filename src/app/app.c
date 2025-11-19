#include "app.h"
#include "constants.h"
#include "julia_set.h"
#include "queue.h"
#include "tpool.h"

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define CHANGE_PARAM_STEP 0.005
#define CHANGE_PAN_STEP 0.025
#define ZOOM_IN_FACTOR 0.95
#define ZOOM_OUT_FACTOR 1.05

void app_events(struct App* a);
void app_handle_key_event(struct App* a, bool* is_update_needed);
void app_draw(struct App* a);
bool app_set_new_texture(struct App* a);
uint8_t* pixel_array_new(void);

bool app_init(struct App* a) {
  a->pixel_array = pixel_array_new();
  a->julia_set_args = julia_set_args_new();
  a->tpool = tpool_new(&(struct TPoolConfig) {
          .n_threads = SDL_GetNumLogicalCPUCores(),
          .queue_size = SDL_GetNumLogicalCPUCores(),
  });
  if (a->pixel_array == NULL || a->julia_set_args == NULL || a->tpool == NULL) {
    fprintf(stderr, "Couldn't allocate memory for needed structs (tpool, pixel_array, julia_set_args)");
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
  if (!SDL_SetRenderVSync(a->renderer, SDL_RENDERER_VSYNC_ADAPTIVE)) {
    fprintf(stderr, "Couldn't set VSync: %s", SDL_GetError());
    return false;
  }
  a->texture = SDL_CreateTexture(a->renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
  if (a->texture == NULL) {
    fprintf(stderr, "Couldn't create SDL3 texture: %s", SDL_GetError());
    return false;
  }
  a->is_running = true;
  return true;
}

void app_run(struct App* a) {
  app_set_new_texture(a);
  while (a->is_running) {
    app_events(a);
    app_draw(a);
  }
}

void app_free(struct App* a) {
  if (a->window != NULL) {
    SDL_DestroyWindow(a->window);
  }
  if (a->renderer != NULL) {
    SDL_DestroyRenderer(a->renderer);
  }
  if (a->texture != NULL) {
    SDL_DestroyTexture(a->texture);
  }
  if (a->pixel_array != NULL) {
    free(a->pixel_array);
  }
  if (a->julia_set_args != NULL) {
    free(a->julia_set_args);
  }
  if (a->tpool != NULL) {
    tpool_free(a->tpool);
  }
  SDL_Quit();
}

void app_events(struct App* a) {
  bool is_update_needed = false;
  while (SDL_PollEvent(&a->event)) {
    switch (a->event.type) {
      case SDL_EVENT_QUIT:
        a->is_running = false;
        is_update_needed = false;
        break;
      case SDL_EVENT_KEY_DOWN:
        app_handle_key_event(a, &is_update_needed);
        break;
      default:
        break;
    }
  }
  if (is_update_needed) {
    app_set_new_texture(a);
  }
}

void app_handle_key_event(struct App* a, bool* is_update_needed) {
  *is_update_needed = true;
  switch (a->event.key.key) {
    case SDLK_ESCAPE:
      a->is_running = false;
      *is_update_needed = false;
      break;
    case SDLK_UP:
    case SDLK_K:
      a->julia_set_args->c_real += CHANGE_PARAM_STEP;
      break;
    case SDLK_DOWN:
    case SDLK_J:
      a->julia_set_args->c_real -= CHANGE_PARAM_STEP;
      break;
    case SDLK_RIGHT:
    case SDLK_L:
      a->julia_set_args->c_imag += CHANGE_PARAM_STEP;
      break;
    case SDLK_LEFT:
    case SDLK_H:
      a->julia_set_args->c_imag -= CHANGE_PARAM_STEP;
      break;
    case SDLK_D:
      a->julia_set_args->real_centre += CHANGE_PAN_STEP * a->julia_set_args->zoom;
      break;
    case SDLK_A:
      a->julia_set_args->real_centre -= CHANGE_PAN_STEP * a->julia_set_args->zoom;
      break;
    case SDLK_W:
      a->julia_set_args->imag_centre += CHANGE_PAN_STEP * a->julia_set_args->zoom;
      break;
    case SDLK_S:
      a->julia_set_args->imag_centre -= CHANGE_PAN_STEP * a->julia_set_args->zoom;
      break;
    case SDLK_Z:
      a->julia_set_args->zoom *= ZOOM_IN_FACTOR;
      break;
    case SDLK_X:
      a->julia_set_args->zoom *= ZOOM_OUT_FACTOR;
      break;
    case SDLK_R:
      julia_set_args_default(a->julia_set_args);
      break;
    default:
      break;
  }
}

void app_draw(struct App* a) {
  SDL_RenderClear(a->renderer);
  SDL_RenderTexture(a->renderer, a->texture, NULL, NULL);
  SDL_RenderPresent(a->renderer);
}

uint8_t* pixel_array_new(void) {
  int pixel_array_size = WIDTH * HEIGHT * BYTES_PER_COLOR;
  uint8_t* pixel_array = aligned_alloc(8, pixel_array_size);
  return pixel_array;
}

struct JuliaSetThreadArgs {
  struct JuliaSetArgs* julia_set_args;
  uint8_t* pixel_array;
  int first_row;
  int last_row;
};

void init_julia_set_tasks(struct App* a, struct Task tasks[], struct JuliaSetThreadArgs args[], int n_tasks) {
  int rows_per_thread = HEIGHT / n_tasks;
  for (int i = 0; i < n_tasks; ++i) {
    args[i] = (struct JuliaSetThreadArgs) {
            .julia_set_args = a->julia_set_args,
            .pixel_array = a->pixel_array,
            .first_row = i * rows_per_thread,
            .last_row = (i == n_tasks - 1) ? HEIGHT : (i + 1) * rows_per_thread,
    };
    tasks[i].args = (void*) &args[i];
    tasks[i].function = &julia_set;
    tasks[i].is_finished = false;
    queue_enqueue(a->tpool->task_queue, &tasks[i]);
  }
}

void wait_for_tasks_finish(struct App* a, struct Task tasks[], int n_tasks) {
  bool all_tasks_done = false;
  pthread_mutex_lock(&a->tpool->mutex);
  while (!all_tasks_done) {
    pthread_cond_wait(&a->tpool->cond, &a->tpool->mutex);
    all_tasks_done = true;
    for (int i = 0; i < n_tasks; ++i) {
      if (!tasks[i].is_finished) {
        all_tasks_done = false;
        break;
      }
    }
  }
  pthread_mutex_unlock(&a->tpool->mutex);
}

bool app_set_new_texture(struct App* a) {
  int n_threads = a->tpool->n_threads;
  struct Task tasks[n_threads];
  struct JuliaSetThreadArgs args[n_threads];
  init_julia_set_tasks(a, tasks, args, n_threads);
  wait_for_tasks_finish(a, tasks, n_threads);
  return SDL_UpdateTexture(a->texture, NULL, a->pixel_array, WIDTH * BYTES_PER_COLOR);
}
