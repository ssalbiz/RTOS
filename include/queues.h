#ifndef QUEUES_H
#define QUEUES_H
#include "global.h"


void mwq_allocate();
void ewq_allocate();

void mq_allocate(message_queue** mq);
int  mq_is_empty(message_queue* mq);
void mq_enqueue(MessageEnvelope* env, message_queue* mq); 
void mq_free(message_queue* mq);


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
  
