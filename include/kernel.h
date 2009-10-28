#ifndef KERNEL_H
#define KERNEL_H
#include "queues.h"
#include "global.h"
#include <sys/mman.h>

//kernel data structures
PCB* current_process;
priority_process_queue* _rpq; //global ready process queue
priority_process_queue* _mwq; //global MESSAGE_WAIT process queue
priority_process_queue* _ewq; //global ENVELOPE_WAIT queue
message_queue* _feq; 	      //global free envelope queue
process_queue* _process_list;
PCB* timer_i_process;
PCB* keyboard_i_process;
PCB* crt_i_process; 

jmp_buf kernel_buf;

void K_terminate() ;
void K_context_switch(jmp_buf prev, jmp_buf next) ;
void K_process_switch() ; 
void K_release_processor() ;
void null_process() ;
PCB* pid_to_PCB(int target) ;
void K_cleanup() ;
void K_send_message();

#endif
