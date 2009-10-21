#include "kernel.h"

void terminate() {
  cleanup();
  exit(0);
}

void cleanup() {
  //atomic(1);
  printf("RTX: sending signal\n");
  kill(_kbd_pid, SIGINT);
  kill(_crt_pid, SIGINT);
  
  //*
  int stat = 0;
  if (_kbd_mem_ptr != NULL) {
    printf("RTX: unmapping keyboard share\n");
    stat = munmap(_kbd_mem_ptr, MEMBLOCK_SIZE);
    if (DEBUG && stat == -1) {printf("RTX: Error unmapping keyboard share\n");} else {printf("RTX: SUCCESS\n");}
    stat = close(_kbd_fid);
    if (DEBUG && stat == -1) {printf("RTX: Error unmapping keyboard share\n");} else {printf("RTX: SUCCESS\n");}
    stat = unlink(KEYBOARD_FILE);
    if (DEBUG && stat == -1) {printf("RTX: Error unmapping keyboard share\n");} else {printf("RTX: SUCCESS\n");}

  }
  if (_crt_mem_ptr != NULL) {
    printf("RTX: unmapping crt share\n");
    stat = munmap(_crt_mem_ptr, MEMBLOCK_SIZE);
    if (DEBUG && stat == -1) {printf("RTX: Error unmapping crt share\n");} else {printf("RTX: SUCCESS\n");}
    stat = close(_crt_fid);
    if (DEBUG && stat == -1) {printf("RTX: Error unmapping crt share\n");} else {printf("RTX: SUCCESS\n");}
    stat = unlink(CRT_FILE);
    if (DEBUG && stat == -1) {printf("RTX: Error unmapping crt share\n");} else {printf("RTX: SUCCESS\n");}

  }

  //while(_process_list != NULL) {
  //*/
  
  if (!rpq_is_empty()) { 
    proc_free();
    printf("RTX: deallocating global process list\n");
  }
//  if () _mwq_free();
//  if (_ewq != NULL) _ewq_free();
  if (!feq_is_empty()) {
    feq_free();
    printf("RTX: deallocating envelope list\n");
  }

  //atomic(0);
}
