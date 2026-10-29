#include <pthread.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1

#define ONCE_INIT      0
#define ONCE_RUNNING   1
#define ONCE_DONE      2

static int futex_wait(int* addr, int val) {
    return (int)SYSCALL4(__NR_futex, (long)addr, FUTEX_WAIT, val, 0);
}

static int futex_wake(int* addr, int count) {
    return (int)SYSCALL3(__NR_futex, (long)addr, FUTEX_WAKE, count);
}

int pthread_once(pthread_once_t* once_control, void (*init_routine)(void)) {
    if (!once_control || !init_routine)
        return EINVAL;

    if (*once_control == ONCE_DONE)
        return 0;

    if (__sync_bool_compare_and_swap(once_control, ONCE_INIT, ONCE_RUNNING)) {
        init_routine();
        *once_control = ONCE_DONE;
        futex_wake(once_control, 0x7fffffff);
        return 0;
    }

    while (*once_control != ONCE_DONE)
        futex_wait(once_control, ONCE_RUNNING);

    return 0;
}
