#include<signal.h>
#include<ncurses.h>
#include<sys/mman.h>
#include "global.h"
#include<string.h>

int parent_pid, fid, mem_size;
caddr_t mem_ptr;

void die() {
  munmap(mem_ptr, mem_size);
//  endwin();			/* End curses mode		  */
  printf("CRT: Received SIGINT, quitting..\n");
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
  printf("CRT: %d %d %d\n", parent_pid, mem_size, fid);
  unmask();
//  initscr();			/* Start curses mode 		  */
//  refresh();			/* Print it on to the real screen */
  mem_ptr = mmap((caddr_t)0, mem_size, PROT_READ|PROT_WRITE,
  		 MAP_SHARED, fid, (off_t)0);
  mem_buffer *buffer = (mem_buffer*) mem_ptr;
  char local_buffer[MEMBLOCK_SIZE];
  local_buffer[0] = '\0';
  unmask();
  while(1) {
    while (buffer->flag != MEM_DONE) {
      sleep(1); //1-sec polling
    }
    strncpy(local_buffer, buffer->data, mem_size); //RTX in charge or null termination
    buffer->length = 0;
    buffer->flag = MEM_READY;
    if (strlen(local_buffer) > 0)
      printf("OUTPUT:%s\n", local_buffer);
    kill(parent_pid, SIGUSR2);
  }
  return 0; 
}
