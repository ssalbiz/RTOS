#ifndef INITIALIZE_H
#define INITIALIZE_H
#include<signal.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include "global.h"
#define ENVELOPES 20
//data structures relevant only to initialization
typedef struct arg_list {
  int parent_pid;
  int mem_size;
  FILE* fid;
} arg_list;

typedef struct init_table {
  int pid;
  int priority;
  int stack_size;
  void* process_code;
} init_record;

int mask();
int register_handlers();
void setup_kernel_structs();
void init_processes();
arg_list* allocate_shared_memory(caddr_t *mem_ptr);
int unmask();

#endif
