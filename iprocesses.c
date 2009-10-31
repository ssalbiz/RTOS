#include "iprocesses.h"

void timer_service(void) {
  return;
}

void signal_handler(int signal) {
  printf("signal %d received...\n", signal);
  //signal handler considered trusted code
  atomic(1);
  interrupted_process = current_process;
  current_process->state = INTERRUPTED;

  if (setjmp(interrupted) == 0) { //save context of currently executing process
    switch(signal) {
      case SIGINT: K_terminate(); break;
      case SIGALRM: //do i_proc voodoo
                    longjmp(interrupted, 1);
    		    break;
      case SIGUSR1: longjmp(interrupted, 1); 
      		    break;
      case SIGUSR2: longjmp(interrupted, 1);
      		    break;
      default: K_terminate(); break;
    }
  }
#ifdef DEBUG
  printf("leaving signal handler\n");
#endif
  current_process = interrupted_process;
  interrupted_process = NULL;
  assert(current_process != NULL);
  current_process->state = EXECUTING;
  atomic(0);
}
