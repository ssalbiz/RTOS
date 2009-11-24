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
jmp_buf interrupted;


void timer_service(void);
void timeout_enqueue(MessageEnvelope* msg, message_queue* mq);
void keyboard_service(void);
void crt_service(void);
void update_clock(void);
void signal_handler(int signal); 
#endif
