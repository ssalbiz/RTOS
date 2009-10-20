#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#define MESSAGE_SIZE 256
#define MEMBLOCK_SIZE 256
#define MIN_PRIORITY 3
#define MAX_PRIORITY 0
#define TIMER_INTERVAL 10000
enum States {
  EXECUTING,
  READY,
  MESSAGE_WAIT,
  ENVELOPE_WAIT,
  SLEEP,
  INTERRUPTED
};

enum Priority {
  LOW,
  MIDLOW,
  MID,
  MAX
};

enum bool {
  FALSE,
  TRUE
};

enum msg_type {
  WAKEUP,
  CONSOLE_OUTPUT,
  CONSOLE_INPUT,
  DEFAULT, //for user processes that do not explicitly set a type
};
  

typedef struct PCB {
  int pid; //process id
  enum States state; //process current state
  enum Priority priority; //process priority
  void* stack_ptr; //process stack pointer
  int stack_size; //process stack limit
  struct PCB* q_next; //process queue reference
  struct PCB* p_next; //global process list reference
  void* message_send; //send message queue
  void* message_receieve; // receiving message queue
  enum bool i_process; //is this an i_process? PCB

} PCB;

typedef struct MessageEnvelope {
  struct MessageEnvelope* next;
  int sender_pid, destination_pid;
  char data[MESSAGE_SIZE];
  int timeout_ticks;
  enum msg_type type;
} MessageEnvelope;

typedef struct shared_mem_block {
  enum bool status;
  int size;
  char data[MEMBLOCK_SIZE];
} shared_mem_block;

typedef struct trace_message_event {
  int sender_pid, destination_pid;
  enum msg_type type;
  int timestamp;
} trace_message_event;

typedef struct ready_process_queue {
  PCB* rpq_head[MIN_PRIORITY + 1];
  PCB* rpq_tail[MIN_PRIORITY + 1];
} ready_process_queue;

ready_process_queue _rpq; //global ready process queue
PCB* current_process;

PCB* _process_list;
PCB* timer_i_process;
PCB* keyboard_i_process;
PCB* crt_i_process;

caddr_t _kbd_mem_ptr, _crt_mem_ptr;
int _kbd_pid, _crt_pid;



#endif
