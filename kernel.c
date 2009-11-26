/* Copyright 2009 Syed S. Albiz
 * This file is part of myRTX.
 *
 * myRTX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * myRTX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with myRTX.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
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
    release_processor(); //trusted kernel process
  }
}

void K_set_wall_clock(int hrs, int min, int sec) {
  wall_hr = hrs % 24;
  wall_min = min % 60;
  wall_sec = sec % 60;
}

void K_set_wall_clock_state(int state) {
  if (state) 
    wall_state = 1;
  else
    wall_state = 0;
}


PCB* pid_to_PCB(int target) {
  //re-implement with pid array/hashtable. O(n) lookup is terrible
  if (target == timer_i_process->pid) return timer_i_process;
  else if (target == keyboard_i_process->pid) return keyboard_i_process;
  else if (target == crt_i_process->pid) return crt_i_process;
        
  PCB* next = pq_peek(_process_list);
  while (next != NULL && next->pid != target)
	next  = next->p_next;
  if (next->pid == target) return next;
  else return NULL;
}

int K_request_process_status(MessageEnvelope* msg) {
  PCB* next = pq_peek(_process_list);
  char tmp_buf[60], state_msg[20];
  sprintf(msg->data, " PROCESS STATUS\n%5s %15s %15s %10s\n-------------------------------------------------\n", "PID", "NAME", "STATE", "PRIORITY");
  while (next != NULL) {
    switch(next->state) {
      case EXECUTING	: strcpy(state_msg, "EXECUTING"); break;
      case READY	: strcpy(state_msg, "READY"); break;
      case MESSAGE_WAIT	: strcpy(state_msg, "MESSAGE_WAIT"); break;
      case ENVELOPE_WAIT: strcpy(state_msg, "ENVELOPE_WAIT"); break;
      case INTERRUPTED	: strcpy(state_msg, "INTERRUPTED"); break;
      default: sprintf(state_msg, "%d", next->state);
    }
    sprintf(tmp_buf, "%5d %15s %15s %10d\n", next->pid, 
				   next->name,
    				   state_msg, 
				   next->priority);
    strcat(msg->data, tmp_buf);
    next = next->p_next;
  }
  
  return 0;  
}

int K_change_priority(int new_priority, int target_pid) {
  PCB* target = pid_to_PCB(target_pid);
  if (new_priority <= MAX_PRIORITY || new_priority > MIN_PRIORITY
  	|| target_pid == timer_i_process->pid
	|| target_pid == keyboard_i_process->pid
	|| target_pid == crt_i_process->pid
	|| target->priority == MAX_PRIORITY) { //max priority reserved, less is neg cannot change system processes
    return -1;
  }
  if (target == current_process) {
    target->priority = new_priority;
    return 0;
  } else if (target == timer_i_process ||
  	     target == keyboard_i_process ||
	     target == crt_i_process) {
    return -1; //Illegal operation!
  } else {
    switch(target->state) {
      case READY: ppq_remove(target, _rpq); target->priority = new_priority; ppq_enqueue(target, _rpq); break;
      case MESSAGE_WAIT: ppq_remove(target, _mwq); target->priority = new_priority; ppq_enqueue(target, _mwq); break;
      case ENVELOPE_WAIT: ppq_remove(target, _ewq); target->priority = new_priority; ppq_enqueue(target, _ewq); break;
      default: break;
    }
  }
  return 0;
}

int K_request_delay(int timeout, int wakeup, MessageEnvelope* env) {
  env->timeout_ticks = timeout;
  env->type = WAKEUP;
  env->sender_pid = current_process->pid;
  K_send_message(timer_i_process->pid, env);
  return 0;
}

int K_send_console_chars(MessageEnvelope* env) {
  assert(env != NULL);
  PCB* interrupted = NULL;
  env->type = CONSOLE_OUTPUT;
  K_send_message(crt_i_process->pid, env);
  //call crt_i_process
  interrupted     = current_process;
  current_process = crt_i_process;
  crt_service();
  //let iprocess write env->data to shared memory
  //return control
  current_process = interrupted;
  return 0;
}

int K_get_console_chars(MessageEnvelope* env) {
  //read from shared memory into env
  assert(env != NULL);
  env->type = CONSOLE_INPUT;
  K_send_message(keyboard_i_process->pid, env);
  return 0;
}

MessageEnvelope* K_request_message_envelope(void) {
  MessageEnvelope *env = NULL;
  if (current_process == timer_i_process && clock_envelope_state) {
    clock_envelope_state = 0;
    return clock_envelope;
  } else if (current_process == timer_i_process) return NULL;
  while (mq_is_empty(_feq)) {
    current_process->state = ENVELOPE_WAIT;
    ppq_enqueue(current_process, _ewq);
    K_process_switch();
  }
  env = mq_dequeue(_feq);
  memset(env->data, '\0', MESSAGE_SIZE);
  mq_enqueue(env, current_process->message_send);
  return env;
}

void K_release_message_envelope(MessageEnvelope* env) {
  assert(env != NULL);
  memset(env->data, '\0', MESSAGE_SIZE);
  mq_remove(env, current_process->message_send);
  mq_remove(env, current_process->message_receive);
  if (env == clock_envelope) { clock_envelope_state = 1; return; }
  mq_enqueue(env, _feq);

  if (!ppq_is_empty(_ewq)) {
    PCB* nextPCB = ppq_dequeue(_ewq);
    nextPCB->state = READY;
    ppq_enqueue(nextPCB, _rpq);
  }
}


void K_register_trace(MessageEnvelope* env, int type) {
  assert(_tq != NULL && env != NULL);
  trace_enqueue(env, _tq, type, seconds);
}

void K_send_message(int dest_pid, MessageEnvelope* env) {
  env->sender_pid = current_process->pid;
  env->destination_pid = dest_pid;
  //assume message text and type are handled elsewhere
  PCB* target = pid_to_PCB(dest_pid);
  //check for message type sanity
  mq_remove(env, current_process->message_send);
  mq_enqueue(env, target->message_receive);
  K_register_trace(env, 0);
  //do trace
  if (target->state == MESSAGE_WAIT) {
    target->state = READY;
    ppq_remove(target, _mwq);
    ppq_enqueue(target, _rpq);
  }
}

MessageEnvelope* K_receive_message(void) {
  PCB* current = current_process;
  MessageEnvelope* env = NULL;
  assert(current != NULL);
  while (mq_is_empty(current->message_receive)) {
    if (current == timer_i_process || 
 	current == keyboard_i_process ||
	current == crt_i_process) return NULL;
    //if no message waiting and not an i_process, block on receive
    current->state = MESSAGE_WAIT;
    ppq_enqueue(current, _mwq);
    K_process_switch();
  }
  env = mq_dequeue(current_process->message_receive);
  //still want the envelope associated with this process, so put it on the send queue
  mq_enqueue(env, current_process->message_send);
  //do trace
  K_register_trace(env, 1);
  return env;
}

int K_get_trace_buffer(MessageEnvelope* env) {
  assert(env != NULL);
  int i = 0;
  int len = MESSAGE_SIZE/32;
  char tmp_buf[len], msg_type[20];
  msg_event* evts;
  memset(env->data, '\0', MESSAGE_SIZE);
  for (i = 0; i < TRACE_LENGTH; i++) {
    evts = _tq->send[(i+_tq->send_length)%16];
    switch(evts->mtype) {
      case WAKEUP: strcpy(msg_type, "TIMER WAKEUP"); break;
      case CONSOLE_INPUT: strcpy(msg_type, "CONSOLE INPUT"); break;
      case CONSOLE_OUTPUT: strcpy(msg_type, "CONSOLE OUTPUT"); break;
      case DEFAULT: strcpy(msg_type, "DEFAULT IPC"); break;
      default: sprintf(msg_type, "%d", evts->mtype); break;
    }
    sprintf(tmp_buf, "SENT:\tTO: %d,\tFR: %d,\tTIME: %d,\tTYPE: %20s\n", 
    		evts->destination_pid, 
		evts->source_pid,
		evts->timestamp,
		msg_type);
  
    strcat(env->data, tmp_buf);
  }
  for( i = 0; i < TRACE_LENGTH; i++) {
    evts = _tq->receive[(i+_tq->receive_length)%16];
    switch(evts->mtype) {
      case WAKEUP: strcpy(msg_type, "TIMER WAKEUP"); break;
      case CONSOLE_INPUT: strcpy(msg_type, "CONSOLE INPUT"); break;
      case CONSOLE_OUTPUT: strcpy(msg_type, "CONSOLE OUTPUT"); break;
      case DEFAULT: strcpy(msg_type, "DEFAULT IPC"); break;
      default: sprintf(msg_type, "%d", evts->mtype); break;
    }
    sprintf(tmp_buf, "RCVD:\tFR: %d,\tTO: %d,\tTIME: %d,\tTYPE: %20s\n", 
    		evts->source_pid, 
		evts->destination_pid,
		evts->timestamp,
		msg_type);
    strcat(env->data, tmp_buf);
  }
  return 0;
}


    
void K_cleanup() {
#ifdef DEBUG
  printf("RTX: sending signal\n");
  sleep(2);
#endif
  kill(_kbd_pid, SIGINT);
  wait(NULL);
  kill(_crt_pid, SIGINT);
  wait(NULL);
  
  int stat = 0;
  if (_kbd_mem_ptr != NULL) {
#ifdef DEBUG
    printf("RTX: unmapping keyboard share\n");
#endif
    stat = munmap(_kbd_mem_ptr, MEMBLOCK_SIZE);
#ifdef DEBUG
    if (stat == -1) {printf("RTX: Error unmapping keyboard share\n");} else {printf("RTX: SUCCESS\n");}
#endif
    stat = close(_kbd_fid);
#ifdef DEBUG
    if (stat == -1) {printf("RTX: Error closing keyboard share file\n");} else {printf("RTX: SUCCESS\n");}
#endif
    stat = unlink(KEYBOARD_FILE);
#ifdef DEBUG
    if (stat == -1) {printf("RTX: Error removing keyboard share file\n");} else {printf("RTX: SUCCESS\n");}
#endif

  }
  if (_crt_mem_ptr != NULL) {
#ifdef DEBUG
    printf("RTX: unmapping crt share\n");
#endif
    stat = munmap(_crt_mem_ptr, MEMBLOCK_SIZE);
#ifdef DEBUG
    if (stat == -1) {printf("RTX: Error unmapping crt share\n");} else {printf("RTX: SUCCESS\n");}
#endif
    stat = close(_crt_fid);
#ifdef DEBUG
    if (stat == -1) {printf("RTX: Error unmapping crt share\n");} else {printf("RTX: SUCCESS\n");}
#endif
    stat = unlink(CRT_FILE);
#ifdef DEBUG
    if (stat == -1) {printf("RTX: Error unmapping crt share\n");} else {printf("RTX: SUCCESS\n");}
#endif
  }

  if (!pq_is_empty(_process_list)) { 
    pq_free(&_process_list);
#ifdef DEBUG
    printf("RTX: deallocating global process list\n");
    printf("RTX: deallocating i_processes\n");
#endif
    //free i_processes
    assert(timer_i_process != NULL);
    assert(keyboard_i_process!= NULL);
    assert(crt_i_process != NULL);
#ifdef DEBUG
    printf("RTX: deallocating i_processes message queues\n");
#endif
    mq_free(&(timer_i_process->message_send));
    mq_free(&(timer_i_process->message_receive));
    mq_free(&(keyboard_i_process->message_send));
    mq_free(&(keyboard_i_process->message_receive));
    mq_free(&(crt_i_process->message_send));
    mq_free(&(crt_i_process->message_receive));
#ifdef DEBUG
    printf("RTX: deallocating i_process stacks\n");
#endif
    free(timer_i_process->stack_head);
    free(crt_i_process->stack_head);
    free(keyboard_i_process->stack_head);
    free(timer_i_process);
    free(crt_i_process);
    free(keyboard_i_process);
  }
  //since PCBs have all been freed, no need to free more
  ppq_free(_rpq);
  ppq_free(_ewq);
  ppq_free(_mwq);
  trace_free(&_tq);
#ifdef DEBUG
    printf("RTX: deallocating envelope list\n");
#endif
  if (!mq_is_empty(_feq)) {
    mq_free(&(_feq));
  } else {
    free(_feq);
  }
  free   (clock_envelope);
#ifdef DEBUG
  printf("RTX: deallocating timeout queue\n");
#endif
  free(_timeout); //all timeouts taken care of old chap.

}
