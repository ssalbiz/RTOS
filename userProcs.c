#include "global.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "userAPI.h"


void test_process_send() { //pid == 3
  //testing message send
  MessageEnvelope* env = request_message_envelope();
  strcpy(env->data, "does this work?");
  //printf("input:_%s_\n", env->data);
  send_console_chars(env);
  env = receive_message();
  get_console_chars(env);
  env = receive_message();
  send_console_chars(env);
  env = receive_message();
  //printf("sending message to %d\n", 2);
  request_delay(10, 1, env);
  env = receive_message();
  strcpy(env->data, "does this work?");
  send_message(4, env);
  //printf("send message successful\n");
  fflush(stdout);
  env = request_message_envelope();
  while (1) {
    send_console_chars(env);
    env = receive_message();
    get_console_chars(env);
    env = receive_message();
  }
    
}

void test_process_receive() { //pid == 4
  //testing message recieve
  //printf("attemping to receive from 1\n");
  MessageEnvelope* env = receive_message();
  //printf("got:_%s_ from process id: %d\n", env->data, env->sender_pid);
  get_trace_buffer(env);
  send_console_chars(env);
  env = receive_message();
//  printf("message trace:\n%s\n", env->data);
  change_priority(MIN_PRIORITY, 4);
  request_process_status(env);
//  printf("process status:\n%s\n", env->data);
  send_console_chars(env);
  release_message_envelope(env);
  while (1) release_processor();
}
