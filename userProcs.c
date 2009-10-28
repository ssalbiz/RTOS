#include "global.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "userAPI.h"


void test_process_send() { //pid == 1
  //testing message send
  MessageEnvelope* env = (MessageEnvelope*) malloc(sizeof(MessageEnvelope));
  printf("sending message to %d\n", 2);
  strcpy(env->data, "does this work?");
  send_message(2, env);
  free(env);
  printf("send message successful\n");
  fflush(stdout);
  while (1) release_processor();
}

void test_process_receive() { //pid == 2
  //testing message recieve
  printf("attemping to receive from 1\n");
  MessageEnvelope* env = receive_message();
  printf("got:_%s_ from process id: %d\n", env->data, env->sender_pid);
  while (1) release_processor();
}
