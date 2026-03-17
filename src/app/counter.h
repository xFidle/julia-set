#ifndef COUNTER_H
#define COUNTER_H

#include <stdbool.h>
#include <stdint.h>

struct FPSCounter {
  uint64_t freq;
  uint64_t start;
  uint64_t frame_count;
  int fps;
};

void fps_counter_init(struct FPSCounter* counter);
bool fps_counter_update(struct FPSCounter* counter);

#endif
