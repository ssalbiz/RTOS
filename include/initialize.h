/*
 * This file is part of myRTX.
 *
 * myRTX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * myRTX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with myRTX.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */
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
  char name[15];
} init_table;

int mask();
int register_handlers();
void setup_kernel_structs();
void init_processes();
arg_list* allocate_shared_memory(caddr_t *mem_ptr, char* fname);
int unmask();

init_table IT[NUM_UPROCESS];

#endif
