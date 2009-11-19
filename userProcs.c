#include "global.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "userAPI.h"


void test_process_send() { //pid == 4
  //testing message send
  MessageEnvelope* env = request_message_envelope();
  strcpy(env->data, "does this work?\n");
  //printf("input:_%s_\n", env->data);
//  send_console_chars(env);
//  env = receive_message();
//  get_console_chars(env);
//  env = receive_message();
//  send_console_chars(env);
//  env = receive_message();
  //printf("sending message to %d\n", 2);
  request_delay(10, 1, env);
  env = receive_message();
  strcpy(env->data, "does this work?");
  env->type = DEFAULT;
  send_message(4, env);
  //printf("send message successful\n");
//  fflush(stdout);
  env = request_message_envelope();
  while (1) {
  release_processor();
//    send_console_chars(env);
//    //   env = receive_message();
//    get_console_chars(env);
//    env = receive_message();
  }
    
}

void test_process_receive() { //pid == 5
  //testing message recieve
  //printf("attemping to receive from 1\n");
  MessageEnvelope* env = receive_message();
  //printf("got:_%s_ from process id: %d\n", env->data, env->sender_pid);
  get_trace_buffer(env);
//  send_console_chars(env);
//  env = receive_message();
//  printf("message trace:\n%s\n", env->data);
  change_priority(MIN_PRIORITY, 4);
  request_process_status(env);
//  printf("process status:\n%s\n", env->data);
//  send_console_chars(env);
//  env = receive_message();
  release_message_envelope(env);
  while (1) release_processor();
}

void CCI() { //top priority, pid = 3
  MessageEnvelope *env = NULL, *n_env = NULL, *head = NULL, *tmp = NULL;
  int envs = 2, tmp1, tmp2, tmp3;
  char u_input[MEMBLOCK_SIZE];
  env = request_message_envelope();
  head = env;
  do {
    n_env = request_message_envelope();
    env->next = n_env;
    env = env->next; //grab some more envelopes
    env->next = NULL;
  } while (--envs > 0);
  tmp = head;
  head = head->next;
  strcpy(tmp->data, "MTE 241 RTX: Enter Command to ^C to exit\n");
  send_console_chars(tmp);
  tmp = receive_message();
  tmp = head;
  head = head->next;
  do {
    get_console_chars(tmp);
    tmp = receive_message(); //possible block on receive
    strcpy(u_input, tmp->data);
     
    if (strcmp(u_input, "s") == 0) {
      send_console_chars(tmp);
      tmp = receive_message();
    } else if (strcmp(u_input, "ps") == 0) {
      request_process_status(tmp);
      send_console_chars(tmp);
      tmp = receive_message(); //echoback block
    } else if (strcmp(u_input, "cd") == 0) {
      send_console_chars(tmp);
      set_wall_clock_state(1);
      tmp = receive_message();
    } else if (strcmp(u_input, "ct") == 0) {
      send_console_chars(tmp);
      set_wall_clock_state(0);
      tmp = receive_message();
    } else if (strcmp(u_input, "b") == 0) {
      get_trace_buffer(tmp);
      send_console_chars(tmp);
      tmp = receive_message();
    } else if (strcmp(u_input, "t") == 0) {
      strcpy(tmp->data, "Terminating...\n");
      send_console_chars(tmp);
      terminate();
    } else if (u_input[0] == 'n') {
      if (sscanf(u_input, "%c %d %d", &(u_input[0]), &tmp1, &tmp2) < 3) {
        send_console_chars(tmp);
        tmp = receive_message();
      } else {
        int ret = change_priority(tmp1, tmp2);
        sprintf(tmp->data, "%d\n", ret);
        send_console_chars(tmp);
        tmp = receive_message();
      }
    } else if (u_input[0] == 'c') {
      tmp1 = 0; tmp2 = 0; tmp3 = 0;
      if (sscanf(u_input, "%c %d:%d:%d", &(u_input[0]), &tmp1, &tmp2, &tmp3) < 4) {
        send_console_chars(tmp);
        tmp = receive_message();
      } else {
        set_wall_clock(tmp1, tmp2, tmp3);
      }
    } else {
      strcat(tmp->data, ": ERROR, bad command\n");
      send_console_chars(tmp);
      tmp = receive_message();
    }
  } while (1);
}
