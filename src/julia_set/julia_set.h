#ifndef _JULIASET_H
#define _JULIASET_H

#include <stdint.h>
#include <stdlib.h>

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

void julia_set_args_init(struct JuliaSetArgs* julia_set_args);
void julia_set(struct JuliaSetArgs* julia_set_args, uint8_t* pixel_array);

#endif