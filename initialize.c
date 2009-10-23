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
  ppq_allocate(&_rpq); //process queues
  ppq_allocate(&_mwq);
  ppq_allocate(&_ewq);
  pq_allocate(&_process_list);
  mq_allocate(&_feq); //message queues
  MessageEnvelope *new;
  //allocate message envelopes
  for (i = 0; i < ENVELOPES; i++) {
    new = (MessageEnvelope*) malloc(sizeof(MessageEnvelope));
    mq_enqueue(new, _feq);
  }
}

jmp_buf kernel_buf;

void init_processes() { //initialize PCB properties from init table and start context
  int i = 0;
  PCB* newPCB = NULL;
  for (; i < 1; i++) { //TODO: replace after unit tests
    newPCB = (PCB*) malloc(sizeof(PCB));
    newPCB->pid        = IT[i].pid;
    newPCB->priority   = IT[i].priority;
    newPCB->stack_size = IT[i].stack_size;
    newPCB->stack = ((char*)malloc(newPCB->stack_size)) + newPCB->stack_size - STK_OFFSET;
    newPCB->stack_head = newPCB->stack - newPCB->stack_size + STK_OFFSET;
    newPCB->state      = READY;
    newPCB->q_next     = NULL;
    newPCB->p_next     = NULL;
    newPCB->process_code = (void*) IT[i].process_code;
    pq_enqueue(newPCB, _process_list);
    ppq_enqueue(newPCB, _rpq);
//    if ( setjmp(kernel_buf) == 0) {
//      jmpsp = newPCB->stack;
//      __asm__ ("movl %0,%%esp" :"=m" (jmpsp));
//      if (setjmp(newPCB->context) == 0) {
//        longjmp(kernel_buf, 1);
//      } else {
//        void (*tmp) ();
//        tmp = (void*) newPCB->process_code;
//        tmp();
//      }
//    }
  }
}

void init_process_contexts() {
    char* jmpsp;
    PCB* newPCB = pq_peek(_process_list);
    if ( setjmp(kernel_buf) == 0) {
      jmpsp = newPCB->stack;
      __asm__ ("movl %0,%%esp" :"=m" (jmpsp));
      if (setjmp(newPCB->context) == 0) {
        longjmp(kernel_buf, 1);
      } else {
        void (*tmp) ();
        tmp = (void*) newPCB->process_code;
        tmp();
      }
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
   mask(); //block signals
   register_handlers(); //register signal handlers
   setup_kernel_structs(); //allocate memory necessary for initialization
   //hardcode initialization table for now. Later read them from file or something
   //*
   init_table* np_rec = create_init_table(0, 0, 256, (void*)null_process);
   IT[0] = *np_rec;
   //*/

   init_processes();
   free(np_rec);

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
     mask();
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
     mask();
     execl("./CRT", arg1, arg2, arg3, (char*) 0);
     exit(1);
 //    terminate();
   }
   sleep(2);
   unmask();
   init_process_contexts();
   printf("Quitting Normally..\n");
   terminate();
   return 0;
}
