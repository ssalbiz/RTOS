#include "initialize.h"

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
  sigset(SIGILL,  signal_handler);
  sigset(SIGSEGV, signal_handler);
  sigset(SIGHUP,  signal_handler);
  sigset(SIGINT,  signal_handler);
  sigset(SIGUSR1, signal_handler);
  sigset(SIGUSR2, signal_handler);
  sigset(SIGALRM, signal_handler);
return 0;
}

void setup_kernel_structs() {
  int i;
  ppq_allocate(&_rpq);
  mwq_allocate();
  ewq_allocate();
  MessageEnvelope *new;
  //allocate message envelopes
  for (i = 0; i < ENVELOPES; i++) {
    new = (MessageEnvelope*) malloc(sizeof(MessageEnvelope));
    feq_enqueue(new);
  }
}

void init_processes() { //initialize PCB properties from init table and start context
  int i = 0;
  PCB* newPCB = NULL;
  for (; i < 1; i++) { //TODO: replace after unit tests
    newPCB = (PCB*) malloc(sizeof(PCB));
    newPCB->pid = IT[i].pid;
    newPCB->priority = IT[i].priority;
    newPCB->stack_size = IT[i].stack_size;
    newPCB->state = READY;
    newPCB->q_next = NULL;
    newPCB->p_next = NULL;
    proc_enqueue(newPCB);
    ppq_enqueue(newPCB, _rpq);
  }
}

arg_list* allocate_shared_memory(caddr_t* mem_ptr, char* fname) {
  arg_list* args = (arg_list*) malloc(sizeof(arg_list));
  args->parent_pid = getpid();
  args->mem_size = MEMBLOCK_SIZE;
  args->fid = open(fname, O_RDWR | O_CREAT | O_EXCL, (mode_t) 0755);
  if (args->fid == -1) { printf("ERROR: could not create mmap file\n"); return NULL; }
  ftruncate(args->fid, args->mem_size);
  (*mem_ptr) = mmap((caddr_t) 0,
  		    args->mem_size,
		    PROT_READ | PROT_WRITE,
		    MAP_SHARED,
		    args->fid,
                    (off_t) 0);
return args;
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

init_table* create_init_table(int pid, int priority, int stack, void* process_code) {
  init_table* new_rec = (init_table*) malloc(sizeof(init_table));
  new_rec->pid = pid;
  new_rec->priority = priority;
  new_rec->stack_size = stack;
  new_rec->process_code = process_code;
  return new_rec;
}


int main(int argc, char** argv) {
   mask();
   register_handlers();
   setup_kernel_structs();
   //hardcode initialization table for now. Later read them from file or something
   //*
   init_table* np_rec = create_init_table(0, 0, 256, (void*)null_process);
   IT[0] = *np_rec;

   //*/

   init_processes();
   free(np_rec);
   if (ppq_is_empty(_rpq))
     printf("METHOD FAIL\n");
   else 
     printf("Method works\n");

   arg_list* kbd_args = allocate_shared_memory(&_kbd_mem_ptr, KEYBOARD_FILE);
   arg_list* crt_args = allocate_shared_memory(&_crt_mem_ptr, CRT_FILE);
   _kbd_fid = kbd_args->fid;
   _crt_fid = crt_args->fid;
   char arg1[7], arg2[7], arg3[7];
   //parse arguments
   sprintf(arg1, "%d", kbd_args->parent_pid);
   sprintf(arg2, "%d", kbd_args->fid);
   sprintf(arg3, "%d", kbd_args->mem_size);
   free(kbd_args);
   _kbd_pid = fork();
   if (_kbd_pid == 0) {
     execl("./KB", arg1, arg2, arg3, (char*) 0);
     exit(1);
 //    terminate();
   } 
   sprintf(arg1, "%d", crt_args->parent_pid);
   sprintf(arg2, "%d", crt_args->fid);
   sprintf(arg3, "%d", crt_args->mem_size);
   free(crt_args);
   _crt_pid = fork();
   if (_crt_pid == 0) {
     execl("./CRT", arg1, arg2, arg3, (char*) 0);
     exit(1);
 //    terminate();
   }
   printf("Quitting..\n");
   terminate();
   

return 0;
}
