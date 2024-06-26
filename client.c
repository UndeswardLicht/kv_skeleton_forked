#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//This lib is needed here because client should communicate withe server via shared library
#include "libcomm.h"

//Very high-level understanding of the client. TBC


//Entry point to the programm, it takes no arguments but returns the PID of the process, where it was created
int main(){

  pid_t client_pid;
  int fd_client;
  int fd_server;
  client_pid = getpid();


  fd_client = create_client_pipe(client_pid);
  fd_server = connect_to_server();

  //  while (input is not specific com to terminate the connection){         send_msg();
  //}



  disconnect_pipe(fd_server);
  destroy_pipe(fd_client, client_pid);


  return 0;
}
