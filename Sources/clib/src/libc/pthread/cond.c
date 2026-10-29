#include <stddef.h>
#include <pthread.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1

static int futex_wait(int* addr, int val, const struct timespec* ts) {
    return (int)SYSCALL4(__NR_futex, (long)addr, FUTEX_WAIT, val, (long)ts);
}

static int futex_wake(int* addr, int count) {
    return (int)SYSCALL3(__NR_futex, (long)addr, FUTEX_WAKE, count);
}

int pthread_condattr_init(pthread_condattr_t* attr) {
    if (!attr)
        return EINVAL;

    attr->shared = PTHREAD_PROCESS_PRIVATE;
    attr->clock = 0;
    return 0;
}

int pthread_condattr_destroy(pthread_condattr_t* attr) {
    if (!attr)
        return EINVAL;

    return 0;
}

int pthread_condattr_setpshared(pthread_condattr_t* attr, int pshared) {
    if (!attr)
        return EINVAL;

    attr->shared = pshared;
    return 0;
}

int pthread_condattr_getpshared(const pthread_condattr_t* attr, int* pshared) {
    if (!attr || !pshared)
        return EINVAL;

    *pshared = attr->shared;
    return 0;
}

int pthread_condattr_setclock(pthread_condattr_t* attr, int clock_id) {
    if (!attr)
        return EINVAL;

    attr->clock = clock_id;
    return 0;
}

int pthread_condattr_getclock(const pthread_condattr_t* attr, int* clock_id) {
    if (!attr || !clock_id)
        return EINVAL;

    *clock_id = attr->clock;
    return 0;
}

int pthread_cond_init(pthread_cond_t* cond, const pthread_condattr_t* attr) {
    if (!cond)
        return EINVAL;

    cond->shared = attr ? attr->shared : PTHREAD_PROCESS_PRIVATE;
    cond->seq = 0;
    cond->futex = 0;
    return 0;
}

int pthread_cond_destroy(pthread_cond_t* cond) {
    if (!cond)
        return EINVAL;

    return 0;
}

int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex) {
    if (!cond || !mutex)
        return EINVAL;

    unsigned int seq = cond->seq;
    pthread_mutex_unlock(mutex);
    futex_wait(&cond->futex, (int)seq, NULL);
    pthread_mutex_lock(mutex);
    return 0;
}

int pthread_cond_timedwait(pthread_cond_t* cond, pthread_mutex_t* mutex, const struct timespec* abstime) {
    if (!cond || !mutex)
        return EINVAL;

    unsigned int seq = cond->seq;
    pthread_mutex_unlock(mutex);

    int r = futex_wait(&cond->futex, (int)seq, abstime);
    pthread_mutex_lock(mutex);
    if (r == -ETIMEDOUT)
        return ETIMEDOUT;

    return 0;
}

int pthread_cond_signal(pthread_cond_t* cond) {
    if (!cond)
        return EINVAL;

    __sync_fetch_and_add(&cond->seq, 1);
    futex_wake(&cond->futex, 1);
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t* cond) {
    if (!cond)
        return EINVAL;

    __sync_fetch_and_add(&cond->seq, 1);
    futex_wake(&cond->futex, 0x7fffffff);
    return 0;
}
