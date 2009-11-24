/*
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
 *
 */
#ifndef KERNEL_H
#define KERNEL_H
#include "queues.h"
#include "global.h"
#include "userAPI.h"
#include "iprocesses.h"
#include <sys/mman.h>
#include <sys/wait.h>

//kernel data structures
PCB* current_process;
priority_process_queue* _rpq; //global ready process queue
priority_process_queue* _mwq; //global MESSAGE_WAIT process queue
priority_process_queue* _ewq; //global ENVELOPE_WAIT queue
message_queue* _feq; 	      //global free envelope queue
message_queue* _timeout;      //message timeout queue
process_queue* _process_list;
trace_buffer* _tq;

//signal handlers
PCB* timer_i_process;
PCB* keyboard_i_process;
PCB* crt_i_process; 
PCB* interrupted_process;

jmp_buf kernel_buf;
int ticks, seconds;//absolute time since RTX start

int wall_hr, wall_min, wall_sec, wall_state;
int clock_envelope_state;
MessageEnvelope* clock_envelope; //reserved message envelope for wall clock

void K_terminate() ;
void K_context_switch(jmp_buf prev, jmp_buf next) ;
void K_process_switch() ; 
void K_release_processor() ;
void null_process() ;
PCB* pid_to_PCB(int target) ;
void K_cleanup() ;
void K_send_message(int dpid, MessageEnvelope* env);
MessageEnvelope* K_receive_message();
int K_get_trace_buffer(MessageEnvelope* env);
void K_register_trace(MessageEnvelope* msg, int type);
MessageEnvelope* K_request_message_envelope(void);
void K_release_message_envelope(MessageEnvelope* env);
int K_request_process_status(MessageEnvelope* env);
int K_change_priority(int new_priority, int target_pid);
//TODO: implement
int K_request_delay(int timeout, int wakeup, MessageEnvelope* env);
int K_get_console_chars(MessageEnvelope* env);
int K_send_console_chars(MessageEnvelope* env);
void K_set_wall_clock(int h, int m, int s);
void K_set_wall_clock_state(int s);

#endif
