#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/mman.h>
#include "global.h"

int parent_pid, fid, mem_size;
caddr_t mem_ptr;
WINDOW* kbd_win;



void die() {
  kill(parent_pid, SIGINT);
  munmap(mem_ptr, mem_size);
//  delwin(kbd_win);
  endwin();
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
  register_handler(SIGHUP);
}


int main(int argc, char** argv) {
  int row, col, x, y;
  register_handlers();
  sscanf(argv[0], "%d", &parent_pid);
  sscanf(argv[1], "%d", &fid);
  sscanf(argv[2], "%d", &mem_size);
  mem_ptr = mmap((caddr_t)0, mem_size, PROT_READ|PROT_WRITE,
  		 MAP_SHARED, fid, (off_t)0);
  mem_buffer *buffer = (mem_buffer*) mem_ptr;
  char local_buffer[MEMBLOCK_SIZE];
  int index = 0;
  char kbd_in = '\0';
//  initscr();
  //noecho();
//  cbreak();
  getmaxyx(stdscr, row, col);
  getyx(stdscr, y, x);
  kbd_win = derwin(stdscr, 0, 0, LINES-1, 0);
  idlok(kbd_win, TRUE);
  scrollok(kbd_win, TRUE);
  leaveok(kbd_win, TRUE);
//  wprintw(kbd_win, "KBD: %d %d %d\n", parent_pid, mem_size, fid);
//  wprintw(kbd_win, "$:");
//  wrefresh(kbd_win);
  unmask();

  while(1) {
//    kbd_in = getchar();
//    wrefresh(kbd_win);
    kbd_in = wgetch(kbd_win);
//    refresh();
 //   wprintw(kbd_win, "%c", kbd_in);
//    wrefresh(kbd_win);
    //echo back
    if (kbd_in != '\n' && index < MEMBLOCK_SIZE-2) { //need space for null terminator and array offset
      local_buffer[index++] = kbd_in;
    } else {
  //    wprintw(kbd_win, "\n$:");
  //    wrefresh(kbd_win);
      if (kbd_in != '\n' && index < MEMBLOCK_SIZE-3) //newline, null terminator and array offset
        local_buffer[index++] = kbd_in;
      local_buffer[index++] = '\0';
      while (buffer->flag != MEM_DONE);
      //  sleep(1); //1-sec polling
      strcpy(buffer->data, local_buffer);
      buffer->flag = MEM_READY;
      buffer->length = index;
      index = 0;
      kill(parent_pid, SIGUSR1);
    }
  }
return 0;
}

