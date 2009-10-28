#include "iprocesses.h"

void signal_handler(int signal) {
  printf("quitting...\n");
  //signal handler considered trusted code
  atomic(1);
  K_terminate();
  atomic(0);
  exit(1);
}
