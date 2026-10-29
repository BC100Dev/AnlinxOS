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

int pthread_rwlockattr_init(pthread_rwlockattr_t* attr) {
    if (!attr)
        return EINVAL;

    attr->shared = PTHREAD_PROCESS_PRIVATE;
    attr->kind = 0;
    return 0;
}

int pthread_rwlockattr_destroy(pthread_rwlockattr_t* attr) {
    if (!attr)
        return EINVAL;

    return 0;
}

int pthread_rwlockattr_setpshared(pthread_rwlockattr_t* attr, int pshared) {
    if (!attr)
        return EINVAL;

    attr->shared = pshared;
    return 0;
}

int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t* attr, int* pshared) {
    if (!attr || !pshared)
        return EINVAL;

    *pshared = attr->shared;
    return 0;
}

int pthread_rwlock_init(pthread_rwlock_t* rwlock, const pthread_rwlockattr_t* attr) {
    if (!rwlock)
        return EINVAL;

    rwlock->shared = attr ? attr->shared : PTHREAD_PROCESS_PRIVATE;
    rwlock->seq = 0;
    rwlock->futex = 0;
    rwlock->waiters_r = 0;
    rwlock->waiters_w = 0;
    return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t* rwlock) {
    if (!rwlock)
        return EINVAL;

    if (rwlock->seq != 0 || rwlock->waiters_r > 0 || rwlock->waiters_w > 0)
        return EBUSY;

    return 0;
}

int pthread_rwlock_rdlock(pthread_rwlock_t* rwlock) {
    if (!rwlock)
        return EINVAL;

    while (1) {
        int seq = (int)rwlock->seq;
        if (seq >= 0) {
            if (__sync_bool_compare_and_swap(&rwlock->seq, (unsigned)seq, (unsigned)(seq + 1)))
                return 0;
        } else {
            __sync_fetch_and_add(&rwlock->waiters_r, 1);
            futex_wait(&rwlock->futex, seq, NULL);
            __sync_fetch_and_sub(&rwlock->waiters_r, 1);
        }
    }
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t* rwlock) {
    if (!rwlock)
        return EINVAL;

    int seq = (int)rwlock->seq;
    if (seq < 0)
        return EBUSY;

    if (!__sync_bool_compare_and_swap(&rwlock->seq, (unsigned)seq, (unsigned)(seq + 1)))
        return EBUSY;

    return 0;
}

int pthread_rwlock_timedrdlock(pthread_rwlock_t* rwlock, const struct timespec* abstime) {
    if (!rwlock)
        return EINVAL;

    while (1) {
        int seq = (int)rwlock->seq;
        if (seq >= 0) {
            if (__sync_bool_compare_and_swap(&rwlock->seq, (unsigned)seq, (unsigned)(seq + 1)))
                return 0;
        } else {
            __sync_fetch_and_add(&rwlock->waiters_r, 1);
            int r = futex_wait(&rwlock->futex, seq, abstime);
            __sync_fetch_and_sub(&rwlock->waiters_r, 1);
            if (r == -ETIMEDOUT)
                return ETIMEDOUT;
        }
    }
}

int pthread_rwlock_wrlock(pthread_rwlock_t* rwlock) {
    if (!rwlock)
        return EINVAL;

    while (1) {
        if (__sync_bool_compare_and_swap(&rwlock->seq, 0, (unsigned)-1))
            return 0;

        __sync_fetch_and_add(&rwlock->waiters_w, 1);
        futex_wait(&rwlock->futex, (int)rwlock->seq, NULL);
        __sync_fetch_and_sub(&rwlock->waiters_w, 1);
    }
}

int pthread_rwlock_trywrlock(pthread_rwlock_t* rwlock) {
    if (!rwlock)
        return EINVAL;

    if (!__sync_bool_compare_and_swap(&rwlock->seq, 0, (unsigned)-1))
        return EBUSY;

    return 0;
}

int pthread_rwlock_timedwrlock(pthread_rwlock_t* rwlock, const struct timespec* abstime) {
    if (!rwlock)
        return EINVAL;

    while (1) {
        if (__sync_bool_compare_and_swap(&rwlock->seq, 0, (unsigned)-1))
            return 0;

        __sync_fetch_and_add(&rwlock->waiters_w, 1);
        int r = futex_wait(&rwlock->futex, (int)rwlock->seq, abstime);
        __sync_fetch_and_sub(&rwlock->waiters_w, 1);
        if (r == -ETIMEDOUT)
            return ETIMEDOUT;
    }
}

int pthread_rwlock_unlock(pthread_rwlock_t* rwlock) {
    if (!rwlock)
        return EINVAL;

    int seq = (int)rwlock->seq;
    if (seq == -1)
        rwlock->seq = 0;
    else
        __sync_fetch_and_sub(&rwlock->seq, 1);

    if (rwlock->waiters_w > 0 || rwlock->waiters_r > 0)
        futex_wake(&rwlock->futex, rwlock->waiters_w > 0 ? 1 : (int)rwlock->waiters_r);

    return 0;
}
