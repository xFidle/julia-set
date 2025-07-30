#include "app.h"

#include <stdlib.h>

int main(void) {
  int exit_status = EXIT_FAILURE;
  struct App app = {0};
  if (app_init(&app)) {
    app_run(&app);
    exit_status = EXIT_SUCCESS;
  }
  app_free(&app);
  return exit_status;
}
