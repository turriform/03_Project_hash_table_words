#if !defined(L_HASH_TABLE_H)
#define L_HASH_TABLE_H
#include "framework.h"
#include "pair.h"

/**

 HashTable
 1) size - current size
 2) filled - current existing buckets
 3) max_jumps - max jumps that bucket has to make to find the spot
 4) load_factor  - load indicator, used to determine when to resize
 5) buckets - list of all buckets


**/

typedef struct HashTable
{
    struct HashTable *self;

    size_t size;
    size_t filled;
    size_t max_jumps;
    double load_factor;
    bucket_t **buckets;

    // pointer to bucket insert function that checks table capacity
    void (*fn_insert)(struct HashTable *, char *);

    // pointer to bucket insert function
    bucket_t *(*fn_bucket_insert)(struct HashTable *, char *); // table, value

    // pointer to bucket get function
    bucket_t *(*fn_bucket_get)(struct HashTable *, char *);

    // pointer to bucket delete function
    void (*fn_delete)(struct HashTable *, char *);

    // pointer to Hash Table resize
    void (*fn_resize)(struct HashTable *);

    void (*fn_print)(struct HashTable *);

    bool (*fn_spot_available)(struct HashTable *, size_t);

    bool (*fn_value_match)(struct HashTable *, size_t, char *);

    bool (*fn_existed)(struct HashTable *, size_t);

    void (*fn_dtor)(struct HashTable *);

    void (*fn_update_capacity)(struct HashTable *);

} hash_table_t;

/// declarations

void hash_table_destroy(hash_table_t *self);

void hash_table_print(hash_table_t *self);

void hash_table_update_capacity(hash_table_t *self);

bool hash_table_spot_available(hash_table_t *self, size_t key);

bool hash_table_spot_existed(hash_table_t *self, size_t key);

bool hash_table_value_match(hash_table_t *self, size_t key, char *value);

bucket_t *
hash_table_bucket_get(hash_table_t *hash_table, char *value);

void hash_table_bucket_delete(hash_table_t *hash_table, char *value);

bucket_t *
hash_table_bucket_insert(hash_table_t *hash_table, char *value);

void hash_table_insert(hash_table_t *self, char *val);

void hash_table_resize(hash_table_t *self);

///

void hash_table_update_capacity(hash_table_t *self)
{
    self->filled++;
    self->load_factor = (double)self->filled / (double)self->size;
}

bool hash_table_spot_available(hash_table_t *self, size_t key)
{
    return self->buckets[key]->exists == false;
}

bool hash_table_spot_existed(hash_table_t *self, size_t key)
{
    return self->buckets[key]->existed == true;
}

bool hash_table_value_match(hash_table_t *self, size_t key, char *value)
{
    // TODO: might have to use memcmp
    size_t len = strlen(value);
    // bool res = self->buckets[key]->key_value.value == value;
    bool res = (strncmp(self->buckets[key]->key_value.value, value, len) == 0);
    return res;
}

bucket_t *hash_table_bucket_get(hash_table_t *hash_table, char *value)
{
    size_t key = hash_polinomial(hash_table->size, value);
    size_t jumps = hash_table->max_jumps;

    while (jumps--)
    {
        if (hash_table->fn_value_match(hash_table->self, key, value))
        {
            printf("Found Bucket \n");

            return hash_table->buckets[key];
        }

        key++;
        key %= hash_table->size;
    }

    return NULL;
}

void hash_table_bucket_delete(hash_table_t *hash_table, char *value)
{
    bucket_t *bucket = hash_table->fn_bucket_get(hash_table->self, value);
    if (bucket == NULL)
    {
        printf("Bucket not found \n");
        return;
    }
    bucket->print(bucket);

    bucket->on_delete(bucket);
}

