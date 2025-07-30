#ifndef _JULIASET_H
#define _JULIASET_H

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

struct JuliaSetArgs* julia_set_args_new(void);
void julia_set_args_default(struct JuliaSetArgs* args);
extern void* julia_set(void* julia_set_thread_args);

#endif