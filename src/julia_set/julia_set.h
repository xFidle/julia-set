#ifndef _JULIASET_H
#define _JULIASET_H

#include <stdint.h>

struct JuliaSetArgs {
  int width;
  int height;
  double real_centre;
  double imag_centre;
  double c_real;
  double c_imag;
  double zoom;
  double radius;
};

void julia_set_args_init(struct JuliaSetArgs* args);
void* julia_set(void* julia_set_thread_args);

#endif