bucket_t *hash_table_bucket_insert(hash_table_t *hash_table, char *value)
{
    size_t key = hash_polinomial(hash_table->size, value);
    size_t jumps = 0;
    // printf("%ld \n", key);

    while (!hash_table->fn_spot_available(hash_table->self, key))
    {
        if (hash_table->fn_value_match(hash_table->self, key, value))
        {
            printf("Matched \n");
            hash_table->buckets[key]->key_value.count++;
            return hash_table->buckets[key];
        }

        key++;
        key %= hash_table->size;
        jumps++;
    }

    hash_table->buckets[key]->exists = true;
    hash_table->buckets[key]->existed = true;
    hash_table->buckets[key]->key_value.jumps = jumps;
    hash_table->buckets[key]->key_value.value = value;
    hash_table->buckets[key]->key_value.key = key;
    hash_table->buckets[key]->key_value.count++;

    hash_table->max_jumps = MAX(hash_table->max_jumps, jumps);
    hash_table->fn_update_capacity(hash_table->self);

    return hash_table->buckets[key];
}

void hash_table_insert(hash_table_t *self, char *val)
{
    if (self->load_factor > HASH_TABLE_MAX_LOAD)
    {
        hash_table_resize(self);
    }
    self->fn_bucket_insert(self, val);
}

void hash_table_resize(hash_table_t *self)
{
    // TODO: add better size function (first prime of self->size *= 2);
    size_t old_size = self->size;
    self->size *= 2;
    self->filled = 0;
    self->load_factor = 0;
    self->max_jumps = 0;

    bucket_t **old_buckets = self->buckets;

    self->buckets = (bucket_t **)calloc(self->size, sizeof(bucket_t *));
    if (self->buckets == NULL)
    {
        printf("Error occured while allocating memory\n");
        return;
    }

    // filling new buckets
    bucket_t *bucket;
    for (size_t i = 0; i != self->size; i++)
    {
        bucket = bucket_create(i);
        self->buckets[i] = bucket;
    }

    for (size_t i = 0; i != old_size; i++)
    {
        // rewriting old buckets to new
        if (old_buckets[i]->exists)
        {
            bucket_t *new_bucket = self->fn_bucket_insert(self, old_buckets[i]->key_value.value);
            new_bucket->key_value.count = old_buckets[i]->key_value.count;
        }
        old_buckets[i]->dtor(old_buckets[i]);
    }
    free(old_buckets);
}

hash_table_t *hash_table_create(size_t initial_capacity)
{

    hash_table_t *hash_table = (hash_table_t *)malloc(sizeof(hash_table_t));
    hash_table->self = hash_table;

    hash_table->buckets = (bucket_t **)calloc(initial_capacity, sizeof(bucket_t *));
    if (hash_table->buckets == NULL)
    {
        printf("Error occured while allocating memory\n");
        return NULL;
    }

    bucket_t *bucket;
    for (size_t i = 0; i != initial_capacity; i++)
    {
        bucket = bucket_create(i);
        hash_table->buckets[i] = bucket;
    }

    hash_table->size = (size_t)initial_capacity;
    hash_table->filled = 0;
    hash_table->load_factor = (double)hash_table->filled / (double)hash_table->size;
    hash_table->max_jumps = 0;

    hash_table->fn_insert = hash_table_insert;
    hash_table->fn_bucket_insert = hash_table_bucket_insert;
    hash_table->fn_print = hash_table_print;
    hash_table->fn_bucket_get = hash_table_bucket_get;
    hash_table->fn_delete = hash_table_bucket_delete;
    hash_table->fn_update_capacity = hash_table_update_capacity;
    hash_table->fn_spot_available = hash_table_spot_available;
    hash_table->fn_existed = hash_table_spot_existed;
    hash_table->fn_dtor = hash_table_destroy;
    hash_table->fn_value_match = hash_table_value_match;

    return hash_table;
}

void hash_table_destroy(hash_table_t *self)
{
    for (size_t i = 0; i != self->size; i++)
    {
        free(self->buckets[i]);
    }

    free(self->buckets);
    free(self);
}

void hash_table_print(hash_table_t *self)
{
    printf("PRINT TABLE\n");
    printf("Size: %ld \n", self->size);
    printf("Filled: %ld \n", self->filled);
    printf("Load: %.2f \n", self->load_factor);
    printf("Max Jumps (alpha): %ld \n", self->max_jumps);

    for (size_t i = 0; i != self->size; i++)
    {
        if (self->buckets[i]->exists)
        {
            self->buckets[i]->print(self->buckets[i]);
        }
    }
}

#endif