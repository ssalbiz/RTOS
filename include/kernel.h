#ifndef KERNEL_H
#define KERNEL_H
#include "queues.h"
#include "global.h"
#include <sys/mman.h>
jmp_buf kernel_buf;
void context_switch(jmp_buf p, jmp_buf n);
void process_switch();
void null_process();
void terminate();
void cleanup();
#endif
