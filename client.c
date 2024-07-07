#include "config.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "libcomm.h"

int main(){

  pid_t client_pid;
  int fd_client;
  int fd_server;
  client_pid = getpid();

  fd_server = connect_to_server();
  debug("Cl: Connected to server pipe\n");

  //  write(fd_server, client_pid, sizeof(client_pid));
  char msg[] = "Some msg from client";
  char pid[9];
  sprintf(pid, "%d", client_pid);
  debug("My pid is %s\n", pid);


  write(fd_server, pid, sizeof(pid));
  send_msg(fd_server, msg, sizeof(msg));
  printf("msg is sent from client on srvr pipe, waiing for rspns\n");

  //  close(fd_server);

  fd_client = create_client_pipe(client_pid);
  debug("Cl: Client pipe created\n");

  char response[MAX_STRING_LENGTH];
  recv_msg(fd_client, response, sizeof(response));
  printf("response msg from server is: %s\n", response);







  destroy_pipe(fd_client, client_pid);
  disconnect_pipe(fd_server);


  return 0;
}
