#ifndef JULIASET_H
#define JULIASET_H

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
void julia_set_args_default(struct JuliaSetArgs* args);
extern void* julia_set(void* julia_set_thread_args);

#endif
