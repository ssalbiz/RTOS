#include "kernel.h"
#include "global.h"

void terminate() {
  cleanup();
  exit(0);
}

void cleanup() {
  //atomic(1);
  kill(_kbd_pid, SIGINT);
  kill(_crt_pid, SIGINT); 
  /*
  int stat = 0;
  if (_kbd_mem_ptr != NULL) {
    stat = munmap(_kbd_mem_ptr, MEMBLOCK_SIZE);
    stat = close(_kbd_mem_info->fid);
  }
  if (_crt_mem_ptr != NULL) {
    stat = munmap(_crt_mem_ptr, MEMBLOCK_SIZE);
    stat = close(_crt_mem_info->fid);
  }

  while(_process_list != NULL) {
  /*/

  //atomic(0);
}
