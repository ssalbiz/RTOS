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
  MessageEnvelope* env = NULL;
  if (wall_state)
    env = K_request_message_envelope();
  if (++wall_sec >= 60)
    if (++wall_min >= 60) 
      ++wall_hr;
  wall_sec %= 60;
  wall_min %= 60;
  wall_hr  %= 24;
  if (wall_state && env != NULL) {
    sprintf(env->data, "CLOCK:%02d:%02d:%02d\n", wall_hr, wall_min, wall_sec);
    K_send_console_chars(env);
  }
return;
}

void keyboard_service(void) {
  MessageEnvelope* env = NULL;
  mem_buffer* buffer;
  do {
    env = K_receive_message();
    if (env != NULL) {
      buffer = (mem_buffer*) _kbd_mem_ptr;
      //keyboard process responsible for null termination
      if (buffer->flag == MEM_READY) {
        strncpy(env->data, buffer->data, MIN(buffer->length, MESSAGE_SIZE)); //preprocessor hacks
	buffer->flag = MEM_DONE; //done reading
	K_send_message(env->sender_pid, env);
      } else {//this should be always true since the keyboard is only going to send the signal when its ready
        //pretend we never received the envelope. Remove from the send queue and re-enqueue to receive
	mq_remove(env, keyboard_i_process->message_send);
	mq_enqueue(env, keyboard_i_process->message_receive);
      }
    } 
  } while (env != NULL);
}

void crt_service(void) {
  MessageEnvelope *env = NULL;
  mem_buffer* buffer = NULL;
  do {
    env = K_receive_message();
    if (env != NULL) {
      buffer = (mem_buffer*) _crt_mem_ptr;
      if (buffer->flag == MEM_READY) {
        strncpy(buffer->data, env->data, MESSAGE_SIZE);
	buffer->flag = MEM_DONE;
        if (env->sender_pid == timer_i_process->pid) {
	  K_release_message_envelope(env);
        } else {
	  K_send_message(env->sender_pid, env);
        }
      } else { //pretend envelope not received. Wait for next invocation
        mq_remove(env, crt_i_process->message_send);
	mq_enqueue(env, crt_i_process->message_receive);
      }
    }
  } while (env != NULL);
}

    


void signal_handler(int signal) {
  //signal handler considered trusted code
  atomic(1);
#ifdef DEBUG
//  printf("signal %d received...\n", signal);
  fflush(stdout);
#endif
  if (current_process == NULL) { atomic(0); return; }
  interrupted_process = current_process;
  current_process->state = INTERRUPTED;

    switch(signal) {
      case SIGINT: terminate(); break;
      case SIGUSR1: current_process = keyboard_i_process;
      		    keyboard_service();
		    break;
      case SIGUSR2: current_process = crt_i_process;
      		    crt_service();
		    break;
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
