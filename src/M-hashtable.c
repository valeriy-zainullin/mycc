#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emmintrin.h>
#include <nmmintrin.h>

#define MAY_BE_UNUSED __attribute__((unused))

enum ht_state {
	HT_EMPTY = 0,
	HT_DELETED,
	HT_BUSY
};

#define MAX_STR_LEN 31
#define MAX_STR_LEN_STR "31"
struct ht_item {
	char key[MAX_STR_LEN + 1];
	char value[MAX_STR_LEN + 1];
};

struct ht {
	struct ht_item*  data;
	char*            state;
	size_t           capacity;
	size_t           size;

	size_t           hits;
	size_t           total;
	
	bool             dbg_info;
	char const *     filename;
	char const *     function;
	size_t           line;
} _uninitialized_ht;

static bool ht_check_equal(__typeof__(_uninitialized_ht.data) lhs, __typeof__(_uninitialized_ht.data) rhs) {
	static const size_t checked_part_size = sizeof(lhs->key); // sizeof(*lhs);
	
	return memcmp(lhs, rhs, checked_part_size) == 0;
}

static size_t ht_primary_hash(__typeof__(_uninitialized_ht.data) item) {
	unsigned char* byte_ptr = (unsigned char*) item;
	
	static const size_t hashed_part_size = sizeof(item->key); // sizeof(*item);

	unsigned hash = 0;
	for (size_t i = 0; i < hashed_part_size; ++i) {
		// hash = _mm_crc32_u8(hash, byte_ptr[i]);
		__asm__ (
			"crc32b %2, %%eax"
			: "=a"(hash)
			: "a"(hash), "m"(byte_ptr[i])
		);
	}
	
	unsigned rehash = hash;
	for (size_t i = 0; i < hashed_part_size; ++i) {
		// rehash = _mm_crc32_u8(rehash, byte_ptr[i]);
		__asm__ (
			"crc32b %2, %%eax"
			: "=a"(rehash)
			: "a"(rehash), "m"(byte_ptr[i])
		);
	}

	return ((size_t) hash << 32) | rehash;
}

static size_t ht_secondary_hash(__typeof__(_uninitialized_ht.data) value) {
	return ht_primary_hash(value);
}

MAY_BE_UNUSED static void ht_dump(struct ht const* ht) {
	assert(ht != NULL);
	
	for (size_t i = 0; i < ht->capacity; ++i) {
		if (ht->state[i] == HT_BUSY) {
			printf("<%s,%s> ", ht->data[i].key, ht->data[i].value);
		}
	}
	printf("\n");
}


static struct ht* ht_new(size_t capacity) {
	struct ht* ht = calloc(1, sizeof(struct ht));
	if (ht == NULL) {
		return NULL;
	}
    
	ht->data  = calloc(capacity, sizeof(*ht->data));
	ht->state = calloc(capacity, sizeof(*ht->state));
	ht->capacity = capacity;
    
	if (ht->data == NULL || ht->state == NULL) {
		free(ht->data);
		free(ht->state);
		free(ht);
    
		return NULL;
	}

	return ht;
}

MAY_BE_UNUSED static struct ht* ht_new_dbg(size_t capacity, const char* filename, const char* function, size_t line) {
	struct ht* ht = ht_new(capacity);
	
	if (ht != NULL) {
		ht->dbg_info = true;
		ht->filename = filename;
		ht->function = function;
		ht->line = line;
	}
	
	return ht;
}

#define ht_new_dbg(capacity) ht_new_dbg(capacity, __FILE__, __func__, __LINE__)

static struct ht* ht_delete(struct ht* ht) {
	if (ht == NULL) {
		return ht;
	}
	
	if (ht->dbg_info) {
		fprintf(stderr, "%s, %s:%zu ", ht->function, ht->filename, ht->line);
	}
	fprintf(stderr, "hits=%zu total=%zu\n", ht->hits, ht->total);
	
