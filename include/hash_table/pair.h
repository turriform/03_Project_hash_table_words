#if !defined(LOCAL_PAIR_H)
#define LOCAL_PAIR_H
#include "framework.h"

typedef struct KeyValue
{
    size_t key;
    size_t jumps;
    size_t count;
    char *value;
} key_value_t;

typedef struct Bucket
{
    struct Bucket *self;

    bool existed;
    bool exists;

    key_value_t key_value;
    void (*dtor)(struct Bucket *);

    void (*hash)(struct Bucket *, size_t, char *);
    void (*hash_jump)(struct Bucket *, size_t);
    void (*print)(struct Bucket *);
    void (*on_delete)(struct Bucket *);

} bucket_t;

size_t hash_polinomial(size_t table_size, char *value)
{

    size_t init_factor = 1, a = 97;
    size_t i = 0, res = 0;
    char *str = value;
    while (str[i])
    {
        res += (init_factor * (size_t)str[i]) % table_size;
        init_factor = (init_factor * a) % table_size;
        i++;
    }
    return res % table_size;
}

void bucket_destroy(bucket_t *self)
{
    free(self);
}

void bucket_update_on_delete(bucket_t *self)
{
    self->exists = false;
    self->key_value.value = "";
    self->key_value.jumps = 0;
    self->key_value.count = 0;
}

void bucket_calculate_hash(bucket_t *self, size_t table_size, char *value)
{
    size_t hash = hash_polinomial(table_size, value);

    self->key_value.key = hash;
}

void bucket_print(bucket_t *self)
{
    printf("Bucket: \n");

    printf("| Jumps: %ld | ", self->key_value.jumps);
    printf("Count: %ld | ", self->key_value.count);
    printf("Exists: %s | ", self->exists ? "Yes" : "No");
    printf("Existed: %s || ", self->existed ? "Yes" : "No");
    printf("Key:\t%ld | ", self->key_value.key);
    printf("Value:\t%s | ", self->key_value.value?self->key_value.value:"N\\A");
    printf("\n");
}

void bucket_hash_jump(bucket_t *self, size_t table_size)
{
    self->key_value.jumps++;
    self->key_value.key++;
    self->key_value.key %= table_size;
}

bucket_t *bucket_create(size_t key)
{
    bucket_t *bucket = (bucket_t *)malloc(sizeof(bucket_t));
    bucket->self = bucket;
    bucket->existed = false;
    bucket->exists = false;
    bucket->key_value.jumps = 0;
    bucket->key_value.count = 0;
    bucket->key_value.key = key;
    bucket->key_value.value = "";

    bucket->dtor = bucket_destroy;
    bucket->hash = bucket_calculate_hash;
    bucket->print = bucket_print;
    bucket->on_delete = bucket_update_on_delete;

    return bucket;
}

#endif // _LOCAL_PAIR_H
