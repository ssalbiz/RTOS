#include "kernel.h"
#include "queues.h"
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
  
  if (!rpq_is_empty) _rpq_free();
//  if () _mwq_free();
//  if (_ewq != NULL) _ewq_free();
//  if (_feq != NULL) _feq_free();

  //atomic(0);
}
