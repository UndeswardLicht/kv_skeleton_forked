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

  create_client_pipe(client_pid);

  fd_server = connect_to_server();
  debug("Cl: Connected to server pipe");

  /* char pid[9]; */
  /* sprintf(pid, "%d", client_pid); */
  debug("My pid is %d\n", client_pid);

  fd_client = connect_to_client(client_pid);

  char buf_in[MAX_STRING_LENGTH];
  bool valid = false;

  while(TRUE){
    //    while(!valid){
        printf("> ");
        fgets(buf_in, 1024, stdin);
        //        valid = validateString(buf_in);
        //    }
        int res = send_msg(fd_server, buf_in, strlen(buf_in));
        if (res != 1){
          fprintf(stderr, "Cl: Failed to send msg\n");
        }

        if(recv_msg(fd_client, message, sizeof(message)) == TRUE){
          printf("%s", message);
        }
        printf("\n");

      }

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
