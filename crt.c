#include<signal.h>
#include<ncurses.h>
#include<sys/mman.h>
#include "global.h"
#include<string.h>

int parent_pid, fid, mem_size;
caddr_t mem_ptr;
FILE *tr_out;
WINDOW* crt_win;

void die() {
  kill(parent_pid, SIGINT);
  munmap(mem_ptr, mem_size);
  delwin(crt_win);
  endwin();			/* End curses mode		  */
  fclose(tr_out);
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
  register_handler(SIGHUP);
}


int main(int argc, char** argv) {
  int x, y, row, col;
  char *tmp;
  tr_out = fopen("trace.out", "w");
  register_handlers();
  sscanf(argv[0], "%d", &parent_pid);
  sscanf(argv[1], "%d", &fid);
  sscanf(argv[2], "%d", &mem_size);
  unmask();
  initscr(); 			/* Start curses mode 		  */
  noecho();
  cbreak();
  getmaxyx(stdscr,row,col);
  crt_win = derwin(stdscr, 20, 0, LINES-10, 0);
  scrollok(crt_win, TRUE);
  idlok(crt_win, TRUE);
  wprintw(crt_win, "CRT: %d %d %d\n", parent_pid, mem_size, fid);
  refresh();			/* Print it on to the real screen */

  mem_ptr = mmap((caddr_t)0, mem_size, PROT_READ|PROT_WRITE,
  		 MAP_SHARED, fid, (off_t)0);
  mem_buffer *buffer = (mem_buffer*) mem_ptr;
  char local_buffer[MEMBLOCK_SIZE];
  local_buffer[0] = '\0';
  unmask();
  while(1) {
    while (buffer->flag != MEM_DONE) {
    }
    strncpy(local_buffer, buffer->data, mem_size); //RTX in charge or null termination
    tmp = strtok(local_buffer, "\n");
    buffer->length = 0;
    buffer->flag = MEM_READY;
    wrefresh(crt_win);
    getyx(stdscr, y, x);
    while (tmp != NULL) {
      if (strlen(tmp) > 0) {
        if (strstr(tmp, "CLOCK") != NULL) {
          mvwprintw(crt_win, 0, col-strlen(tmp)-1, "%s\n\r", tmp);
	  move(y, x);
        } else {
          wprintw(crt_win, "$:%s\n\r", tmp);
          fprintf(tr_out, "$:%s\n\r", tmp);
          //move(y, 0);
        }
        wrefresh(crt_win);
      }
      tmp = strtok(NULL, "\n");
    }
    wrefresh(crt_win);

    kill(parent_pid, SIGUSR2);
  }
  return 0; 
}
