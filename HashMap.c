#include "HashMap.h"
#include <stdio.h>


/**
 * @brief FNV-1a 32 bit hash function for fast index computation
 * @param key const char*: byte stream to compute hash for   
 */
static size_t hash(const char* key)
{
	size_t h = 2166136261u;
	while (*key) {
		h ^= (unsigned char)*key++;
		h *= 16777619u;
	}

	fflush(stdout);
	return h;
}

/**
 * @brief Resizes the hashmap capacity. Not exposed to user.
 * @param m HashMap to resize
 * @param new_capacity
 */
static int map_resize(HashMap_t** m, size_t new_capacity)
{
	printf("Map Resize triggered...\n");
	HashMap_t* new = {0};
	map_init(&new, new_capacity);

	// possible pointer ivalidation of 'old_table'?
	// m = calloc(sizeof(HashMap_t) + new_capacity * sizeof(Entry_t), 1);

	// if (!m) {
	// 	return 0;  // failed resize
	// }

	// m->capacity = new_capacity;
	// map_free(m);
	

	for (size_t i = 0; i < (*m)->capacity; i++) {
		if ((*m)->table[i].in_use) {
			// reinsert into new table
			map_set(new, (*m)->table[i].key, (*m)->table[i].value);
		}
	}

	free(*m);
	m = &new;

	return 1;  // success
}


int map_init(HashMap_t* m[static 1], size_t capacity)
{
	// ensure one contiguous block of memory and less memory fragmentation
	if (*m != NULL) { return 0; }

	capacity = capacity <= 0 ? INITIAL_CAPACITY : capacity;

	*m = malloc(sizeof(**m) + sizeof(Entry_t[capacity]));

	if (*m != NULL) {
		**m = (HashMap_t){ .capacity = capacity };
	}

	// printf("capacity = %ld\n", (**m).capacity);
	// printf("size = %ld\n", (*m)->size);
	// printf("table = %p\n", &(*m)->table);
	// printf("m = %p\n", *m);

	return 1;
}


void map_free(HashMap_t* m)
{
	// For this specific implementation, we used a single malloc
	// with the flexible array member, we technically don't need a seperate function
	// But it's ok to be agnostic to implementation detail and provide a consistent name
	free(m);
}


int map_set(HashMap_t* m, const char* key, const char* val)
{
	if ((m->size + 1) > (size_t)(m->capacity * FILL_FACTOR)) {
		int status = map_resize(&m, m->capacity * 2);
		if (!status) {
			return status;
		}
	}

	size_t idx = hash(key);

	for (size_t i = 0; i < m->capacity; i++) {
		size_t probe = (idx + i) % m->capacity;
		Entry_t* e = &m->table[probe];

		// Insert in unused slot
		if (!e->in_use) {
			strncpy(e->key, key, sizeof(e->key));
			e->key[KEY_LEN - 1] = '\0';
			printf("e->key %s | key %s | ", e->key, key);
			strncpy(e->value, val, sizeof(e->value));
			e->value[VAL_LEN - 1] = '\0';
			printf("e->val %s | val %s\n", e->value, val);
			e->in_use = 1;
			m->size++;
			return 1;
		}

		// Update existing key
		if (strcmp(e->key, key) == 0) {
			strncpy(e->value, val, sizeof(e->value));
			e->value[VAL_LEN - 1] = '\0';
			return 1;
		}
	}

	// Entry could not be set
	return 0;
}


Entry_t* map_get(HashMap_t* m, const char* key)
{
	size_t idx = hash(key);

	for (size_t i = 0; i < m->capacity; i++) {
		size_t probe = (idx + i) % m->capacity;
		printf("idx = %ld ...",probe);
		fflush(stdout);
		Entry_t* e = &m->table[probe];

		// Could happen, if entry was deleted
		// A different key matching this index can now overwrite
		if (!e->in_use) {
			return NULL;
		};

		if (e->in_use && strcmp(e->key, key) == 0)
			return e;
	}

	return NULL;
}


int map_delete(HashMap_t* m, const char* key)
{
	size_t idx = hash(key);

	for (size_t i = 0; i < m->capacity; i++) {
		size_t probe = (idx + i) % m->capacity;
		Entry_t* e = &m->table[probe];

		if (!e->in_use) {
			return 0;
		}

		if (e->in_use && strcmp(e->key, key) == 0) {
			e->in_use = 0;
			for (size_t i = 0; i<VAL_LEN; i++) e->value[i] = '\0';
			m->size--;
			return 1;
		}
	}
	return 0;  // Deletion failed
}

void map_print(HashMap_t* m)
{
	puts("--- Contents of map ---");
	printf("|cap = %lu | size = %lu|\n", m->capacity, m->size);

	for(size_t i = 0; i < m->capacity; i++)
	{
		if (m->table[i].in_use) {
			printf("'%.*s': '%.*s'\n", (int)KEY_LEN, m->table[i].key, (int)VAL_LEN, m->table[i].value);
		}
	}

	puts("--- End of map ---");

	return;
}
