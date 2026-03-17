#include "counter.h"

#include <SDL3/SDL.h>

void fps_counter_init(struct FPSCounter* counter) {
  counter->freq = SDL_GetPerformanceFrequency();
  counter->start = SDL_GetPerformanceCounter();
  counter->frame_count = 0;
  counter->fps = 0;
}

bool fps_counter_update(struct FPSCounter* counter) {
  uint64_t end = SDL_GetPerformanceCounter();
  counter->frame_count++;
  if (end - counter->start >= counter->freq / 2) {
    counter->fps = (int) (counter->frame_count * counter->freq / (end - counter->start));
    counter->frame_count = 0;
    counter->start = end;
    return true;
  }
  return false;
}
