#include <pthread.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

#define FUTEX_WAIT        0
#define FUTEX_WAKE        1
#define FUTEX_WAIT_BITSET 9

#define MUTEX_UNLOCKED 0
#define MUTEX_LOCKED   1
#define MUTEX_CONTESTED 2

static int futex_wait(int* addr, int val, const struct timespec* ts) {
    return (int)SYSCALL4(__NR_futex, (long)addr, FUTEX_WAIT, val, (long)ts);
}

static int futex_wake(int* addr, int count) {
    return (int)SYSCALL3(__NR_futex, (long)addr, FUTEX_WAKE, count);
}

static int atomic_cas(int* ptr, int expected, int desired) {
    return __sync_val_compare_and_swap(ptr, expected, desired);
}

int pthread_mutexattr_init(pthread_mutexattr_t* attr) {
    if (!attr)
        return EINVAL;

    attr->type = PTHREAD_MUTEX_DEFAULT;
    attr->shared = PTHREAD_PROCESS_PRIVATE;
    attr->robust = 0;
    attr->protocol = 0;
    attr->prioceiling = 0;
    return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t* attr) {
    if (!attr)
        return EINVAL;

    return 0;
}

int pthread_mutexattr_settype(pthread_mutexattr_t* attr, int type) {
    if (!attr)
        return EINVAL;

    if (type != PTHREAD_MUTEX_NORMAL && type != PTHREAD_MUTEX_ERRORCHECK && type != PTHREAD_MUTEX_RECURSIVE)
        return EINVAL;

    attr->type = type;
    return 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t* attr, int* type) {
    if (!attr || !type)
        return EINVAL;

    *type = attr->type;
    return 0;
}

int pthread_mutexattr_setpshared(pthread_mutexattr_t* attr, int pshared) {
    if (!attr)
        return EINVAL;

    attr->shared = pshared;
    return 0;
}

int pthread_mutexattr_getpshared(const pthread_mutexattr_t* attr, int* pshared) {
    if (!attr || !pshared)
        return EINVAL;

    *pshared = attr->shared;
    return 0;
}

int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr) {
    if (!mutex)
        return EINVAL;

    mutex->state = MUTEX_UNLOCKED;
    mutex->type = attr ? attr->type : PTHREAD_MUTEX_DEFAULT;
    mutex->shared = attr ? attr->shared : PTHREAD_PROCESS_PRIVATE;
    mutex->waiters = 0;
    mutex->owner = 0;
    mutex->futex = 0;
    return 0;
}

int pthread_mutex_destroy(pthread_mutex_t* mutex) {
    if (!mutex)
        return EINVAL;

    if (mutex->futex != 0)
        return EBUSY;

    return 0;
}

int pthread_mutex_lock(pthread_mutex_t* mutex) {
    if (!mutex)
        return EINVAL;

    int tid = (int)SYSCALL0(__NR_gettid);

    if (mutex->type == PTHREAD_MUTEX_RECURSIVE && (int)mutex->owner == tid) {
        mutex->waiters++;
        return 0;
    }

    int c = atomic_cas(&mutex->futex, MUTEX_UNLOCKED, MUTEX_LOCKED);
    if (c != MUTEX_UNLOCKED) {
        if (c != MUTEX_CONTESTED)
            c = __sync_lock_test_and_set(&mutex->futex, MUTEX_CONTESTED);

        while (c != MUTEX_UNLOCKED) {
            futex_wait(&mutex->futex, MUTEX_CONTESTED, NULL);
            c = __sync_lock_test_and_set(&mutex->futex, MUTEX_CONTESTED);
        }
    }

    mutex->owner = (unsigned int)tid;
    if (mutex->type == PTHREAD_MUTEX_RECURSIVE)
        mutex->waiters = 1;

    return 0;
}

int pthread_mutex_trylock(pthread_mutex_t* mutex) {
    if (!mutex)
        return EINVAL;

    int tid = (int)SYSCALL0(__NR_gettid);
    if (mutex->type == PTHREAD_MUTEX_RECURSIVE && (int)mutex->owner == tid) {
        mutex->waiters++;
        return 0;
    }

    if (atomic_cas(&mutex->futex, MUTEX_UNLOCKED, MUTEX_LOCKED) != MUTEX_UNLOCKED)
        return EBUSY;

    mutex->owner = (unsigned int)tid;
    if (mutex->type == PTHREAD_MUTEX_RECURSIVE)
        mutex->waiters = 1;

    return 0;
}

int pthread_mutex_timedlock(pthread_mutex_t* mutex, const struct timespec* abstime) {
    if (!mutex)
        return EINVAL;

    int c = atomic_cas(&mutex->futex, MUTEX_UNLOCKED, MUTEX_LOCKED);
    if (c != MUTEX_UNLOCKED) {
        if (c != MUTEX_CONTESTED)
            c = __sync_lock_test_and_set(&mutex->futex, MUTEX_CONTESTED);

        while (c != MUTEX_UNLOCKED) {
            int r = futex_wait(&mutex->futex, MUTEX_CONTESTED, abstime);
            if (r == -ETIMEDOUT) return ETIMEDOUT;
            c = __sync_lock_test_and_set(&mutex->futex, MUTEX_CONTESTED);
        }
    }
    mutex->owner = (unsigned int)SYSCALL0(__NR_gettid);
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t* mutex) {
    if (!mutex)
        return EINVAL;

    if (mutex->type == PTHREAD_MUTEX_RECURSIVE && mutex->waiters > 1) {
        mutex->waiters--;
        return 0;
    }

    mutex->owner = 0;
    if (mutex->waiters > 0)
        mutex->waiters = 0;

    if (__sync_fetch_and_sub(&mutex->futex, 1) != MUTEX_LOCKED) {
        mutex->futex = MUTEX_UNLOCKED;
        futex_wake(&mutex->futex, 1);
    }
    return 0;
}
