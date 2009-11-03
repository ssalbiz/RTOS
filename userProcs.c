#include "global.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "userAPI.h"


void test_process_send() { //pid == 3
  //testing message send
  MessageEnvelope* env = request_message_envelope();
  printf("sending message to %d\n", 2);
  request_delay(10, 1, env);
  env = receive_message();
  strcpy(env->data, "does this work?");
  send_message(4, env);
  printf("send message successful\n");
  fflush(stdout);
  while (1) release_processor();
}

void test_process_receive() { //pid == 4
  //testing message recieve
  printf("attemping to receive from 1\n");
  MessageEnvelope* env = receive_message();
  printf("got:_%s_ from process id: %d\n", env->data, env->sender_pid);
  get_trace_buffer(env);
  printf("message trace:\n%s\n", env->data);
  change_priority(MIN_PRIORITY, 2);
  request_process_status(env);
  printf("process status:\n%s\n", env->data);
  release_message_envelope(env);
  while (1) release_processor();
}
