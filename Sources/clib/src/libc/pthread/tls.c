#include <pthread.h>
#include <errno.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define PTHREAD_KEYS_MAX 128

typedef struct {
    int used;
    void (*destructor)(void*);
} tls_key_t;

static tls_key_t tls_keys[PTHREAD_KEYS_MAX];
static __thread void* tls_values[PTHREAD_KEYS_MAX];

int pthread_key_create(pthread_key_t* key, void (*destructor)(void*)) {
    if (!key)
        return EINVAL;

    for (int i = 0; i < PTHREAD_KEYS_MAX; i++) {
        if (!tls_keys[i].used) {
            tls_keys[i].used = 1;
            tls_keys[i].destructor = destructor;
            *key = (pthread_key_t)i;
            return 0;
        }
    }

    return EAGAIN;
}

int pthread_key_delete(pthread_key_t key) {
    if (key >= PTHREAD_KEYS_MAX || !tls_keys[key].used)
        return EINVAL;

    tls_keys[key].used = 0;
    tls_keys[key].destructor = NULL;
    tls_values[key] = NULL;
    return 0;
}

void* pthread_getspecific(pthread_key_t key) {
    if (key >= PTHREAD_KEYS_MAX || !tls_keys[key].used)
        return NULL;

    return tls_values[key];
}

int pthread_setspecific(pthread_key_t key, const void* value) {
    if (key >= PTHREAD_KEYS_MAX || !tls_keys[key].used)
        return EINVAL;

    tls_values[key] = (void*)value;
    return 0;
}
