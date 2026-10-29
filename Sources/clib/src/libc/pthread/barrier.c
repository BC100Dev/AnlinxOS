#include <stddef.h>
#include <pthread.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1
#define PTHREAD_BARRIER_SERIAL_THREAD (-1)

static int futex_wait(int* addr, int val) {
    return (int)SYSCALL4(__NR_futex, (long)addr, FUTEX_WAIT, val, 0);
}

static int futex_wake(int* addr, int count) {
    return (int)SYSCALL3(__NR_futex, (long)addr, FUTEX_WAKE, count);
}

int pthread_barrierattr_init(pthread_barrierattr_t* attr) {
    if (!attr)
        return EINVAL;

    attr->shared = PTHREAD_PROCESS_PRIVATE;
    return 0;
}

int pthread_barrierattr_destroy(pthread_barrierattr_t* attr) {
    if (!attr)
        return EINVAL;

    return 0;
}

int pthread_barrierattr_setpshared(pthread_barrierattr_t* attr, int pshared) {
    if (!attr)
        return EINVAL;

    attr->shared = pshared;
    return 0;
}

int pthread_barrierattr_getpshared(const pthread_barrierattr_t* attr, int* pshared) {
    if (!attr || !pshared)
        return EINVAL;

    *pshared = attr->shared;
    return 0;
}

int pthread_barrier_init(pthread_barrier_t* barrier, const pthread_barrierattr_t* attr, unsigned int count) {
    if (!barrier || count == 0)
        return EINVAL;

    barrier->count = count;
    barrier->waiting = 0;
    barrier->shared = attr ? attr->shared : PTHREAD_PROCESS_PRIVATE;
    barrier->futex = 0;
    return 0;
}

int pthread_barrier_destroy(pthread_barrier_t* barrier) {
    if (!barrier)
        return EINVAL;

    if (barrier->waiting > 0)
        return EBUSY;

    return 0;
}

int pthread_barrier_wait(pthread_barrier_t* barrier) {
    if (!barrier)
        return EINVAL;

    unsigned int seq = barrier->futex;
    if (__sync_add_and_fetch(&barrier->waiting, 1) == barrier->count) {
        barrier->waiting = 0;
        __sync_fetch_and_add(&barrier->futex, 1);

        futex_wake(&barrier->futex, 0x7fffffff);
        return PTHREAD_BARRIER_SERIAL_THREAD;
    }

    while (barrier->futex == seq)
        futex_wait(&barrier->futex, (int)seq);

    return 0;
}
