#include "iprocesses.h"

void timeout_enqueue(MessageEnvelope* env, message_queue* mq) {
return;
}

void timer_service(void) {
  MessageEnvelope* env = NULL;
  do {
    env = K_receive_message();
    timeout_enqueue(env, _timeout);
  } while (env != NULL);
  ticks++;
  if (ticks %10 == 0) {
    _timeout->head->timeout_ticks--;
    if (_timeout->head->timeout_ticks <= 0) {
      env = mq_dequeue(_timeout);
      PCB* ptr = pid_to_PCB(env->sender_pid);
      K_send_message(env->sender_pid, env); //send wakeup message
      ptr->state = READY;
      ppq_enqueue(ptr, _rpq); //wakeup process
    }
    seconds++;
    update_clock();
  }
  return;
}

void update_clock() {
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
