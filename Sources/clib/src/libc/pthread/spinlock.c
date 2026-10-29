#include <pthread.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
    if (!lock)
        return EINVAL;

    (void)pshared;
    lock->futex = 0;
    return 0;
}

int pthread_spin_destroy(pthread_spinlock_t *lock) {
    if (!lock)
        return EINVAL;

    return 0;
}

int pthread_spin_lock(pthread_spinlock_t *lock) {
    if (!lock)
        return EINVAL;

    while (__sync_lock_test_and_set(&lock->futex, 1))
        while (lock->futex) __asm__ volatile("pause" ::: "memory");

    return 0;
}

int pthread_spin_trylock(pthread_spinlock_t *lock) {
    if (!lock)
        return EINVAL;

    if (__sync_lock_test_and_set(&lock->futex, 1))
        return EBUSY;

    return 0;
}

int pthread_spin_unlock(pthread_spinlock_t *lock) {
    if (!lock)
        return EINVAL;

    __sync_lock_release(&lock->futex);
    return 0;
}