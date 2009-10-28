#include "kernel.h"





void K_terminate() {
  K_cleanup();
  exit(0);
}

void K_context_switch(jmp_buf prev, jmp_buf next) {
  if (setjmp(prev) == 0) {
    longjmp(next, 1);
  } else {
    return;
  }
}

void K_process_switch() { 
//assumptions: 
//-called by currently executing process (PCB == current),
//-calling process has already enqueued itself into a process queue to re-enter execution at some point
  PCB* next = ppq_dequeue(_rpq);
  PCB* tmp = current_process;
  assert(tmp != NULL);
  next->state = EXECUTING;
  current_process = next; //non-atomic. Will reset?
  K_context_switch(tmp->context, next->context);
}

void K_release_processor() {
  PCB* current = current_process;
  current->state = READY;
  ppq_enqueue(current, _rpq);
  K_process_switch();
}

void null_process() {
  while(1) {
    K_release_processor(); //trusted kernel process
  }
}


PCB* pid_to_PCB(int target) {
  //re-implement with pid array/hashtable. O(n) lookup is terrible
  PCB* next = pq_peek(_process_list);
  while (next != NULL && next->pid != target)
	next  = next->p_next;
  if (next->pid == target) return next;
  else return NULL;
}

void K_send_message(int dest_pid, MessageEnvelope* env) {
  env->sender_pid = current_process->pid;
  env->destination_pid = dest_pid;
  env->timeout_ticks = -1;
  //assume message text and type are handled elsewhere
  PCB* target = pid_to_PCB(dest_pid);
  mq_enqueue(env, target->message_send);
  //do trace
  if (target->state == MESSAGE_WAIT) {
    target->state == READY;
    ppq_remove(target, _mwq);
    ppq_enqueue(target, _mwq);
  }
}

    
void K_cleanup() {
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

}
