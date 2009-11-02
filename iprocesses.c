#include "iprocesses.h"

void timeout_enqueue(MessageEnvelope* env, message_queue* mq) {
return;
}

void timer_service(void) {
  MessageEnvelope* env = NULL;
  do {
    env = K_receive_message();
    if (env != NULL)
      timeout_enqueue(env, _timeout);
  } while (env != NULL);
  ticks++;
  if (ticks %10 == 0) {
    if (!mq_is_empty(_timeout)) {
      _timeout->head->timeout_ticks--;
      if (_timeout->head->timeout_ticks <= 0) {
        env = mq_dequeue(_timeout);
        PCB* ptr = pid_to_PCB(env->sender_pid);
        K_send_message(env->sender_pid, env); //send wakeup message
        ptr->state = READY;
        ppq_enqueue(ptr, _rpq); //wakeup process
      }
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
  //signal handler considered trusted code
  atomic(1);
#ifdef DEBUG
  printf("signal %d received...\n", signal);
  fflush(stdout);
#endif
  interrupted_process = current_process;
  current_process->state = INTERRUPTED;

    switch(signal) {
      case SIGINT: terminate(); break;
      case SIGALRM: current_process = timer_i_process;
                    timer_service();
    		    break;
      default: terminate(); break;
    }
#ifdef DEBUG
  printf("leaving handler\n");
  fflush(stdout);
#endif
  current_process = interrupted_process;
  interrupted_process = NULL;
  current_process->state = EXECUTING;
  atomic(0);
}
