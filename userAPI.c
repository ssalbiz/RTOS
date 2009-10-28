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
}
