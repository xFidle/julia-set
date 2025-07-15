#include <stdbool.h>
#include <stdlib.h>
#include "app/app.h"

int main(void) {
  bool exit_status = EXIT_FAILURE;
  struct App app;
  if (app_init(&app)) {
    app_run(&app);
    exit_status = EXIT_SUCCESS;
  }
  app_free(&app);
  return exit_status;
}
