#include "global.h"

void rpq_allocate() {
  _rpq.pq_head[0] = _rpq.pq_tail[0];
  _rpq.pq_head[1] = _rpq.pq_tail[1];
  _rpq.pq_head[2] = _rpq.pq_tail[2];
  _rpq.pq_head[3] = _rpq.pq_tail[3];
}

void mwq_allocate() { }
void ewq_allocate() { }

void feq_enqueue(MessageEnvelope* env) {
  assert(env != NULL);
  if (feq_is_empty()) {
    feq.head = env;
    feq.tail = feq.head;
  } else {
    env->next = NULL;
    feq.tail->next = env;
    feq.tail = env;
  }
}

void rpq_enqueue(PCB* next) {
 assert(next != NULL); 
 if (rpq_is_empty()) {
   rpq.head = next;
   rpq.tail = rpq.head;
 } else {
   next->next = NULL;
   rpq.tail->next = next;
   rpq.tail = next;
 }
}

PCB* rpq_dequeue() {
  PCB* ret;
  if (rpq_is_empty) return NULL;
  ret = rpq.head;
  rpq.head = rpq.head->next;
  ret->next = NULL;
  return ret;
}

PCB* rpq_peek() {
  return rpq.head;
}

PCB* rpq_dequeue(PCB* target) {
  PCB* t = rpq.head;
  if (target == rpq.head) {
    rpq.head = rpq.head->next;
  }
  while (t->next != NULL) {
    if (t->next == target) {
      if (rpq.tail == t->next) { rpq.tail = t; }
      t->next = (t->next)->next;
    }
  }
}

void rpq_free() {
  PCB* next = rpq.head;
  while (next->next != NULL) {
    
}
  

