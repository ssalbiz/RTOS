#ifndef INITIALIZE_H
#define INITIALIZE_H
#include <fcntl.h>
#include <sys/mman.h>
#include "global.h"
#include "kernel.h"
#include "iprocesses.h"
#include "userAPI.h"
#include "userProcs.h"
#define ENVELOPES 20
//data structures relevant only to initialization
/*! argument list for helper process intialization.
 */
typedef struct arg_list {
  int parent_pid;
  int mem_size;
  int fid;
} arg_list;

/*! initialization table for userspace processes
    pids, priority and stack allocations are specified in the file "init_table"
    located in the root directory. Entry points are hard-coded
 */
typedef struct init_table {
  int pid;
  enum Priority priority;
  int stack_size;
  void* process_code;
} init_table;

/*! masks all expected signals that may interfere with proper system initialization
    SIGALRM, SIGUSR1, SIGUSR2 and SIGINT are masked on invocation. Unlike the kernel atomic
    function, no previous context is saved
 */
int mask();

/*! registers signal handler with UNIX for each and every signal of interest
    using the sigaction POSIX function. the sa_flags are set to SA_RESTART, and the
    mask set contains every signal of interest (INT, USR1, USR2, ALRM), to ensure that the signal
    handler is uninterruptible
 */
int register_handlers();

/*! Performs main memory allocation for kernel data structures, including:
      - free envelope queue
      - process ready queue
      - process message_wait queue
      - process envelope_wait queue
      - global process list
      - message timeout queue
      - all message envelopes to be used by the system
     Refer to kernel.h for more detail on the function of each of these structures
 */
void setup_kernel_structs();

/*! Allocates memory and sets pids and entry points for all processes. Process contexts are initialized
    such that the dispatcher will invoke the entry point on the first context switch
 */
void init_processes();

/*! maps shared memory block to a given filename
    returns an argument list
 */
arg_list* allocate_shared_memory(caddr_t *mem_ptr, char* fname);

/*! unmasks all masked signals just before processes begin execution.
 */
int unmask();

init_table IT[NUM_UPROCESS];

#endif
