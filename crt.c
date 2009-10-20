#include<stdio.h>
#include<signal.h>

void die() {
  printf("CRT: Received SIGINT, quitting..\n");
}

void register_handlers() {
  sigset(SIGINT, die);
}

int main() {
  register_handlers();
  while(1) { }
  return 0; 
}
