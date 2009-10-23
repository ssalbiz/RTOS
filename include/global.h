#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#define DEBUG 1
#define MESSAGE_SIZE 256
#define MEMBLOCK_SIZE 256
#define NUM_PROCESS 23
#define NUM_UPROCESS 20
#define MIN_PRIORITY 3
#define MAX_PRIORITY 0
#define TIMER_INTERVAL 10000
#define STK_OFFSET 1
#define KEYBOARD_FILE "kbd_mem"
#define CRT_FILE "crt_mem"
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

typedef struct message_queue {
  MessageEnvelope* head;
  MessageEnvelope* tail;
} message_queue;


typedef struct PCB {
  int pid; //process id
  enum States state; //process current state
  enum Priority priority; //process priority
  char* stack; //process stack pointer
  char* stack_head; //original stack allocation address
  int stack_size; //process stack limit
  struct PCB* q_next; //process queue reference
  struct PCB* p_next; //global process list reference
  void* process_code; //initial function for process
  jmp_buf context;
  message_queue message_send; //send message queue
  message_queue message_receieve; // receiving message queue
  enum bool i_process; //is this an i_process? PCB

} PCB;

typedef struct priority_process_queue {
  PCB* pq_head[MIN_PRIORITY + 1];
  PCB* pq_tail[MIN_PRIORITY + 1];
} priority_process_queue;

typedef struct process_queue {
  PCB* head;
  PCB* tail;
} process_queue;


priority_process_queue* _rpq; //global ready process queue
priority_process_queue* _mwq; //global MESSAGE_WAIT process queue
priority_process_queue* _ewq; //global ENVELOPE_WAIT queue
message_queue* _feq; //global free envelope queue
PCB* current_process;

process_queue* _process_list;
PCB* timer_i_process;
PCB* keyboard_i_process;
PCB* crt_i_process; 

caddr_t _kbd_mem_ptr, _crt_mem_ptr;
int _kbd_pid, _crt_pid;
int _kbd_fid, _crt_fid;

#endif
