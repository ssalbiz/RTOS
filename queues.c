#include "queues.h"
#include "global.h"

void rpq_allocate() {
  _rpq.pq_head[0] = _rpq.pq_tail[0];
  _rpq.pq_head[1] = _rpq.pq_tail[1];
  _rpq.pq_head[2] = _rpq.pq_tail[2];
  _rpq.pq_head[3] = _rpq.pq_tail[3];
}

void mwq_allocate() { }
void ewq_allocate() { }

int feq_is_empty() {
  return (_feq.head == NULL);
}

void feq_enqueue(MessageEnvelope* env) {
  assert(env != NULL);
  if (feq_is_empty()) {
    _feq.head = env;
    _feq.tail = _feq.head;
  } else {
    env->next = NULL;
    _feq.tail->next = env;
    _feq.tail = env;
  }
}

int rpq_is_empty() {
  return (_rpq.pq_head[0] == NULL && 
  	  _rpq.pq_head[1] == NULL &&
	  _rpq.pq_head[2] == NULL &&
	  _rpq.pq_head[3] == NULL);
}

int rpq_is_empty_p(int p) {
  return (_rpq.pq_head[p] == NULL);
}


void rpq_enqueue(PCB* q_next) {
 assert(q_next != NULL); 
 int priority = q_next->priority;
 if (rpq_is_empty(priority)) {
   _rpq.pq_head[priority] = q_next;
   _rpq.pq_tail[priority] = _rpq.pq_head[priority];
 } else {
   q_next->q_next = NULL;
   _rpq.pq_tail[priority]->q_next = q_next;
   _rpq.pq_tail[priority] = q_next;
 }
}

PCB* rpq_dequeue() {
  PCB* ret;
  int i = 0;
  if (rpq_is_empty()) return NULL;
  for (; i < MIN_PRIORITY; i++) {
    if (!rpq_is_empty(i)) {
      ret = _rpq.pq_head[i];
      _rpq.pq_head[i] = _rpq.pq_head[i]->q_next;
      ret->q_next = NULL;
      return ret;
    }
  }
  return NULL;
}

PCB* rpq_peek() {
  int i = 0;
  for (; i < MIN_PRIORITY; i++) 
    if (!rpq_is_empty(i)) 
      return _rpq.pq_head[i];

  return NULL;
}

PCB* rpq_remove(PCB* target) {
  int priority = target->priority;
  PCB* t = _rpq.pq_head[priority];
  if (target == _rpq.pq_head[priority]) {
    _rpq.pq_head[priority] = _rpq.pq_head[priority]->q_next;
  }
  while (t->q_next != NULL) {
    if (t->q_next == target) {
      if (_rpq.pq_tail[priority] == t->q_next) { _rpq.pq_tail[priority] = t; }
      t->q_next = (t->q_next)->q_next;
    }
  }
}

void rpq_free() { //safely deallocate PCBs
  int i = 0;
  PCB* q_next = _rpq.pq_head[i];
  PCB* store;
  for (; i< MIN_PRIORITY; i++) {
    q_next = _rpq.pq_head[i];
    while (q_next != NULL) {
      store = q_next->q_next;
      //todo: free message queues, stack ptr, context
      free(q_next);
      q_next = store;
    }
    _rpq.pq_head[i] = NULL;
    _rpq.pq_tail[i] = NULL;
  }
}
  
