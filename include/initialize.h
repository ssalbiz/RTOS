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
typedef struct arg_list {
  int parent_pid;
  int mem_size;
  int fid;
} arg_list;

typedef struct init_table {
  int pid;
  enum Priority priority;
  int stack_size;
  void* process_code;
} init_table;

int mask();
int register_handlers();
void setup_kernel_structs();
void init_processes();
arg_list* allocate_shared_memory(caddr_t *mem_ptr, char* fname);
int unmask();

init_table IT[NUM_UPROCESS];

#endif
