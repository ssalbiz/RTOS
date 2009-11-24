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
#include "queues.h"


//Queue Allocators
//three types of queues used, priority process queues, FIFO process queues, and FIFO message envelopes
void ppq_allocate(priority_process_queue** ppq) { //NULL heads and tails
  (*ppq) = (priority_process_queue*) malloc(sizeof(priority_process_queue));
  assert((*ppq) != NULL);
  (*ppq)->pq_head[0] = NULL; (*ppq)->pq_tail[0] = NULL; 
  (*ppq)->pq_head[1] = NULL; (*ppq)->pq_tail[1] = NULL;
  (*ppq)->pq_head[2] = NULL; (*ppq)->pq_tail[2] = NULL;
  (*ppq)->pq_head[3] = NULL; (*ppq)->pq_tail[3] = NULL;
}

void mq_allocate(message_queue** mq) {
  (*mq) = (message_queue*) malloc(sizeof(message_queue));
  assert((*mq) != NULL);
  (*mq)->head   = NULL;
  (*mq)->tail   = NULL;
}

void pq_allocate(process_queue** pq) {
  (*pq) = (process_queue*) malloc(sizeof(process_queue));
  assert((*pq) != NULL);
  (*pq)->head = NULL;
  (*pq)->tail = NULL;
}

void trace_allocate(trace_buffer** tq) {
  (*tq) = (trace_buffer*) malloc(sizeof(trace_buffer));
  int c =0;
  assert((*tq) != NULL);
  (*tq)->send_length    = 0;
  (*tq)->receive_length = 0;
  for (; c < TRACE_LENGTH; c++) {
    (*tq)->send[c] = (msg_event*) malloc(sizeof(msg_event));
    (*tq)->receive[c] = (msg_event*) malloc(sizeof(msg_event));
  }
  
}

//Queue is_empty (if queue head is NULL, no elements, therefore empty)
int mq_is_empty(message_queue* mq) {
  assert(mq != NULL);
  return (mq->head == NULL);
}


int ppq_is_empty(priority_process_queue* ppq) {
  assert(ppq != NULL);
  return (ppq->pq_head[0] == NULL && 
  	  ppq->pq_head[1] == NULL &&
	  ppq->pq_head[2] == NULL &&
	  ppq->pq_head[3] == NULL);
}

int pq_is_empty(process_queue* pq) {
  assert(pq != NULL);
  return (pq->head == NULL);
}


int ppq_is_empty_p(int p, priority_process_queue *ppq) {
  assert(ppq != NULL);
  return (ppq->pq_head[p] == NULL);
}

int trace_is_empty(trace_buffer* tq, enum Event type) {
  if (type == SEND) 
    return (tq->send_length <= 0);
  else 
    return (tq->receive_length <= 0);
}
  


//Queue Enqueues
//add to tail, readjust tail pointer
//
//

void trace_enqueue(MessageEnvelope* msg, trace_buffer* tq, enum Event type, int time) {
  assert(tq != NULL && msg != NULL);
  msg->next = NULL;
  msg_event* tmp = NULL;
  if (type == SEND) {
    tmp = tq->send[(tq->send_length++)%16];
  } else {
    tmp = tq->receive[(tq->receive_length++)%16];
  }
  tmp->destination_pid 	= msg->destination_pid;
  tmp->source_pid 	= msg->sender_pid;
  tmp->mtype		= msg->type;
  tmp->type		= type;
  tmp->timestamp	= time;
}

void mq_enqueue(MessageEnvelope* env, message_queue* mq) {
  assert(env != NULL && mq != NULL);
  env->next = NULL;
  if (mq_is_empty(mq)) {
    mq->head = env;
    mq->tail = mq->head;
  } else {
    (mq->tail)->next = env;
    mq->tail = env;
  }
}

void pq_enqueue(PCB* next, process_queue* pq) {
  assert(pq != NULL && next != NULL);
  next->p_next = NULL;
  if (pq_is_empty(pq)) {
    pq->head = next;
    pq->tail = pq->head;
  } else {
    (pq->tail)->p_next = next;
    pq->tail = next;
  }
}


void ppq_enqueue(PCB* q_next, priority_process_queue* ppq) {
  assert(q_next != NULL && ppq != NULL);
  q_next->q_next = NULL;
  int priority = q_next->priority;
  if (ppq_is_empty_p(priority, ppq)) {
    ppq->pq_head[priority] = q_next;
    ppq->pq_tail[priority] = ppq->pq_head[priority];
  } else {
    ppq->pq_tail[priority]->q_next = q_next;
    ppq->pq_tail[priority] = q_next;
  }
}

//Queue dequeues
//remove head, reset head/tail pointer as necessary
//return NULL on empty list

PCB* ppq_dequeue(priority_process_queue* ppq) {
  assert(ppq != NULL);
  PCB* ret;
  int i = 0;
  if (ppq_is_empty(ppq)) return NULL;
  for (; i < MIN_PRIORITY+1; i++) {
    if (!ppq_is_empty_p(i, ppq)) {
      ret = ppq->pq_head[i];
      ppq->pq_head[i] = (ppq->pq_head[i])->q_next;
      ret->q_next = NULL;
      if (ppq->pq_head[i] == NULL)
        ppq->pq_tail[i] = NULL;
      return ret;
    }
  }
  return NULL;
}

