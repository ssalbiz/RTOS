#include "iprocesses.h"

void signal_handler(int signal) {
  printf("quitting...\n");
  terminate();
  exit(1);
//  terminate();
}
