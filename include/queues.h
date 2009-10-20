#ifndef QUEUES_H
#define QUEUES_H
#include "global.h"


void rpq_allocate(); 
void mwq_allocate();
void ewq_allocate();

int feq_is_empty();
void feq_enqueue(MessageEnvelope* env); 

int rpq_is_empty(); 

int rpq_is_empty_p(int p);


void rpq_enqueue(PCB* q_next);
PCB* rpq_dequeue();

PCB* rpq_peek();
PCB* rpq_remove(PCB* target);
#endif
void rpq_free();
  
