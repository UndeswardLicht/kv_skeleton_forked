#include "config.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>

#include "libcomm.h"
#include "libstore.h"

store *st;
int fd_server;
int fd_client;
void vsioRebiataHare(store *b, int c);

int
main(void)
{
  st = create_store();
  char buf_in[MAX_STRING_LENGTH];
  char buf_out[MAX_STRING_LENGTH];

   if (st == NULL) {
     fatal("unable to initialize store");
   }
   fd_server = create_server_pipe();

   while(TRUE){
     if(recv_msg(fd_server, buf_in, sizeof(buf_in)) == TRUE){
       debug("Sr: from client: %s", buf_in);

       if(strcmp(buf_in, "fin") == 0){
         write(fd_client, "Sr: Session can be ended\n", 24);
         vsioRebiataHare(st, fd_client);
       }

       //tokenizing strings
       char *pid_char, *method, *type, *key, *value;
       pid_char = strtok(buf_in, " ");
       method = strtok(NULL, " ");
       type = strtok(NULL, " ");
       key = strtok(NULL, " ");
       value = strtok(NULL, " ");
       /* debug("Sr: pid: %s, first: %s, second: %s, third:%s, fourth:%s",pid_char, method, type, key, value); */

            if(strcmp(method, "set") == 0){
              //ifs to store the message
               if(strcmp(type, "int") == 0){
                 int v = atoi(value);
                 add_number(st, key, v);
               }else{
                 add_string(st, key, value);
               }
               snprintf(buf_out, sizeof(buf_out),"Sr: the value '%s' and  key '%s' are stored\n", value, key);
            }
            else if(strcmp(method, "get") == 0){
              //ifs to get the values from vault
              char *got_value = get_value_by_key(st, key);
              if(got_value != NULL){
                snprintf(buf_out, sizeof(buf_out), "Sr: %s = %s\n", key, got_value);
                free(got_value);
              }
              else{
                snprintf(buf_out, sizeof(buf_out), "No such key: %s\n", key);
              }
            }

            int client_pid = atoi(pid_char);
            fd_client = connect_to_client(client_pid);

            if(send_msg(fd_client,buf_out,strlen(buf_out)) != TRUE){
              fprintf(stderr, "Sr: Failed to send msg to client\n");
            }
          }

        }

   vsioRebiataHare(st, fd_server);
   return 0;
}

void vsioRebiataHare(store *st, int fd_server){
  remove_store(st);
  destroy_pipe(fd_server, 0);
  exit(0);
}

