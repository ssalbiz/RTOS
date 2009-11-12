#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include "global.h"

int parent_pid, fid, mem_size;
caddr_t mem_ptr;


void die() {
  kill(parent_pid, SIGINT);
  munmap(mem_ptr, mem_size);
  printf("KBD: Received SIGINT, quitting..\n");
  exit(1);
}

int register_handler(int signal) {
    struct sigaction sa;
    sa.sa_handler = die;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGUSR2);
    sigaddset(&sa.sa_mask, SIGALRM);
    sa.sa_flags = SA_RESTART; 
    if (sigaction(signal, &sa, NULL) == -1) 
      return -1;
    else 
      return 0;
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
  register_handler(SIGINT);
}


int main(int argc, char** argv) {
  register_handlers();
  sscanf(argv[0], "%d", &parent_pid);
  sscanf(argv[1], "%d", &fid);
  sscanf(argv[2], "%d", &mem_size);
  printf("KBD: %d %d %d\n", parent_pid, mem_size, fid);
  mem_ptr = mmap((caddr_t)0, mem_size, PROT_READ|PROT_WRITE,
  		 MAP_SHARED, fid, (off_t)0);
  mem_buffer *buffer = (mem_buffer*) mem_ptr;
  char local_buffer[MEMBLOCK_SIZE];
  int index = 0;
  char kbd_in = '\0';
  unmask();
  while(1) { 
    kbd_in = getchar();
    //echo back
    if (index < MEMBLOCK_SIZE-1) {
      local_buffer[index++] = kbd_in;
      local_buffer[index++] = '\0';
      if (buffer->flag == MEM_DONE) {
        strcpy(buffer->data, local_buffer);
        buffer->flag = MEM_READY;
        buffer->length = index;
        index = 0;
        kill(parent_pid, SIGUSR1);
      }
    } else {
      while (buffer->flag != MEM_DONE)
        sleep(1);
      strcpy(buffer->data, local_buffer);
      buffer->flag = MEM_READY;
      buffer->length = index;
      index = 0;
      kill(parent_pid, SIGUSR1);
    }
  }
return 0;
}

