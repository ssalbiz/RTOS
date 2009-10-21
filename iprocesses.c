#include "iprocesses.h"

void signal_handler(int signal) {
  printf("quitting...\n");
  exit(1);
//  terminate();
}
