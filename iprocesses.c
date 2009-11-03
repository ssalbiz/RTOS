#include "iprocesses.h"

void timeout_enqueue(MessageEnvelope* env, message_queue* mq) {
  assert(env != NULL && mq != NULL);
  MessageEnvelope* list = mq_peek(mq);
  if (mq_is_empty(mq)) {
    mq_enqueue(env, mq);
    return;
  }
  int tticks = list->timeout_ticks;
  if (env->timeout_ticks < list->timeout_ticks) {
    env->next = mq->head;
    mq->head = env;
    return;
  } else {
    while(list->next != NULL && (tticks+list->next->timeout_ticks) < env->timeout_ticks) {
      list = list->next;
      tticks += list->timeout_ticks;
    }
    if (list->next == NULL) {
      mq_enqueue(env, mq);
      env->timeout_ticks -= tticks;
    } else {
      env->next = list->next; //pointer switching to insert env into list
      list->next = env;
      env->timeout_ticks -= tticks;
      tticks = env->timeout_ticks; //decrement rest of the timeouts to relatavize with respect to new insertion
      while(env->next != NULL) {
        env = env->next;
	env->timeout_ticks -= tticks;
      }
    }
  }
return;
}

void timer_service(void) {
  MessageEnvelope* env = NULL;
  do {
    env = K_receive_message();// iprocess is kernel code
    if (env != NULL) 
      timeout_enqueue(env, _timeout);
  } while (env != NULL);
  ticks++;
  if (!mq_is_empty(_timeout)) {
    _timeout->head->timeout_ticks--;
    if ((_timeout->head)->timeout_ticks <= 0) {
        env = mq_dequeue(_timeout);
	env->type = WAKEUP;
        K_send_message(env->sender_pid, env); //send wakeup message
	//wakeup message if it is on the timeout accomplised
    }
  }
  if (ticks %10 == 0) {
    seconds++;
    update_clock();
  }
  return;
}

void update_clock() {
  if (++wall_sec >= 60)
    if (++wall_min >= 60) 
      ++wall_hr;
  wall_sec %= 60;
  wall_min %= 60;
  wall_hr  %= 24;
return;
}

void signal_handler(int signal) {
  //signal handler considered trusted code
  atomic(1);
#ifdef DEBUG
//  printf("signal %d received...\n", signal);
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
//  printf("leaving handler\n");
  fflush(stdout);
#endif
  current_process = interrupted_process;
  interrupted_process = NULL;
  current_process->state = EXECUTING;
  atomic(0);
}
