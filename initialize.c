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

int register_handler(int signal) {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGINT);
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGUSR2);
    sigaddset(&sa.sa_mask, SIGALRM);
    sa.sa_flags = SA_RESTART; 
    if (sigaction(signal, &sa, NULL) == -1) 
      return -1;
    else 
      return 0;
}

int register_handlers() {
//  sigset(SIGILL,  signal_handler);
//  sigset(SIGSEGV, signal_handler);
  register_handler(SIGHUP);
  register_handler(SIGINT);
  register_handler(SIGUSR1);
  register_handler(SIGUSR2);
  register_handler(SIGALRM);
return 0;
}

void setup_kernel_structs() {
  int i;
  ppq_allocate(&_rpq); //process queues
  ppq_allocate(&_mwq);
  ppq_allocate(&_ewq);
  pq_allocate(&_process_list);
  mq_allocate(&_feq); //message queues
  mq_allocate(&_timeout);
  trace_allocate(&_tq);
  MessageEnvelope *new_env;
  //allocate message envelopes
  for (i = 0; i < ENVELOPES; i++) {
    new_env = (MessageEnvelope*) malloc(sizeof(MessageEnvelope));
    mq_enqueue(new_env, _feq);
  }
  //allocate clock envelope
  clock_envelope = (MessageEnvelope*) malloc(sizeof(MessageEnvelope));
  clock_envelope->next = NULL;
  clock_envelope_state = 1;
}

void dispatch() {
  //should never return, so dummy jmp_buf is fine
  jmp_buf dummy;
  PCB* first_process = ppq_dequeue(_rpq);
  assert(first_process != NULL);
  current_process = first_process;
  assert(current_process->context != NULL);
  //initializer is trusted code, and is allowed to run kernel primitives directly
  atomic(1);
  K_context_switch(dummy, current_process->context);
}


void init_process_context(PCB* target) {
    char* proc_sp = NULL;
    PCB* newPCB = target;
    if (setjmp(kernel_buf) == 0) {
      proc_sp = newPCB->stack;
#ifdef i386 //reset stack ptr to current process PCB
      __asm__("movl %0,%%esp" :"=m" (proc_sp)); 
#endif
#ifdef __amd64 //reset stack ptr to word size (assume 32-bit compatibility sub-mode??)
      __asm__("movl %0,%%esp" :"=m" (proc_sp));
#endif
#ifdef __sparc
      _set_sp( (char*) newPCB->stack_head + newPCB->stack_size );
#endif
      if (setjmp(newPCB->context) == 0) {
        longjmp(kernel_buf, 1);
      } else {
        void (*tmp) (); //since context will only be restored when the process is selected for execution
        tmp = (void*) current_process->process_code;
	atomic(0);//make context switches atomic
        tmp();
      }
    }
}


void init_processes() { //initialize PCB properties from init table and start context
  int i = 0;
  PCB* newPCB = NULL;
  for (; i < NUM_PROCESS; i++) { //TODO: replace after unit tests
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
    assert(newPCB->process_code != NULL);
    mq_allocate(&(newPCB->message_send));
    mq_allocate(&(newPCB->message_receive));
#ifdef DEBUG
    printf("%d\n", newPCB->pid);
#endif
    pq_enqueue(newPCB, _process_list);
    ppq_enqueue(newPCB, _rpq);
    init_process_context(newPCB);
  }
  //allocate i_processes, dont touch context
  PCB** ip[] = {&timer_i_process, &keyboard_i_process, &crt_i_process};
  for (i = 0; i < I_PROCS; i++) {
#ifdef DEBUG
    printf("%d\n", i);
#endif
    (*ip[i]) = (PCB*) malloc(sizeof(PCB));
    (*ip[i])->pid        = i; //kernel processes get low pnums
    (*ip[i])->priority   = MAX_PRIORITY; //KERNEL
    (*ip[i])->stack_size = I_STACK_SIZE; //KERNEL
    (*ip[i])->stack = ((char*)malloc((*ip[i])->stack_size)) + (*ip[i])->stack_size - STK_OFFSET;
    (*ip[i])->stack_head = (*ip[i])->stack - (*ip[i])->stack_size + STK_OFFSET;
    (*ip[i])->state      = READY;
    (*ip[i])->q_next     = NULL;
    (*ip[i])->p_next     = NULL;
    (*ip[i])->process_code = NULL;
    mq_allocate(&((*ip[i])->message_send));
    mq_allocate(&((*ip[i])->message_receive));
  }
#ifdef DEBUG    
  sleep(2);
#endif

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

void read_initialization_table() {
  int i = 0;
  int pid, pri, stk;
  void* process_code[] = {(void*)null_process, (void*)CCI, (void*)processA, (void*)processB, (void*)processC}; //hard code preloaded processes
  FILE* fconf = fopen("init_table", "r");
  assert (fconf != NULL);
  for (; i < NUM_PROCESS; i++) {
    fscanf(fconf, "%d %d %d\n", &pid, &pri, &stk);
    IT[i].pid = pid;
    IT[i].priority = pri;
    IT[i].stack_size = stk;
    IT[i].process_code = process_code[i];
  }
  fclose(fconf);
}


int main(int argc, char** argv) {
   mask(); //block signals
   register_handlers(); //register signal handlers
   setup_kernel_structs(); //allocate memory necessary for initialization
   read_initialization_table();

   init_processes();

   arg_list* kbd_args = allocate_shared_memory(&_kbd_mem_ptr, KEYBOARD_FILE);
   arg_list* crt_args = allocate_shared_memory(&_crt_mem_ptr, CRT_FILE);
   if (kbd_args == NULL || crt_args == NULL) terminate();
   _kbd_fid = kbd_args->fid;
   _crt_fid = crt_args->fid;
   ((mem_buffer*)_kbd_mem_ptr)->flag = MEM_DONE;
   ((mem_buffer*)_crt_mem_ptr)->flag = MEM_DONE;
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
   masked = FALSE;
   //sleep(2);
   ticks = 0;
   wall_state = 0;
   unmask();
   ualarm(TIMER_INTERVAL, TIMER_INTERVAL);
   dispatch();
   printf("Quitting from kernel...(this means you fucked up)\n");
   terminate();
   return 0;
}
