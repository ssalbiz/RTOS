#include "initialize.h"
#include "global.h"
#include "iprocesses.h"

int mask() {
  sigset_t newmask;
  sigemptyset(&newmask);
  sigaddset(&newmask, SIGINT);
  sigaddset(&newmask, SIGUSR1);
  sigaddset(&newmask, SIGUSR2);
  sigaddset(&newmask, SIGALRM);
  if (sigprocmask(SIG_BLOCK, &newmask, NULL) != 0) 
    return -1;
  else
    return 0;
}

int register_handlers() {

  sigset(SIGILL, signal_handler);
  sigset(SIGSEGV, signal_handler);
  sigset(SIGHUP, signal_handler);
  sigset(SIGINT, signal_handler);
  sigset(SIGUSR1, signal_handler);
  sigset(SIGUSR2, signal_handler);
  sigset(SIGALRM, signal_handler);


return 0;
}

void setup_kernel_structs() {
}

void init_processes() {
}

arg_list* allocate_shared_memory(caddr_t* mem_ptr) {
return NULL;
}

int unmask() {
  sigset_t newmask;
  sigemptyset(&newmask);
  sigaddset(&newmask, SIGINT);
  sigaddset(&newmask, SIGUSR1);
  sigaddset(&newmask, SIGUSR2);
  sigaddset(&newmask, SIGALRM);
  if (sigprocmask(SIG_UNBLOCK, &newmask, NULL) != 0) 
    return -1;
  else
    return 0;
}


int main(int argc, char** argv) {
   mask();
   register_handlers();
   setup_kernel_structs();
   init_processes();
   allocate_shared_memory(&_kbd_mem_ptr);
   allocate_shared_memory(&_crt_mem_ptr);
   _kbd_pid = fork();
   if (_kbd_pid == 0) {
     execl("./keyboard", (char*) 0);
 //    terminate();
   } 
   _crt_pid = fork();
   if (_crt_pid == 0) {
     execl("./crt", (char*) 0);
 //    terminate();
   }
   
   printf("Quitting..\n");

   

return 0;
}
