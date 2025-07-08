#include "julia_set.h"
#include "constants.h"
#include <stdlib.h>

JuliaSetArgs* prepare_start_arguments() {
  JuliaSetArgs* args;
  args = malloc(sizeof(JuliaSetArgs));
  args->width = WIDTH;
  args->height = HEIGHT;
  args->real_centre = START_REAL_CENTRE;
  args->imag_centre = START_IMAG_CENTRE;
  args->c_real = START_C_REAL;
  args->c_imag = START_C_IMAG;
  args->zoom = START_ZOOM;
  args->radius = RADIUS;
  return args;
}