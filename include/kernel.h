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
#ifndef KERNEL_H
#define KERNEL_H
#include "queues.h"
#include "global.h"
#include "userAPI.h"
#include "iprocesses.h"
#include <sys/mman.h>
#include <sys/wait.h>

//kernel data structures
/*! pointer to currently executing process PCB. queue kernel pointer is expected to be NULL
    as the currently executing process may not simultaneously exist on any other process queue
 */
PCB* current_process;

/*! Ready Process Queue, contains PCBs ready for execution. Dispatcher selects head of this queue
    for execution.
 */
priority_process_queue* _rpq; //global ready process queue

/*! Message Waiting Queue, contains PCBS blocked on a message_receive() call. These PCBS will be 
    re-enqueued to the ready process queue if and when a call to send_message is made by another 
    process sending a message to the blocked process
    Note that any process PCB on this queue MUST have its state set to MESSAGE_WAIT. All kernel
    primitives that modify this queue are required to ensure that this condition is met.
 */
priority_process_queue* _mwq; //global MESSAGE_WAIT process queue

/*! Envelope Waiting Queue, contains PCBs blocked on a request_message_envelope() call. These PCBs will
    be re-enqueued to the ready process queue if and when a call to release_message_envelope() is made
    freeing up an envelope resource to be allocated to the blocked process.
    Note that any process PCB on this queue MUST have its state set to ENVELOPE_WAIT. All kernel 
    primitives that modify this queue are required to ensure that this condition is met
 */
priority_process_queue* _ewq; //global ENVELOPE_WAIT queue

/*! Free Envelope Queue, contains message envelopes that are not currently owned by any process or
    waiting to be received.
 */
message_queue* _feq; 	      //global free envelope queue
/*! Message timeout queue. Maintained by the timer i_process. Contains timeout messages to be returned
    after the specified number of clock ticks.
 */
message_queue* _timeout;      //message timeout queue

/*! global process list, a linked list of every single system and user process, 
    except for the i_processes.
 */
process_queue* _process_list;

/*! trace buffer, stores the last TRACE_LENGTH-many successful IPC primitive invocations
    where an IPC primitive is either a send_message or a receive_message
 */
trace_buffer* _tq;

//signal handlers
/*! i process PCBs, declared explicitly as they do no reside on any process queue
 */
PCB* timer_i_process;
PCB* keyboard_i_process;
PCB* crt_i_process; 
/*! buffer PCB used for process switching inside the signal handler. The regular kernel primitive is
    with context switching is avoided here for performance reasons
 */
PCB* interrupted_process;

/*! kernel context buffer, used at initialization to jump back to kernel space after 
    initialization of the dummy context is complete
 */
jmp_buf kernel_buf;

/*! state variables tracking the absolute time elapsed since system initialization completed.
 */
int ticks, seconds;//absolute time since RTX start

/*! state variables tracking the wall clock time
 */
int wall_hr, wall_min, wall_sec, wall_state;

/*! reserved envelope and state flag for the timer i-process. The timer i process is the only iprocess
    that requires a call to request_message_envelope(), as it does not have a message passed to it 
    directly. Since message envelopes may run out, and the i process is not allowed to block, this 
    reserved envelope is used to send timer updates. The state flag is used to track the availability 
    of the reserved envelope. If the envelope is not available to the iprocess, instead of blocking,
    the primitive returns a NULL pointer
 */
int clock_envelope_state;
MessageEnvelope* clock_envelope; //reserved message envelope for wall clock

/*! cleans up allocated resources, kills helpers unmaps shared blocks and exits the system
 */
void K_terminate() ;

/*! longjmps from the previous context to the next context. previous context returns here when restored
 */
void K_context_switch(jmp_buf prev, jmp_buf next) ;

/*! Switches from currently executing process to the next process on the process ready queue
    However, this primitive assumes that the currently executing process PCB has been handled and
    placed on an appropriate process queue by a kernel primitive.
 */
void K_process_switch() ; 

/*! enqueues the current process back onto the ready queue and does a process switch
 */
void K_release_processor() ;

/*! null process to keep the RTX running when no other processes are ready.
    continually releases control back to the kernel until something of a higher priority becomes
    ready. Occupies the lowest priority slot
 */
void null_process() ;

/*! looks up a given pid and returns the corresponding PCB (linear time lookup)
    checks the global process queue and the iprocesses.
 */
PCB* pid_to_PCB(int target) ;

/*! frees all RTX resources, and kills helpers
 */
void K_cleanup() ;

/*! removes the given envelope from the current processes' message queue and puts it on the message
    receive queue of the PCB corresponding to the given destination pid. If the destination process
    was blocked on a receive_message call, the PCB is removed from the message waiting queue and 
    placed back onto the ready queue for execution
    The invoking process will never block.
 */
void K_send_message(int dpid, MessageEnvelope* env);

/*! checks the message_receive queue of the current process for pending messages. If at least one
    exists it is dequeued and returned. If none exist then the process blocks. The state is set to
    MESSAGE_WAIT and the process PCB is enqueued to the message waiting queue. Then the process-
    switch is called.
 */
MessageEnvelope* K_receive_message();

/*! dumps the contents of the tracebuffer into the given message envelope.
 */
int K_get_trace_buffer(MessageEnvelope* env);

/*! creates a message event for the given message envelope and inserts it into the tracebuffer
 */
void K_register_trace(MessageEnvelope* msg, int type);

/*! checks the free envelope queue for a free message envelope. If one exists, it is returned
    immediately. If not, the process blocks, has it state set to ENVELOPE WAIT, and the process
    is switched to the next ready process.
 */
MessageEnvelope* K_request_message_envelope(void);

/*! The given envelope is released back onto the free envelope queue. If any processes are
    blocked on a request_message_envelope, the head of the envelope waiting queue is put back
    on the ready process queue. Note that this does not guarantee that this process will 
    obtain the envelope, as another ready process could request this envelope before the 
    process regains execution control.
 */
void K_release_message_envelope(MessageEnvelope* env);
int K_request_process_status(MessageEnvelope* env);
int K_change_priority(int new_priority, int target_pid);
//TODO: implement
int K_request_delay(int timeout, int wakeup, MessageEnvelope* env);
int K_get_console_chars(MessageEnvelope* env);
int K_send_console_chars(MessageEnvelope* env);
void K_set_wall_clock(int h, int m, int s);
void K_set_wall_clock_state(int s);

#endif
