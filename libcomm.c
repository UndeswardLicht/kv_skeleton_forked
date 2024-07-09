#include "config.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "libcomm.h"

/* HINT: __maybe__ easier to use FILE steam for communication, instead
 * of low-level descriptors, since the stream is based on strings.
 * i.e. popen()/fscanf()/fprintf()/fclose()
 * Needs API changing.
 **/

int
create_server_pipe()
{
  char pipe_name[] = "/tmp/server.pipe";
  int fd_server;
  (void)umask(0);

  if(mkfifo(pipe_name, S_IFIFO | 0666) < 0){
    error("Can't create server pipe!\n");
    debug("Failed to create server pipe");
    exit(-1);
  }
  fd_server = connect_to_server();
  debug("Sr: Server pipe created");
  return fd_server;
}

int
create_client_pipe(pid_t pid)
{
  char pipe_name[] = "/tmp/client.";
  char postfix[] = ".pipe";
  sprintf(pipe_name, "%d", pid);
  strcat(pipe_name,postfix);

  int fd_client;
  (void)umask(0);

  if(mkfifo(pipe_name, S_IFIFO | 0666) < 0){
     error("Cannot create client pipe: %s\n", strerror(errno));
     debug("Failed to create client pipe");
     exit(-1);
  }
  debug("Cl: Client pipe created");
  return TRUE;
}

static int
connect_pipe(char *path)
{
  int fd;
  if((fd = open(path,O_RDWR)) < 0){
    printf("Can't open pipe '%s' for reading|writing data! Errno: %d\n", path,errno);
    exit(-1);
  }
  printf("Libcm: opened pipe '%s' for writing!\n",path);
  return fd;
}

int
connect_to_server(void)
{
	int fd;
	fd = connect_pipe("/tmp/server.pipe");
	if (fd < 0) {
          error("unable to connect: %s", strerror(errno));
          return -1;
	}

	return fd;
}

int
connect_to_client(pid_t pid)
{
  char path[] = "/tmp/client.";
  char postfix[] = ".pipe";
  sprintf(path, "%d", pid);
  strcat(path,postfix);

  int fd;
  fd = connect_pipe(path);
  if (fd < 0) {
    error("unable to connect to %s: %s\n", path, strerror(errno));
    return FALSE;
  }
  return fd;
}

int
disconnect_pipe(int fd)
{
  if(close(fd) < 0){
    error("Can't close the pipe: %s\n", strerror(errno));
    return -1;
  }

  printf("The pipe is closed!\n");
  return TRUE;
}

int
destroy_pipe(int fd, pid_t pid)
{
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



void
init(void)
{
	debug("Library for communication loaded");
}

void
fini(void)
{
	debug("Library for communication unloaded");
}