PCB* pq_dequeue(process_queue* pq) {
  if (pq_is_empty(pq)) return NULL;
  PCB* ret = NULL;
  ret = pq->head;
  ret->p_next = NULL;
  pq->head = (pq->head)->p_next;
  if (pq->head == NULL)
    pq->tail = NULL;
  return ret;
}


  
MessageEnvelope* mq_dequeue(message_queue* mq) {
  assert(mq != NULL);
  if (mq_is_empty(mq)) return NULL;
  MessageEnvelope* deq = mq->head;
  mq->head = (mq->head)->next;
  if (mq->head == NULL)
    mq->tail = NULL;
  deq->next = NULL;
  return deq;
}

//Queue peeks
//return head
PCB* ppq_peek(priority_process_queue* ppq) {
  assert(ppq != NULL);
  int i = 0;
  for (; i < MIN_PRIORITY; i++) 
    if (!ppq_is_empty_p(i, ppq)) 
      return ppq->pq_head[i];
  return NULL;
}

PCB* pq_peek(process_queue* pq) {
  assert(pq != NULL);
  return pq->head;
}

MessageEnvelope* mq_peek(message_queue* mq) {
  assert(mq != NULL);
  return mq->head;
}

msg_event* trace_peek(trace_buffer* tq, enum Event type) {
  assert(tq != NULL);
  if (type == SEND) 
    return tq->send[0];
  else
    return tq->receive[0];
}

//Queue removes
//return target or NULL of target not contained
//if contained, remove target from queue
//iterate through queue until target is encountered, juggle references
PCB* ppq_remove(PCB* target, priority_process_queue* ppq) {
  assert(ppq != NULL);
  int priority = target->priority;
  PCB* t = ppq->pq_head[priority];
  if (target == ppq->pq_head[priority]) {
    ppq->pq_head[priority] = ppq->pq_head[priority]->q_next;
    target->q_next = NULL;
    return target;
  }
  while (t->q_next != NULL) {
    if (t->q_next == target) {
      if (ppq->pq_tail[priority] == t->q_next) { 
        ppq->pq_tail[priority] = t;
	t->q_next = NULL;
	return target;
      }
      t->q_next = (t->q_next)->q_next;
      t->q_next = NULL;
      return target;
    } else {
      t = t->q_next;
    }
  }

  return NULL;
}

PCB* pq_remove(PCB* target, process_queue* pq) {
  assert(target != NULL && pq != NULL);
  PCB* next = pq->head;
  if (target == pq->head) {
    pq->head = (pq->head)->p_next;
    return target;
  }
  while (next->p_next != target && next->p_next != NULL) {
    next = next->p_next;
  }
  if (next->p_next == target) {
    if (next->p_next == pq->tail) {
      pq->tail = next;
      next->p_next = NULL;
      return target;
    }
    next->p_next = (next->p_next)->p_next;
    next->p_next = NULL;
    return target;
  } else { return NULL; }
}

MessageEnvelope* mq_remove(MessageEnvelope* target, message_queue* mq) {
  assert(target != NULL && mq != NULL);
  MessageEnvelope* next = mq->head;
  if (mq_is_empty(mq)) return NULL;
  if (target == mq->head) {
    mq->head = (mq->head)->next;
    return target;
  }
  while (next->next != target && next->next != NULL) {
    next = next->next;
  }
  if (next->next == target) {
    if (next->next == mq->tail) {
      mq->tail = next;
      next->next = NULL;
      return target;
    }
    next->next = (next->next)->next;
    target->next = NULL;
    return target;
  } else {
    return NULL;
  }
}

//Queue deallocators
//called at cleanup
void mq_free(message_queue** mq) {
  assert(mq != NULL && *mq != NULL);
  MessageEnvelope* env = NULL;
  while (!mq_is_empty(*mq)) {
    env = mq_dequeue(*mq);
    assert(env != NULL);
    free(env);
  }
  free(*mq);
}


void pq_free(process_queue** pq) {
  assert((*pq) != NULL);
  PCB* next = (*pq)->head;
  PCB* store;
  while (next != NULL) {
    store = next->p_next;
    //free stack
    assert(next->stack_head != NULL);
    free(next->stack_head);
    assert(next->message_send != NULL && next->message_receive != NULL);
    mq_free(&(next->message_send));
    mq_free(&(next->message_receive));
    free(next);
    next = store;
  }
  (*pq)->head = NULL;
  (*pq)->tail = NULL;
  free(*pq);
  *pq = NULL;
}
 
int ppq_free(priority_process_queue* ppq) {
  assert(ppq != NULL);
  free(ppq);
return 0;
}

void trace_free(trace_buffer** tq) {
  assert (*tq != NULL);
  int c = 0;
  for (c = 0; c < TRACE_LENGTH; c++) {
    free((*tq)->send[c]);
    free((*tq)->receive[c]);
  }
  free(*tq);
  *tq = NULL;
}
   


