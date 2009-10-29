#include "userAPI.h"

int atomic(int state) { 
  sigset_t newmask;
  if (state && !masked) {
    masked = TRUE;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);
    sigaddset(&newmask, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &newmask, &rtxmask) != 0) 
	  return -1;
    else
          return 0;
  } else {
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
}
