#include "kernel.h"

void terminate() {
  cleanup();
  exit(0);
}

void context_switch(jmp_buf prev, jmp_buf next) {
  if (setjmp(prev) == 0) {
    longjmp(next, 1);
  } else {
    return;
  }
}

void process_switch() { 
//assumptions: 
//-called by currently executing process (PCB == current),
//-calling process has already enqueued itself into a process queue to re-enter execution at some point
  PCB* next = ppq_dequeue(_rpq);
  PCB* tmp = current_process;
  assert(tmp != NULL);
  next->state = EXECUTING;
  current_process = next; //non-atomic. Will reset?
  context_switch(tmp->context, next->context);
}

void release_processor() {

}

void null_process() {
  while(1) {}
//    release_processor();
//  }
}

void cleanup() {
  //atomic(1);
  printf("RTX: sending signal\n");
  kill(_kbd_pid, SIGINT);
  kill(_crt_pid, SIGINT);
  
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

  if (!pq_is_empty(_process_list)) { 
    pq_free(&_process_list);
    printf("RTX: deallocating global process list\n");
  }
  //since PCBs have all been freed, no need to free more
  ppq_free(_rpq);
  ppq_free(_ewq);
  ppq_free(_mwq);
  if (!mq_is_empty(_feq)) {
    mq_free(_feq);
    printf("RTX: deallocating envelope list\n");
  }

  //atomic(0);
}
