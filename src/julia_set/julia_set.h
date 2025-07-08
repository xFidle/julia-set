#include <stdint.h>
#ifndef _JULIASET_H
#define _JULIASET_H

typedef struct JuliaSetArgs {
  int width;
  int height;
  double real_centre;
  double imag_centre;
  double c_real;
  double c_imag;
  double zoom;
  double radius;
} JuliaSetArgs;

JuliaSetArgs* prepare_start_arguments();
void julia_set(JuliaSetArgs* julia_set_args, uint8_t* pixel_array);

#endif