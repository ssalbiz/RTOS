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
#define NUM_PROCESS 3
#define NUM_UPROCESS 20
#define MIN_PRIORITY 3
#define MAX_PRIORITY 0
#define TIMER_INTERVAL 10000
#ifdef i386
#define STK_OFFSET 4 //dword size (size of stack frame)
#endif
#ifdef __amd64
#define STK_OFFSET 4 //amd64 dword??
#endif
#ifdef __sparc
#define STK_OFFSET 4 //sparc dword??
#endif

#define KEYBOARD_FILE "kbd_mem"
#define CRT_FILE "crt_mem"
enum States {
  EXECUTING=0,
  READY=1,
  MESSAGE_WAIT=2,
  ENVELOPE_WAIT=3,
  SLEEP=4,
  INTERRUPTED=5
};

enum Priority {
  MAX=MAX_PRIORITY,
  MID=1,
  MIDLOW=2,
  LOW=MIN_PRIORITY
};

enum bool { 
  FALSE=0,
  TRUE=1
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
  message_queue* message_send; //send message queue
  message_queue* message_receive; // receiving message queue
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


//helper process data structures
caddr_t _kbd_mem_ptr, _crt_mem_ptr;
int _kbd_pid, _crt_pid;
int _kbd_fid, _crt_fid;

//signal masking
enum bool masked;
sigset_t rtxmask;

#endif
