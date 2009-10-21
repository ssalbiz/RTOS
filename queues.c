#include "queues.h"


//Queue Allocators
void rpq_allocate() { //NULL heads and tails
  _rpq.pq_head[0] = _rpq.pq_tail[0] = NULL; 
  _rpq.pq_head[1] = _rpq.pq_tail[1] = NULL;
  _rpq.pq_head[2] = _rpq.pq_tail[2] = NULL;
  _rpq.pq_head[3] = _rpq.pq_tail[3] = NULL;
}

void mwq_allocate() { 
  _mwq.pq_head[0] = _mwq.pq_tail[0] = NULL;
  _mwq.pq_head[1] = _mwq.pq_tail[1] = NULL;
  _mwq.pq_head[2] = _mwq.pq_tail[2] = NULL;
  _mwq.pq_head[3] = _mwq.pq_tail[3] = NULL;
}
void ewq_allocate() {
  _ewq.pq_head[0] = _ewq.pq_tail[0] = NULL;
  _ewq.pq_head[1] = _ewq.pq_tail[1] = NULL;
  _ewq.pq_head[2] = _ewq.pq_tail[2] = NULL;
  _ewq.pq_head[3] = _ewq.pq_tail[3] = NULL;
}

void feq_allocate() {
  _feq.head = NULL;
}

void proc_allocate() {
  _process_list.head = NULL;
  _process_list.tail= NULL;
}

//Queue is_empty (if queue head is NULL, no elements, therefore empty)
int feq_is_empty() {
  return (_feq.head == NULL);
}

int rpq_is_empty() {
  return (_rpq.pq_head[0] == NULL && 
  	  _rpq.pq_head[1] == NULL &&
	  _rpq.pq_head[2] == NULL &&
	  _rpq.pq_head[3] == NULL);
}

int proc_is_empty() {
  return (_process_list.head != NULL);
}

int rpq_is_empty_p(int p) {
  return (_rpq.pq_head[p] == NULL);
}


//Queue Enqueues
//add to tail, readjust tail pointer
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

void proc_enqueue(PCB* next) {
  assert(next != NULL);
  next->p_next = NULL;
  if (proc_is_empty()) {
    _process_list.head = next;
    _process_list.tail = _process_list.head;
  } else {
    _process_list.tail->p_next = next;
    _process_list.tail = next;
  }
}


void rpq_enqueue(PCB* q_next) {
  assert(q_next != NULL); 
  q_next->q_next = NULL;
  int priority = q_next->priority;
  if (rpq_is_empty(priority)) {
    _rpq.pq_head[priority] = q_next;
    _rpq.pq_tail[priority] = _rpq.pq_head[priority];
  } else {
    _rpq.pq_tail[priority]->q_next = q_next;
    _rpq.pq_tail[priority] = q_next;
  }
}



//Queue dequeues
//remove head, reset head/tail pointer as necessary
//return NULL on empty list
PCB* rpq_dequeue() {
  PCB* ret;
  int i = 0;
  if (rpq_is_empty()) return NULL;
  for (; i < MIN_PRIORITY; i++) {
    if (!rpq_is_empty(i)) {
      ret = _rpq.pq_head[i];
      _rpq.pq_head[i] = _rpq.pq_head[i]->q_next;
      ret->q_next = NULL;
      if (_rpq.pq_head[i] == NULL)
        _rpq.pq_tail[i] = NULL;
      return ret;
    }
  }
  return NULL;
}

PCB* proc_dequeue() {
  if (proc_is_empty()) { return NULL; }
  PCB* ret  = NULL;
  ret = _process_list.head;
  ret->p_next = NULL;
  _process_list.head = _process_list.head->p_next;
  if (_process_list.head == NULL) 
    _process_list.tail = NULL;
  return ret;
}
  

MessageEnvelope* feq_dequeue() {
  if (feq_is_empty()) { return NULL; }
  MessageEnvelope* deq = _feq.head;
  _feq.head = _feq.head->next;
  if (_feq.head == NULL) 
    _feq.tail = NULL;
  return deq;
}

//Queue peeks
//return head
PCB* rpq_peek() {
  int i = 0;
  for (; i < MIN_PRIORITY; i++) 
    if (!rpq_is_empty(i)) 
      return _rpq.pq_head[i];

  return NULL;
}

PCB* proc_peek() {
  return _process_list.head;
}

MessageEnvelope* feq_peek() {
  return _feq.head;
}


//Queue removes
//return target or NULL of target not contained
//if contained, remove target from queue
//iterate through queue until target is encountered, juggle references
PCB* rpq_remove(PCB* target) {
  int priority = target->priority;
  PCB* t = _rpq.pq_head[priority];
  if (target == _rpq.pq_head[priority]) {
    _rpq.pq_head[priority] = _rpq.pq_head[priority]->q_next;
    return target;
  }
  while (t->q_next != NULL) {
    if (t->q_next == target) {
      if (_rpq.pq_tail[priority] == t->q_next) { 
        _rpq.pq_tail[priority] = t; 
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
PCB* proc_remove(PCB* target) {
  PCB* next = _process_list.head;
  if (target == _process_list.head) {
    _process_list.head = _process_list.head->p_next; 
    return target;
  }
  while (next->p_next != target && next->p_next != NULL) {
    next = next->p_next;
  }
  if (next->p_next == target) {
    if (next->p_next == _process_list.tail) {
      _process_list.tail = next;
      return target;
    }
    next->p_next = (next->p_next)->p_next;
    return target;
  } else { return NULL; }
}


MessageEnvelope* feq_remove(MessageEnvelope* target) {
  MessageEnvelope* next = _feq.head;
  if (target == _feq.head) {
    _feq.head = _feq.head->next; 
    return target;
  }
  while (next->next != target && next->next != NULL) {
    next = next->next;
  }
  if (next->next == target) {
    if (next->next == _feq.tail) {
      _feq.tail = next;
      return target;
    }
    next->next = (next->next)->next;
    return target;
  } else { return NULL; }
}

//Queue deallocators
//called at cleanup
void proc_free() { //safely deallocate PCBs on global process list
  int i = 0;
  PCB* next = _process_list.head;
  PCB* store;
  next = _process_list.head;
  while (next != NULL) { //iterate through list, free resources held
    store = next->p_next;
    //todo: free message queues, envelopes, stack ptr, context
    free(next);
    next = store;
  }
  _process_list.head = NULL;
  _process_list.tail = NULL;
  
}

void feq_free() {
  MessageEnvelope* env;
  while (!feq_is_empty()) {
    env = feq_dequeue();
    free(env);
  }
}
 
