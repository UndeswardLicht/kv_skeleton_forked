#include "config.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "libcomm.h"
#include "libstore.h"

int main(void){
	store *st = create_store();
	int fd_server;
	int fd_client;
	int i = 0;
	char buf_in[MAX_STRING_LENGTH];

	if (st == NULL) {
		fatal("unable to initialize store");
	}

	/* FIXME: proper descriptors */
	fd_server = create_server_pipe();
	fd_client = connect_to_client(12345);

	/* assume blocking I/O */
	while (recv_msg(fd_server, buf_in, sizeof(buf_in)) == TRUE) {
		/* FIXME: example */
		char key[MAX_STRING_LENGTH] = {0};
		char *value = NULL;
		char buf_out[MAX_STRING_LENGTH];

		/* FIXME: parse input string here */
		debug("read: %s", buf_in);

		/* FIXME: storing whole string */
		snprintf(key, sizeof(key), "string_%d", i++);
		add_string(st, key, buf_in);

		/* FIXME: generate message to client here */
		value = get_value_by_key(st, key);
		snprintf(buf_out, sizeof(buf_out), "echo: %s = %s", key, value);
		free(value);

		if (send_msg(fd_client, buf_out, strnlen(buf_out, sizeof(buf_out))) != TRUE) {
			fatal("write error: %s", strerror(errno));
		}
	}

	remove_store(st);
	disconnect_pipe(fd_client);
	destroy_pipe(fd_server, 0);
	return 0;
}
