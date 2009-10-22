#include "kernel.h"

void terminate() {
  cleanup();
  exit(0);
}

void release_processor() {

}

void null_process() {
//  while(1) {
//    release_processor();
//  }
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
  
  if (!proc_is_empty()) { 
    proc_free();
    printf("RTX: deallocating global process list\n");
  }
  ppq_free(_rpq);
//  if () _mwq_free();
//  if (_ewq != NULL) _ewq_free();
  if (!mq_is_empty(_feq)) {
    mq_free(_feq);
    printf("RTX: deallocating envelope list\n");
  }

  //atomic(0);
}
