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
  assert((*tq) != NULL);
  (*tq)->send           = NULL;
  (*tq)->send_tail      = NULL;
  (*tq)->receive        = NULL;
  (*tq)->receive_tail   = NULL;
  (*tq)->send_length    = 0;
  (*tq)->receive_length = 0;
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
    return (tq->send == NULL);
  else 
    return (tq->receive == NULL);
}
  


//Queue Enqueues
//add to tail, readjust tail pointer
//
//

void trace_enqueue(msg_event* msg, trace_buffer* tq) {
  assert(tq != NULL && msg != NULL);
  if (msg->type == SEND) {
    if (tq->send_length < TRACE_LENGTH) { //spec'd at 16
      if (trace_is_empty(tq, msg->type)) {
        tq->send      = msg;
	tq->send_tail = msg;
      } else {
        tq->send_tail->next = msg;
        tq->send_tail = tq->send_tail->next;
      }
      msg->next = NULL;
      tq->send_length++;
    } else {
      msg_event *tmp = trace_dequeue(tq, msg->type);
      free(tmp);
      tq->send_tail->next = msg;
      msg->next = NULL;
    }
  } else {
    if (tq->receive_length < TRACE_LENGTH) { 
      if (trace_is_empty(tq, msg->type)) {
        tq->receive      = msg;
	tq->receive_tail = msg;
      } else {
        tq->receive_tail->next = msg;
        tq->receive_tail = tq->receive_tail->next;
      }
      msg->next = NULL;
      tq->receive_length++;
    } else {
      msg_event *tmp = trace_dequeue(tq, msg->type);
      free(tmp);
      tq->receive_tail->next = msg;
      msg->next = NULL;
    }
  }
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

msg_event* trace_dequeue(trace_buffer* tq, enum Event type) {
  assert(tq != NULL);
  msg_event* tmp;
  if (trace_is_empty(tq, type)) return NULL;
  if (type == SEND) {
    tmp = tq->send;
    tq->send = tq->send->next;
    tq->send_length--;
  } else {
    tmp = tq->receive;
    tq->receive = tq->receive->next;
    tq->receive_length--;
  }
  return tmp;
}


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
  priority_process_queue _ppq = (*ppq);
  int i = 0;
  for (; i < MIN_PRIORITY; i++) 
    if (!ppq_is_empty_p(i, ppq)) 
      return _ppq.pq_head[i];
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
    return tq->send;
  else
    return tq->receive;
}

//Queue removes
//return target or NULL of target not contained
//if contained, remove target from queue
//iterate through queue until target is encountered, juggle references
PCB* ppq_remove(PCB* target, priority_process_queue* ppq) {
  assert(ppq != NULL);
  priority_process_queue _ppq = (*ppq);
  int priority = target->priority;
  PCB* t = _ppq.pq_head[priority];
  if (target == _ppq.pq_head[priority]) {
    _ppq.pq_head[priority] = _ppq.pq_head[priority]->q_next;
    return target;
  }
  while (t->q_next != NULL) {
    if (t->q_next == target) {
      if (_ppq.pq_tail[priority] == t->q_next) { 
        _ppq.pq_tail[priority] = t;
	t->q_next = NULL;
	return target;
      }
      t->q_next = (t->q_next)->q_next;
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
    return target;
  } else {
    return NULL;
  }
}

//Queue deallocators
//called at cleanup
void mq_free(message_queue* mq) {
  assert(mq != NULL);
  MessageEnvelope* env = NULL;
  while (!mq_is_empty(mq)) {
    env = mq_dequeue(mq);
    assert(env != NULL);
    free(env);
  }
  free(mq);
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
    mq_free(next->message_send);
    mq_free(next->message_receive);
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
  //assume all PCBs have been safely deallocated, only free queue memory
  //TODO: make less retarded
  free(ppq);
return 0;
}

void trace_free(trace_buffer** tq) {
  assert (*tq != NULL);
  msg_event* tmp  = (*tq)->send;
  msg_event* next = NULL;
  while (tmp != NULL) {
    next = tmp->next;
    free(tmp);
    tmp = next;
  }
  tmp  = (*tq)->receive;
  next = NULL;
  while (tmp != NULL) {
    next = tmp->next;
    free(tmp);
    tmp = next;
  }
  free(*tq);
  *tq = NULL;
}
   


