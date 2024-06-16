#include "config.h"

#include <string.h>

#include "libstore.h"

#define STORE_SIZE 100 /* *2 */

static int test_fill(store *st)
{
	for (int i = 0; i < STORE_SIZE; i++) {
		char key_str[MAX_STRING_LENGTH] = {0};
		char key_num[MAX_STRING_LENGTH] = {0};
		char value_str[MAX_STRING_LENGTH] = {0};
		char value_num[MAX_STRING_LENGTH] = {0};
		char *value = NULL;

		snprintf(key_num, sizeof(key_num), "number-%d", i);
		snprintf(value_num, sizeof(value_num), "%d", i);
		value = get_value_by_key(st, key_num);
		if (value == NULL) {
			warning("not found value for key: %s", key_num);
			return FALSE;
		}
		if (strcmp(value, value_num) != 0) {
			warning("read value key %s = %s, expected %s", key_num, value, key_num);
			free(value);
			return FALSE;
		}
		free(value);

		snprintf(key_str, sizeof(key_str), "string-%d", i);
		snprintf(value_str, sizeof(value_str), "Test string %d", i);
		value = get_value_by_key(st, key_str);
		if (value == NULL) {
			warning("not found value for key: %s", key_str);
			return FALSE;
		}
		if (strcmp(value, value_str) != 0) {
			warning("read value key %s = %s, expected %s", key_str, value, value_str);
			free(value);
			return FALSE;
		}
		free(value);
	}

	return TRUE;
}

static int
test_remove_val(store *st)
{
	for (int i = 0; i < STORE_SIZE; i++) {
		char key[MAX_STRING_LENGTH] = {0};
		char *value = NULL;

		(i % 2) ? snprintf(key, sizeof(key), "number-%d", i)
			: snprintf(key, sizeof(key), "string-%d", i);

		value = get_value_by_key(st, key);
		if (value != NULL) {
			warning("found unexpected value for key %s = %s", key, value);
			free(value);
			return FALSE;
		}

		(i % 2) ? snprintf(key, sizeof(key), "string-%d", i)
			: snprintf(key, sizeof(key), "number-%d", i);
		value = get_value_by_key(st, key);
		if (value == NULL) {
			warning("unable to find key %s", key);
			return FALSE;
		}
		free(value);
	}

	return TRUE;
}

int
main(void)
{
	store *st = create_store();

	if (st == NULL) {
		fatal("unable to initialize store");
	}

	for (int i = 0; i < STORE_SIZE; i++) {
		char key_str[MAX_STRING_LENGTH] = {0};
		char key_num[MAX_STRING_LENGTH] = {0};
		char value[MAX_STRING_LENGTH] = {0};

		snprintf(key_num, sizeof(key_num), "number-%d", i);
		add_number(st, key_num, i);

		snprintf(key_str, sizeof(key_str), "string-%d", i);
		snprintf(value, sizeof(value), "Test string %d", i);
		add_string(st, key_str, value);
	}

	//	dump_store(st);

	if (test_fill(st) != TRUE)
		fatal("incorrect data in store");

	debug("test data added and read correctly");

	for (int i = 0; i < STORE_SIZE; i++) {
		char key[MAX_STRING_LENGTH] = {0};
		(i % 2) ? snprintf(key, sizeof(key), "number-%d", i)
			: snprintf(key, sizeof(key), "string-%d", i);
		remove_key(st, key);
	}
	if (test_remove_val(st) != TRUE)
		fatal("incorrect value removal");

	debug("test data removed correctly");

	if (remove_store(st) != TRUE)
		fatal("incorrect store removal");

	debug("all tests passed");
	return 0;
}
