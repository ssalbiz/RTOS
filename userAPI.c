#include "userAPI.h"

void set_wall_clock(int hrs, int min, int sec) {
  atomic(1);
  K_set_wall_clock(hrs % 24, min % 60, sec % 60);
  atomic(0);
}

int atomic(int state) { 
  sigset_t newmask;
  if (state && masked != TRUE) {
    masked = TRUE;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);
    sigaddset(&newmask, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &newmask, &rtxmask) != 0) {
          printf("OH NOES\n");
	  return -1;
    } else {
          return 0;
    }
  } else {
    masked = FALSE;
    if (sigprocmask(SIG_SETMASK, &rtxmask, NULL) != 0) 
	return -1;
    else 
	return 0;
  }
}

int terminate() {
  atomic(1);
  K_terminate();
  atomic(0);
return 0;
}

void context_switch(jmp_buf p, jmp_buf n) {
  atomic(1);
  K_context_switch(p, n);
  atomic(0);
}

int send_message(int d_pid, MessageEnvelope* env) {
  atomic(1);
  K_send_message(d_pid, env);
  atomic(0);
return 0;
}

MessageEnvelope* receive_message() {
   atomic(1);
   MessageEnvelope* env = NULL;
   env = K_receive_message();
   atomic(0);
   return env;
}

void release_processor(void) {
  atomic(1);
  K_release_processor();
  atomic(0);
}

int get_trace_buffer(MessageEnvelope* env) {\
  atomic(1);
  int i = 0;
  i = K_get_trace_buffer(env);
  atomic(0);
  return i;
}

MessageEnvelope* request_message_envelope() {
  atomic(1);
  MessageEnvelope* env = NULL;
  env = K_request_message_envelope();
  atomic(0);
  return env;
}

void release_message_envelope(MessageEnvelope* env) {
  atomic(1);
  K_release_message_envelope(env);
  atomic(0);
}

int request_process_status(MessageEnvelope* env) {
  atomic(1);
  K_request_process_status(env);
  atomic(0);
return 0;
}

int change_priority(int new_priority, int target_pid) {
  atomic(1);
  K_change_priority(new_priority, target_pid);
  atomic(0);
return 0;
}

int u_sleep(int timeout, MessageEnvelope* msg) {
  atomic(1);
  int ret = K_request_delay(timeout, WAKEUP, msg);
  msg = K_receive_message();
  MessageEnvelope* tmp = NULL, *tail_ptr = NULL;
  tail_ptr = msg;
  tmp = msg;
  while (msg->type != WAKEUP) { //continue until we get a wakeup code
     tail_ptr->next = K_receive_message();
     msg = tail_ptr->next;
     tail_ptr = tail_ptr->next;
  }
  //put this hacked queue of messages back into the message received queue
  current_process->message_receive->head = tmp;
  current_process->message_receive->tail = tail_ptr;

  atomic(0);
  return ret;
}

int request_delay(int timeout, int wakeup, MessageEnvelope* env) {
  atomic(1);
  int ret = K_request_delay(timeout, wakeup, env);
  atomic(0);
  return ret;
}

int send_console_chars(MessageEnvelope* env) {
  atomic(1);
  int ret = K_send_console_chars(env);
  atomic(0);
  return ret;
}

int get_console_chars(MessageEnvelope* env) {
  atomic(1);
  int ret = K_get_console_chars(env);
  atomic(0);
  return ret;
}
