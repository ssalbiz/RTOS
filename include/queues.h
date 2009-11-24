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
#ifndef QUEUES_H
#define QUEUES_H
#include "global.h"


void mwq_allocate();
void ewq_allocate();

void mq_allocate(message_queue** mq);
int  mq_is_empty(message_queue* mq);
void mq_enqueue(MessageEnvelope* env, message_queue* mq); 
MessageEnvelope* mq_dequeue(message_queue* mq);
MessageEnvelope* mq_remove(MessageEnvelope* env, message_queue* mq);
MessageEnvelope* mq_peek(message_queue* mq);
void mq_free(message_queue** mq);

void trace_allocate(trace_buffer** tq);
int  trace_is_empty(trace_buffer* tq, enum Event type);
void trace_enqueue(MessageEnvelope* env, trace_buffer* tq, enum Event type, int time); 
msg_event* trace_dequeue(trace_buffer* tq, enum Event type);
//note: trace_remove not implemented. Never used, no point
msg_event* trace_peek(trace_buffer* tq, enum Event type);
void trace_free(trace_buffer** tq);

void pq_allocate(process_queue** pq); 
int  pq_is_empty(process_queue* pq); 
void pq_enqueue(PCB* q_next, process_queue* pq);
PCB* pq_dequeue(process_queue* pq);
PCB* pq_peek(process_queue* pq);
PCB* pq_remove(PCB* target, process_queue* pq);
void pq_free(process_queue** pq);


void ppq_allocate(priority_process_queue** ppq); 
int  ppq_is_empty(priority_process_queue* ppq); 
int  ppq_is_empty_p(int p, priority_process_queue* ppq);
void ppq_enqueue(PCB* q_next,priority_process_queue* ppq);
PCB* ppq_dequeue(priority_process_queue* ppq);
PCB* ppq_peek(priority_process_queue* ppq);
PCB* ppq_remove(PCB* target, priority_process_queue* ppq);
int  ppq_free(priority_process_queue* ppq);

void proc_allocate();
PCB* proc_dequeue();
PCB* proc_remove(PCB* target);
void proc_enqueue(PCB* n);
int proc_is_empty();
void proc_free();
#endif
  
