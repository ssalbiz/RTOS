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
#ifndef IPROCESS_H
#define IPROCESS_H
#include <stdio.h>
#include <stdlib.h>
#include <global.h>
#include "userAPI.h"
#include <signal.h>
/*! maintains message timeout queue, updates internal real-time clock and wall clock
 */
void timer_service(void);

/*! linear time enqueue that relatavizes the timeout of the new message to the sum of all preceding
    envelopes. This is done to allow for constant time updating of the queue (done every tick) by simply
    decrementing the timeout of the head of the queue and dequeing when the head reaches zero.
 */
void timeout_enqueue(MessageEnvelope* msg, message_queue* mq);

/*! Maintains shared memory block between kernel and keyboard helper. Called when SIGUSR1 is raised.
    When the signal is raised, any envelopes sent by invocations of get_console_chars are processed,
    have a line of buffered input copied into them and are then returned in serial order based on 
    available data in the buffer
 */
void keyboard_service(void);

/*! Maintains shared memory block between kernel and crt helper. Called when SIGUSR2 is raised OR
    by invocations of send_console_chars. When called, any message envelopes sent by invocations of 
    send_console_chars are processed, copied into the message buffer and returned in serial order.
 */
void crt_service(void);

/*! called by the timer_service every second, this function uses a reserved envelope to send the crt
    service a message envelope with the updated wall clock string.
 */
void update_clock(void);

/*! The handler function registered with the underlying POSIX environment to be called when any
    defer-able signal is raised. Switches processes to the appropriate i_process and calls the
    i_process entry point when the corresponding signal is raised. No context switching occurs,
    such that the i_process executes on top of the currently executing process stack. Since the
    i_process never blocks and returns almost immediately, the stack is restored when the signal
    handler exits.
 */
void signal_handler(int signal); 
#endif
