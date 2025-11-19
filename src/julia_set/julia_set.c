#include "julia_set.h"
#include "constants.h"

#include <stdlib.h>

struct JuliaSetArgs* julia_set_args_new(void) {
  struct JuliaSetArgs* args = malloc(sizeof(struct JuliaSetArgs));
  if (args == NULL) {
    return NULL;
  }
  julia_set_args_default(args);
  return args;
}

void julia_set_args_default(struct JuliaSetArgs* args) {
  args->width = WIDTH;
  args->height = HEIGHT;
  args->real_centre = START_REAL_CENTRE;
  args->imag_centre = START_IMAG_CENTRE;
  args->c_real = START_C_REAL;
  args->c_imag = START_C_IMAG;
  args->zoom = START_ZOOM;
  args->radius = RADIUS;
}
