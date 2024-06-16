/* Simple and ineffective KV-store implementation.
 * Just an example.
 */
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libstore.h"

store *
create_store()
{
	/* initialization of the first empty element */
	store element = {.next = NULL, .key = NULL, .type = EMPTY, .val.str = NULL};

	store *s = malloc(sizeof(store));

        debug("Store created");
	return memcpy(s, &element, sizeof(store));
}

int
remove_store(store *s)
{
	do {
		store *next = s->next;
		if (s->type == STRING)
			free(s->val.str);
		free(s->key);
		s->key = NULL;
		free(s);
		s = next;
	} while (s != NULL);

	return TRUE;
}

int
dump_store(store *s)
{
	unsigned long cnt = 0;
	if (s == NULL)
		return TRUE;

	while (s->next != NULL) {
		s = s->next;
		switch (s->type) {
		case NUMBER:
			info("element %ld: key=%s, number=%d", cnt, s->key, s->val.num);
			break;
		case STRING:
			info("element %ld: key=%s, string=\"%s\"", cnt, s->key, s->val.str);
			break;
		case EMPTY:
			info("element %ld: empty", cnt);
			break;
		default:
			error("element %ld: unknown", cnt);
			break;
		};
		cnt++;
	}
	return TRUE;
}

int
add_number(store *s, char *key, int value)
{
	store element = {.next = s->next, .key = strdup(key), .type = NUMBER, .val.num = value};
	store *tmp = malloc(sizeof(store));

	s->next = memcpy(tmp, &element, sizeof(store));

	return TRUE;
}

int
add_string(store *s, char *key, char *value)
{
	store element = {.next = s->next,
			 .key = strdup(key),
			 .type = STRING,
			 .val.str = strdup(value)};
	store *tmp = malloc(sizeof(store));

	s->next = memcpy(tmp, &element, sizeof(store));

	return TRUE;
}

int
remove_key(store *s, char *key)
{
	/* TODO: remove the key instead of just marking. */
	while (s->next != NULL) {
		s = s->next;

		if (s->key == NULL)
			continue;

		if (strcmp(s->key, key) == 0) {
			switch (s->type) {
			case STRING:
				free(s->val.str);
				s->val.str = NULL;
				break;
			case NUMBER:
				s->val.num = 0;
				break;
			default:
				error("empty element with key \"%s\"", key);
				return FALSE;
			}
			s->type = EMPTY;
			free(s->key);
			s->key = NULL;
		}
	}

	return TRUE;
}

char *
get_value_by_key(store *s, char *key)
{
	char tmp[MAX_STRING_LENGTH] = {0};
	while (s->next != NULL) {
		s = s->next;
		if (s->key == NULL)
			continue;

		if (strcmp(s->key, key) == 0) {
			switch (s->type) {
			case STRING:
				return strdup(s->val.str);
			case NUMBER:
				snprintf(tmp, sizeof(tmp), "%d", s->val.num);
				return strdup(tmp);
			default:
				error("empty element with key \"%s\"", key);
				return NULL;
			}
		}
	}
	return NULL;
}

void
init(void)
{
	debug("Library for the store loaded");
}

void
fini(void)
{
	debug("Library for the store unloaded");
}
