#include "config.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

//this partc function is used by server, takes no arguments but returns int - file descriptor number
int create_server_pipe(){
  char pipe_name[] = "/tmp/server.pipe";
  int fd_server;
  (void)umask(0);

  if(mknod(pipe_name, S_IFIFO | 0666, 0) < 0){
    printf("Can't create server pipe!\n");
    exit(-1);
  }

  if((fd_server = open(pipe_name,O_RDONLY)) < 0){
    printf("Can't open server pipe for reading!\n");
    exit(-1);
  }
  return fd_server;
}

//this function is used by client, takes PID as an argument to create a proper name for pipe object, it also returns int - file descriptor number of newly created pipe
int create_client_pipe(pid_t pid){
/*
  have to create pipe file object based on PID of client for read:
   /tmp/client.<PID>.fifo
 -  don't know how to pass func argument to it yet
 */
  char pipe_name[] = "/tmp/client.";
  char postfix[] = ".pipe";
  sprintf(pipe_name, "%d", pid);
  strcat(pipe_name,postfix);

  int fd_client;
  (void)umask(0);

  if(mknod(pipe_name, S_IFIFO | 0666, 0) < 0){
     error("Cannot create client pipe: %s\n", strerror(errno));
    return -1;
  }

  if((fd_client = open(pipe_name,O_RDONLY)) < 0){
    error("Can't open client pipe for reading: %s\n", strerror(errno))
    return -1;
  }
  
  return fd_client;
}

//As I understand it: there should be a call which returns the server pipe fd - to the client and the client pipe fd - to the server by the path name of one of them
static int connect_pipe(char *path){

  int fd;
  if((fd = open(path,O_WRONLY)) < 0){
    printf("Can't open pipe for sending data!\n");
    exit(-1);
  }
  
  return fd;
}

//This func is used by the client only. It takes no args but returns int - filde descriptor number of the server pipe, if the call was successfull
int connect_to_server(void){
	int fd;
	warning("stub");
	fd = connect_pipe("/tmp/server.pipe");
	if (fd < 0) {
          error("unable to connect: %s", strerror(errno));
          return -1;
	}

	return fd;
}

//This func is used by the server only. It takes PID as an  arg and returns int - file descriptor number of the client's pipe, if the call was successfull
int connect_to_client(pid_t pid){
  char path[] = "/tmp/client.";
  char postfix[] = ".pipe";
  sprintf(path, "%d", pid);
  strcat(path,postfix);

  int fd;
  fd = connect_pipe(path);
  if (fd < 0) {
    error("unable to connect to %s: %s", path, strerror(errno));
    return FALSE;
  }
  return fd;
}

//Function to disconnect pipes from each other. Takes fd as an argument, returns nothing
int disconnect_pipe(int fd){

  if(close(fd) < 0){
    error("Can't close the pipe: %s\n", strerror(errno));
    return -1;
  }

  printf("The pipe is closed!\n");
  return TRUE;
}

//Function is used both server and client to destroy their pipe objects
int destroy_pipe(int fd, pid_t pid){
  char server_pipe[] = "/tmp/server.pipe";
  char client_pipe[] = "/tmp/client.";
  char postfix[] = ".pipe";


  if (fd > 0)
    disconnect_pipe(fd);

  if (pid == 0) {
    unlink(server_pipe);
  } else {
    sprintf(client_pipe, "%d", pid);
    strcat(client_pipe,postfix);
    
    unlink(client_pipe);
  }
  return TRUE;
}

int
send_msg(int fd, char *buf, size_t buf_len)
{
	int count;
	char data[MAX_STRING_LENGTH + 10] = {0};

	/* add PID prefix (10 bytes) to every message
	 * that allows to distinguish clients' messages
	 */
	sprintf(data, "%08d: ", getpid());
	memcpy(data + 10, buf, buf_len);

	count = write(fd, data, buf_len + 10);
	if (count < 0) {
		error("error on write");
		return FALSE;
	}

	if (count != (buf_len + 10)) {
		warning("written %d of %lu bytes", count, buf_len);
		return FALSE;
	}

	return TRUE;
}

int
recv_msg(int fd, char *buf, size_t buf_len)
{
	int count;

	/* FIXME: read from stdin */

	/* cleanup buffer */
	memset(buf, 0, buf_len);

	count = read(fd, buf, buf_len);
	if (count < 0) {
		error("error on read");
		return FALSE;
	}

	if (count == 0) {
		debug("EOF");
		return FALSE;
	}

	return TRUE;
}

void init(void){
	debug("Library for communication loaded");
}

void fini(void){
	debug("Library for communication unloaded");
}
