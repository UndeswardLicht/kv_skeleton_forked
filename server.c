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
       
	if (st == NULL) {
		fatal("unable to initialize store");
	}

	fd_server = create_server_pipe();
        debug("Sr: Server pipe created\n");

        int pid[9];
        read(fd_server, pid, sizeof(pid));
        debug("this is client PID: %s \n", pid);

        int client_pid = atoi(pid);
        fd_client = connect_to_client(client_pid);

        char confirmation_message[] = "Sr: Connected to client pipe!";
        write(fd_client, confirmation_message, sizeof(confirmation_message));

        

 /*   while (recv_msg(fd_server, buf_in, sizeof(buf_in)) == TRUE) { */
 /*        	/\* FIXME: example *\/ */

 /*       	/\* char key[MAX_STRING_LENGTH] = {0}; *\/ */
 /*        /\* char *value = NULL; *\/ */
 /*     char buf_out[MAX_STRING_LENGTH]; */

 /*        /\* recv_msg(fd_server, buf_in, sizeof(buf_in)); *\/ */
 /*        /\* printf("this is msg from client: %s\n", buf_in); *\/ */


 /*        /\* char msg[] = "Some msg from server"; *\/ */
 /*        /\* send_msg(fd_client, msg, sizeof(msg)); *\/ */

 /*        /\* FIXME: parse input string here *\/ */
 /*    debug("read: %s", buf_in); */

 /*    char *cp, *method, *type, *key, *value; */
 /*    cp = strdup(buf_in); */
 /*    method = strtok(cp, " "); */
 /*    type = strtok(NULL, " "); */
 /*    key = strtok(NULL, " "); */
 /*    value = strtok(NULL, " "); */
 /*    debug("Sr: first: %s, second: %s, third:%s, fourth:%s", method, type, key, value); */

 /*        /\* FIXME: storing whole string *\/ */
 /*          //	snprintf(key, sizeof(key), "string_%d", i++); */
 /*    if(strcmp(type, "int") == 0){ */
 /*      int v = atoi(value); */
 /*      add_number(st, key, v); */
 /*    } */

 /*    if(strcmp(type, "string") == 0){ */
 /*      add_string(st, key, value); */
 /*    } */

 /*        /\* FIXME: generate message to client here *\/ */
 /*    value = get_value_by_key(st, key); */
 /*    snprintf(buf_out, sizeof(buf_out), "echo: %s = %s", key, value); */
 /*    free(value); */

 /*    if (send_msg(fd_client, buf_out, strnlen(buf_out, sizeof(buf_out))) != TRUE) { */
 /*      fatal("write error: %s", strerror(errno)); */
 /*    } */
 /* } */

	remove_store(st);
	disconnect_pipe(fd_client);
	destroy_pipe(fd_server, 0);
        
	return 0;
}
