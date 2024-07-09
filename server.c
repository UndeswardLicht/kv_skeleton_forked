#include "config.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stddef.h>

#include "libcomm.h"
#include "libstore.h"

int
main(void)
{
	store *st = create_store();
	int fd_server;
	int fd_client;
	int i = 0;
	char buf_in[MAX_STRING_LENGTH];
        char buf_out[MAX_STRING_LENGTH];

	if (st == NULL) {
		fatal("unable to initialize store");
	}

	fd_server = create_server_pipe();

        while(TRUE){
          if(recv_msg(fd_server, buf_in, sizeof(buf_in)) == TRUE){
            debug("Sr: received msg from client: %s", buf_in);

            char *cp, *pid_char, *method, *type, *key, *value;
            cp = strdup(buf_in);
            pid_char = strtok(cp, " ");
            method = strtok(NULL, " ");
            type = strtok(NULL, " ");
            key = strtok(NULL, " ");
            value = strtok(NULL, " ");
            debug("Sr: pid: %s, first: %s, second: %s, third:%s, fourth:%s",pid_char, method, type, key, value);

            if(strcmp(method, "set") == 0){
              //to store the message
               if(strcmp(type, "int") == 0){
                 int v = atoi(value);
                 add_number(st, key, v);
               }else{
                 add_string(st, key, value);
               }
               snprintf(buf_out, sizeof(buf_out),"Sr: the value '%s' and  key '%s' are stored\n", value, key);
            }
            else if(strcmp(method, "get") == 0){
              //to get the values from vault
              char *got_value = get_value_by_key(st, key);
              if(got_value != NULL){
                snprintf(buf_out, sizeof(buf_out), "Sr: %s = %s\n", key,got_value);
                free(got_value);
              }
              else{
                snprintf(buf_out, sizeof(buf_out), "No such key: %s\n", key);
              }
            }

            int client_pid = atoi(pid_char);
            fd_client = connect_to_client(client_pid);

            if(send_msg(fd_client,buf_out,strlen(buf_out)) != TRUE){
              fprintf(stderr, "Sr :Failed to send msg to client\n");
            }
            close(fd_client);
            debug("Sr: Closed client pipe\n");
          }

        }

	remove_store(st);
	disconnect_pipe(fd_client);
	destroy_pipe(fd_server, 0);
        
	return 0;
}