	free(ht->data);
	free(ht->state);
	free(ht);

	return NULL;
}

MAY_BE_UNUSED static size_t ht_size(struct ht const* ht) {
	assert(ht != NULL);
	return ht->size;
}

static __typeof__(_uninitialized_ht.data) ht_find(struct ht * ht, __typeof__(_uninitialized_ht.data) item);
static __typeof__(_uninitialized_ht.data) ht_insert(struct ht* ht, __typeof__(_uninitialized_ht.data) item) {
	assert(ht != NULL);
	assert(ht->size + ht->size <= ht->capacity);
	
	{
		__typeof__(_uninitialized_ht.data) result = ht_find(ht, item);
		if (result != NULL) {
			return result;
		}
	}
	
	for (
		size_t it = ht_primary_hash(item) % ht->capacity, inc = ht_secondary_hash(item), tries = 0; 
		tries != ht->capacity;
		it = (it + inc) % ht->capacity, ++tries
	) {
		if (ht->state[it] != HT_BUSY) {
			ht->data[it] = *item;
			ht->state[it] = HT_BUSY;
			
			ht->total += tries + 1;
			ht->hits += 1;
			ht->size += 1;

			return &ht->data[it];
		}
	}
	
	return NULL;
}

static __typeof__(_uninitialized_ht.data) ht_find(struct ht * ht, __typeof__(_uninitialized_ht.data) item) {
	assert(ht != NULL);
	assert(ht->size + ht->size <= ht->capacity);

	for (
		size_t it = ht_primary_hash(item) % ht->capacity, inc = ht_secondary_hash(item), tries = 0;
		tries != ht->capacity;
		it = (it + inc) % ht->capacity, tries++
	) {
		switch (ht->state[it]) {
		case HT_BUSY: {
			if (ht_check_equal(&ht->data[it], item)) {
				ht->total += tries + 1;
				ht->hits += 1;
				return &ht->data[it];
			}
		} 
		break;
		
		case HT_EMPTY: {
			ht->total += tries + 1;
			ht->hits += 1;
		
			return NULL;
		}
		break;
		
		case HT_DELETED: {
			continue;
		}
		break;
		
		default: assert(0);
		}
	}
	return NULL;
}

MAY_BE_UNUSED static void ht_erase(struct ht* ht, __typeof__(_uninitialized_ht.data) item) {
	assert(ht != NULL);
	assert(ht->size + ht->size <= ht->capacity);

	for (
		size_t it = ht_primary_hash(item) % ht->capacity, inc = ht_secondary_hash(item), tries = 0;
		tries != ht->capacity;
		it = (it + inc) % ht->capacity, ++tries
	) {
		switch (ht->state[it]) {
		case HT_BUSY: {
			if (ht_check_equal(&ht->data[it], item)) {
				ht->total += tries + 1;
				ht->hits += 1;
				ht->state[it] = HT_DELETED;
				
				return;
			}
		}
		break;
		
		case HT_EMPTY: {
			ht->total += tries + 1;
			return;
		}
		break;
		
		case HT_DELETED: {
			continue;
		}
		break;
		
		default: assert(0);
		}
	}

	return;
}

#define VERIFY(EXPR) if (!(EXPR)) { printf("File %s, line %d: expr %s is false. Program was terminated.\n", __FILE__, __LINE__, #EXPR); abort(); }

#define MAX_HASHTABLE_INDEX 9
struct query {
	enum {
		QUERY_CREATE,
		QUERY_INSERT,
		QUERY_ERASE,
		QUERY_FIND,
		QUERY_DESTROY
	} type;

	size_t ht_index;
	
	char key[MAX_STR_LEN + 1];
	char value[MAX_STR_LEN + 1];
};

