#include "config.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "libcomm.h"
#include "libstore.h"

bool validateString(char *input);

int main(){

  pid_t client_pid;
  int fd_client;
  int fd_server;
  client_pid = getpid();
  char message[MAX_STRING_LENGTH];

  fd_server = connect_to_server();
  debug("Cl: Connected to server pipe");

  //  write(fd_server, client_pid, sizeof(client_pid));
  // char msg[] = "Some msg from client";
  char pid[9];
  sprintf(pid, "%d", client_pid);
  debug("My pid is %s\n", pid);

  fd_client = create_client_pipe(client_pid);
  debug("Cl: Client pipe created");
  write(fd_server, pid, sizeof(pid));

  /* send_msg(fd_server, msg, sizeof(msg)); */
  /* debug("msg is sent from client on srvr pipe, waiing for rspns\n"); */

  read(fd_client, message, sizeof(message));
  printf("%s \n", message);

  /* char buf_in[MAX_STRING_LENGTH]; */
  /* bool valid = false; */
  /* while(!valid){ */
  /*       printf("> "); */
  /*       fgets(buf_in, 1024, stdin); */
  /*       valid = validateString(buf_in); */
  /*     } */

  /* /\* char response[MAX_STRING_LENGTH]; *\/ */
  /* /\* recv_msg(fd_client, response, sizeof(response)); *\/ */
  /* /\* printf("response msg from server is: %s\n", response); *\/ */

  /* char buf_out[MAX_STRING_LENGTH]; */
  /*  while (send_msg(fd_server, buf_in, sizeof(buf_in)) == TRUE){ */

  /*    if(recv_msg(fd_client, buf_out,sizeof(buf_out)) != TRUE){ */
  /*      fatal("Cl: error when trying to read response from server: %s ", strerror(errno)); */
  /*    } */
  /*  } */







  destroy_pipe(fd_client, client_pid);
  disconnect_pipe(fd_server);


  return 0;
}

bool validateString(char *input){
    char *cp, *method, *type, *key, *value;
    cp = strdup(input);
    method = strtok(cp, " ");
    type = strtok(NULL, " ");
    key = strtok(NULL, " ");
    value = strtok(NULL, " ");
    debug("Cl: first: %s, second: %s, third:%s, fourth:%s\n", method, type, key, value);

    int i = 0;
    int wrd = 0;
    while(input[i] != '\0'){
      if(input[i] == '\t' || input[i] == ' ' || input[i] == ' '){
        wrd++;
      }
      i++;
    }

     if(wrd < 3){
     printf("Command contains less than 3 words!\n");
     return false;
     }

     if(strcmp(method, "get") != 0 &&
        strcmp(method, "set") != 0){
       printf("No such command! enter either 'get' or 'set'.\n");
       return false;
     }

     if(strcmp(type, "int") != 0 &&
        strcmp(type, "string") != 0){
       printf("Your type should be either 'int' or 'string.\n'");
       return false;
     }

  return true;
}
