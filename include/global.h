#ifndef GLOBAL_H
#define GLOBAL_H
#include <stdlib.h>
#include <setjmp.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>
//#define DEBUG 1
#define TRUE 1
#define FALSE 0
#define MESSAGE_SIZE 2048
#define MEMBLOCK_SIZE 2048
#define NUM_PROCESS 5
#define NUM_UPROCESS 20
#define MIN_PRIORITY 3
#define MAX_PRIORITY 0
#define TIMER_INTERVAL 100000
#define TRACE_LENGTH 16
#define I_STACK_SIZE 4000
#define I_PROCS 3
#ifdef i386
#define STK_OFFSET 4//dword size (size of stack frame)
#endif
#ifdef __amd64
#define STK_OFFSET 4 //amd64 dword??
#endif
#ifdef __sparc
#define STK_OFFSET 4 //sparc dword??
#endif
#define MIN(X, Y)  ((X) < (Y) ? (X) : (Y))

#define KEYBOARD_FILE "kbd_mem"
#define CRT_FILE "crt_mem"


/*! This enum represents the possible process states.
    Each process can only occupy one state at a time, 
    Possible state transitions are specified in the SDD and the API
 */
enum States {
  EXECUTING=0,
  READY=1,
  MESSAGE_WAIT=2,
  ENVELOPE_WAIT=3,
  SLEEP=4,
  INTERRUPTED=5
};

/*! This enum specifies the priority scale of the process
    MAX priority is reserved for system processes, including both iprocesses
    and system processes running in userspace (CCI)
 */
enum Priority {
  MAX=MAX_PRIORITY,
  MID=1,
  MIDLOW=2,
  LOW=MIN_PRIORITY
};

/*! Tracebuffer message event type
    Used to determine which sub-queue to append message events to
 */
enum Event {
  SEND=0,
  RECEIVE=1
};

/*! Type of message contained in envelope
    CONSOLE IO typing is set by the kernel
    The WAKEUP type also set by the kernel, specifically, the timer
    User processes should use DEFAULT_IPC
 */
enum msg_type {
  WAKEUP,
  CONSOLE_OUTPUT,
  CONSOLE_INPUT,
  DEFAULT, //for user processes that do not explicitly set a type
};

/*! Shared memory block flag, used to determine if shared memory blocks
    between helpers are ready for reads/writes by the kernel and helpers
    MEM_READY signifies that the block is ready for kernel IO
    MEM_DONE signifies that the block is read for helper process IO
 */
enum shm_flag {
  MEM_READY=0,
  MEM_DONE=1
};
  
/*! Message Envelope for IPC.
    Timeout field is set automatically by the timer service as necessary
    message type also set automatically from kernel space, userspace 
    processes use the DEFAULT_IPC message type
    The sender and destination pids are also set by the kernel when IPC
    primitives are invoked
    next pointer used for embedding in kernel and PCB message queues, this should
    be NULL'ed automagically when the envelope is dequeued
 */
typedef struct MessageEnvelope {
  struct MessageEnvelope* next;
  int sender_pid, destination_pid;
  char data[MESSAGE_SIZE];
  int timeout_ticks;
  enum msg_type type;
} MessageEnvelope;

/*! Basic FIFO queue for message envelopes associated queue functions
    are prefixed with "mq_"
    Refer to queues.h for functional interface
 */ 
typedef struct message_queue {
  MessageEnvelope* head;
  MessageEnvelope* tail;
} message_queue;

/*! Process control block
    As specified in the SDD, this is the central data structure used to identify
    system and user processes
    Every PCB ever used is initialized by the initialization section. This version of the RTX
    Does not support on-the-fly process termination or creation. Therefore there will exist a
    fixed number of processes at all times during the execution of the system
    Stack pointer handling is managed by the underlying POSIX environment
    At system termination the original pointer returned from malloc (stored in stack_head)
    is freed, ensuring no memory leakage.
    The context buffer stores the current stack frames and is used for restoring processes after a
    context switch
    Each process contains 2 message queues, one for incoming messages to be received (message_receive)     The other is a local queue of messages that have been received and are now owned by the process.
    process_code is the entry point of each process specified at initialization,
    and called by the dispatcher.
    Each PCB contains two kernel pointers, q_next and p_next.
    q_next is used to embed the PCB into process wait and ready queues and is mutable on the fly
    p_next is used to embed the PCB into the global process queue and should not be modified after
    system initialization
 */

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
} PCB;


/*! Priority queue, implemented as a container of 4 FIFO PCB queues
    Refer to queues.h for a functional interface
 */
typedef struct priority_process_queue {
  PCB* pq_head[MIN_PRIORITY + 1];
  PCB* pq_tail[MIN_PRIORITY + 1];
} priority_process_queue;

/*! FIFO PCB queue,
    refer to queues.h for a functional interface
 */
typedef struct process_queue {
  PCB* head;
  PCB* tail;
} process_queue;

/*! tracebuffer event used to store traceback information about IPC invocations
    the timestamp is given in seconds and is set by the kernel at each invocation
 */
typedef struct msg_event {
  int destination_pid, source_pid;
  int timestamp; //in absolute seconds since system start
  enum msg_type mtype; //type of message sent/rcv'd
  enum Event type; //type of msg event, (ie sent or received)
  struct msg_event *next;
} msg_event;


/*! tracebuffer, implemented as a container of two fixed size arrays and 
    two integers with specify the position of the next insertion.
    The list is fixed size, so the positions roll over from the maximum length back to zero
 */
typedef struct trace_buffer {
  int send_length, receive_length; //to maintain fixed size
  msg_event* send[TRACE_LENGTH];
  msg_event* receive[TRACE_LENGTH];
} trace_buffer;


/*! Format of the shared memory block between the kernel and helpers.
    data is copied out of the block using strcpy directly
 */
typedef struct mem_buffer {
  int length;
  enum shm_flag flag;
  char data[MEMBLOCK_SIZE];
} mem_buffer;


//helper process data structures
caddr_t _kbd_mem_ptr, _crt_mem_ptr;
int _kbd_pid, _crt_pid;
int _kbd_fid, _crt_fid;

//signal masking
int masked;
sigset_t rtxmask;

#endif