static void solve(bool(* get_next_query)(struct query* query), void (*send_string)(char const*)) {
	struct ht* ht[MAX_HASHTABLE_INDEX + 1] = {0};
	
	static const size_t MAX_NUM_INSERTIONS = 100000;
	
	struct query query = {0};
	while (get_next_query(&query)) {
		if (query.type == QUERY_CREATE) {
			VERIFY(ht[query.ht_index] == NULL);
			ht[query.ht_index] = ht_new_dbg(10 * MAX_NUM_INSERTIONS);
			VERIFY(ht[query.ht_index] != NULL);
		} else if (query.type == QUERY_INSERT) {
			if (ht[query.ht_index] == NULL) {
				continue;
			}
			
			struct ht_item kv = {0};
			strncpy(kv.key, query.key, MAX_STR_LEN + 1);
			strncpy(kv.value, query.value, MAX_STR_LEN + 1);
			
			struct ht_item* kv_ht = ht_find(ht[query.ht_index], &kv);
			if (kv_ht == NULL) {
				ht_insert(ht[query.ht_index], &kv);
			} else {
				strncpy(kv_ht->value, query.value, MAX_STR_LEN);
				VERIFY(kv_ht->value[MAX_STR_LEN] == '\0');
			}
		} else if (query.type == QUERY_ERASE) {
			struct ht_item kv = {0};
			strncpy(kv.key, query.key, MAX_STR_LEN + 1);
			strncpy(kv.value, query.value, MAX_STR_LEN + 1);

			ht_erase(ht[query.ht_index], &kv);
		} else if (query.type == QUERY_FIND) {
			if (ht[query.ht_index] == NULL) {
				send_string("MISSING");
				continue;
			}
			
			struct ht_item kv = {0};
			strncpy(kv.key, query.key, MAX_STR_LEN + 1);
			strncpy(kv.value, query.value, MAX_STR_LEN + 1);

			struct ht_item* kv_ht = ht_find(ht[query.ht_index], &kv);
			
			if (kv_ht == NULL) {
				send_string("MISSING");
			} else {
				send_string(kv_ht->value);
			}
		} else if (query.type == QUERY_DESTROY) {
			if (ht[query.ht_index] == NULL) {
				continue;
			}
			
			ht[query.ht_index] = ht_delete(ht[query.ht_index]);
		} else {
			// Programming error. Should not read such a query.
			assert(false);
		}
	}
	
	for (size_t i = 0; i <= MAX_HASHTABLE_INDEX; ++i) {
		ht[i] = ht_delete(ht[i]);
	}
}

// We could do # macro to make a string out of MAX_CMD_LEN, but it requires
// another macro function, don't want to bother, it's 1:25 am, I'm tired.
#define MAX_CMD_LEN 10
#define MAX_CMD_LEN_STR "10"
static bool get_next_query(struct query* query) {
	char command[MAX_CMD_LEN + 1] = {0};
	if (scanf("%" MAX_CMD_LEN_STR "s", command) != 1) {
		return false;
	}
	
	memset(query, 0, sizeof(*query));
	
	if (strcmp(command, "create") == 0) {
		query->type = QUERY_CREATE;
		scanf("%zu", &query->ht_index);
	} else if (strcmp(command, "insert") == 0) {
		query->type = QUERY_INSERT;
		scanf("%zu %" MAX_STR_LEN_STR "s %" MAX_STR_LEN_STR "s", &query->ht_index, query->key, query->value);
	} else if (strcmp(command, "erase") == 0) {
		query->type = QUERY_ERASE;
		scanf("%zu %" MAX_STR_LEN_STR "s", &query->ht_index, query->key);
	} else if (strcmp(command, "find") == 0) {
		query->type = QUERY_FIND;
		scanf("%zu %" MAX_STR_LEN_STR "s", &query->ht_index, query->key);
	} else if (strcmp(command, "destroy") == 0) {
		query->type = QUERY_DESTROY;
		scanf("%zu", &query->ht_index);
	} else {
		VERIFY(false);
	}
	
	return true;
}

static void send_string(char const* string) {
	printf("%s\n", string);
}

int main() {
	solve(get_next_query, send_string);	

	return 0;
}