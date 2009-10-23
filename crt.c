#include<signal.h>
#include<stdio.h>
#include<stdlib.h>

void die() {
  printf("CRT: Received SIGINT, quitting..\n");
  exit(1);
}

void unmask() {
  sigset_t newmask;
  sigemptyset(&newmask);
  sigaddset(&newmask, SIGINT);
  sigaddset(&newmask, SIGUSR1);
  sigaddset(&newmask, SIGUSR2);
  sigaddset(&newmask, SIGALRM);
  sigprocmask(SIG_UNBLOCK, &newmask, NULL);
}

void register_handlers() {
  sigset(SIGINT, die);
}

int parent_pid, fid, mem_size;

int main(int argc, char** argv) {
  register_handlers();
  sscanf(argv[0], "%d", &parent_pid);
  sscanf(argv[1], "%d", &fid);
  sscanf(argv[2], "%d", &mem_size);
  printf("CRT: %d %d %d\n", parent_pid, mem_size, fid);
  unmask();
  while(1) { }
  return 0; 
}
