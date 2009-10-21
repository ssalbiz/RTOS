#ifndef IPROCESS_H
#define IPROCESS_H
void signal_handler(int signal) {
  printf("quitting...\n");
  exit(1);
//  terminate();
}
#endif
