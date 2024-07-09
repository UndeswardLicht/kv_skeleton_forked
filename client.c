#include "config.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>

#include "libcomm.h"
#include "libstore.h"

bool validateString(char *input);
void handler(int i);
void vsioRebiataHare(int a, int b);

int fd_client = -1;
int fd_server = -1;
pid_t client_pid = 0;

int main(){

  client_pid = getpid();
  char message[MAX_STRING_LENGTH];
  signal(SIGINT, handler);

  create_client_pipe(client_pid);

  fd_server = connect_to_server();
  debug("Cl: Connected to server pipe");
  debug("My pid is %d", client_pid);

  fd_client = connect_to_client(client_pid);

  char buf_in[MAX_STRING_LENGTH];
  bool valid = false;

  while(TRUE){

      while(!valid){
        printf("> ");
        fgets(buf_in, 1024, stdin);
        valid = validateString(buf_in);
      }
      valid = false;

        if(send_msg(fd_server, buf_in, strlen(buf_in)) == 1){
            printf("Cl: Sent to server pipe: %s\n", buf_in);
        }else{
          fprintf(stderr, "Cl: Failed to send msg\n");
        }

      sleep(1);
      while(recv_msg(fd_client, message, sizeof(message)) == TRUE){
        printf("%s", message);
          break;
      }
     }

  vsioRebiataHare(fd_client,client_pid);
  return 0;
}

///Input validator returns boolean type, takes char pointer as an argument - this is the string we read in the main method from user input before
bool validateString(char *input){
    char *cp, *method, *type, *key, *value;
    cp = strdup(input);
    method = strtok(cp, " ");
    type = strtok(NULL, " ");
    key = strtok(NULL, " ");
    value = strtok(NULL, " ");

    int i = 0;
    int wrd = 0;
    while(input[i] != '\0'){
      if(input[i] == '\t' || input[i] == ' ' || input[i] == ' '){
        wrd++;
      }
      i++;
    }

     if(wrd < 3){
     warning("Command contains less than 3 words!\n");
     return false;
     }

     if(strcmp(method, "get") != 0 &&
        strcmp(method, "set") != 0){
       warning("No such command! enter either 'get' or 'set'.\n");
       return false;
     }

     if(strcmp(type, "int") != 0 &&
        strcmp(type, "string") != 0){
       warning("Your type should be either 'int' or 'string.\n'");
       return false;
     }

     free(cp);
  return true;
}

//void takes two ints - descriptor and client_pid
///
void vsioRebiataHare(int fd_client, int client_pid){
    destroy_pipe(fd_client, client_pid);
}

//signal handler, sends "fin" message to server and receives one back. Also calls finishing method to destroy the pipe
void handler(int signal){
  char *confirmation[30] = {0};
  printf("Received signal: %d\n", signal);
  write(fd_server, "fin", 4);
  read(fd_client, confirmation, 30);
  printf("%s\n", confirmation);
  vsioRebiataHare(fd_client, client_pid);
  exit(0);
}
