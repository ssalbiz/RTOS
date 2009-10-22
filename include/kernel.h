#ifndef KERNEL_H
#define KERNEL_H
#include "queues.h"
#include "global.h"
#include <sys/mman.h>
void null_process();
void terminate();
void cleanup();
#endif
