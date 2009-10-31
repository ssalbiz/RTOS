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
