#pragma once

#define KEY_LEN 32
#define VAL_LEN 128
#define INITIAL_CAPACITY 64
#define FILL_FACTOR 0.75

enum hash_status
{
	UNUSED,
	ISUSED,
	DELETED
};

typedef struct
{
	char key[KEY_LEN];
	char value[VAL_LEN];
	short status;

} Entry_t;

typedef struct
{
	size_t capacity;
	size_t size;
	Entry_t table[];

} HashMap_t;

int map_init(HashMap_t *handle[static 1], size_t capacity);
void map_free(HashMap_t *m);
int map_set(HashMap_t *handle[static 1], const char *key, const char *val);
Entry_t *map_get(HashMap_t *m, const char *key);
int map_delete(HashMap_t *m, const char *key);
void map_print(HashMap_t *m);
