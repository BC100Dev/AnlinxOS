#include <pthread.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

#define CLONE_VM        0x00000100
#define CLONE_FS        0x00000200
#define CLONE_FILES     0x00000400
#define CLONE_SIGHAND   0x00000800
#define CLONE_THREAD    0x00010000
#define CLONE_SYSVSEM   0x00040000
#define CLONE_SETTLS    0x00080000
#define CLONE_PARENT_SETTID  0x00100000
#define CLONE_CHILD_CLEARTID 0x00200000
#define CLONE_DETACHED  0x00400000

#define DEFAULT_STACK_SIZE (2 * 1024 * 1024)
#define DEFAULT_GUARD_SIZE (4096)

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1

typedef struct {
    void*(*start)(void*);
    void* arg;
    pthread_attr_t attr;
    int* parent_tid;
    int* child_tid;
} thread_start_args_t;

static int futex_wait(int* addr, int val) {
    return (int)SYSCALL4(__NR_futex, (long)addr, FUTEX_WAIT, val, 0);
}

static int futex_wake(int* addr, int count) {
    return (int)SYSCALL3(__NR_futex, (long)addr, FUTEX_WAKE, count);
}

static void thread_entry(thread_start_args_t* args) {
    void*(*start)(void*) = args->start;
    void* arg = args->arg;
    free(args);
    void* retval = start(arg);
    pthread_exit(retval);
}

int pthread_create(pthread_t* thread, const pthread_attr_t* attr,
                   void*(*start_routine)(void*), void* arg) {
    if (!thread || !start_routine)
        return EINVAL;

    size_t stacksize = attr ? (size_t)attr->stacksize : DEFAULT_STACK_SIZE;
    size_t guardsize = attr ? (size_t)attr->guardsize : DEFAULT_GUARD_SIZE;
    int detachstate = attr ? attr->detachstate : PTHREAD_CREATE_JOINABLE;

    if (stacksize == 0)
        stacksize = DEFAULT_STACK_SIZE;

    uint8_t* stack = malloc(stacksize + guardsize);
    if (!stack)
        return ENOMEM;

    thread_start_args_t* targs = malloc(sizeof(thread_start_args_t));
    if (!targs) {
        free(stack);
        return ENOMEM;
    }

    targs->start = start_routine;
    targs->arg = arg;

    int* child_tid = (int*)(stack + guardsize + stacksize - sizeof(int));
    targs->child_tid = child_tid;

    uint8_t* stack_top = stack + guardsize + stacksize - sizeof(void*) * 4;

    unsigned long flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
        CLONE_THREAD | CLONE_SYSVSEM | CLONE_SETTLS |
        CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID;

    if (detachstate == PTHREAD_CREATE_DETACHED)
        flags |= CLONE_DETACHED;

    long tid = SYSCALL5(__NR_clone, (long)flags, (long)stack_top,
                        (long)child_tid, (long)child_tid, (long)targs);

    if (tid < 0) {
        free(targs);
        free(stack);
        errno = (int)-tid;
        return errno;
    }

    *thread = (pthread_t)tid;
    return 0;
}

int pthread_join(pthread_t thread, void** retval) {
    int* tid = (int*)(uintptr_t)thread;
    while (*tid != 0)
        futex_wait(tid, *tid);

    if (retval)
        *retval = NULL;

    return 0;
}

int pthread_detach(pthread_t thread) {
    (void)thread;
    return 0;
}

pthread_t pthread_self(void) {
    return (pthread_t)SYSCALL0(__NR_gettid);
}

int pthread_equal(pthread_t t1, pthread_t t2) {
    return t1 == t2;
}

void pthread_exit(void* retval) {
    (void)retval;
    SYSCALL1(__NR_exit, 0);
    __builtin_unreachable();
}

int pthread_cancel(pthread_t thread) {
    return (int)SET_ERRNO(SYSCALL2(__NR_tkill, (long)thread, 32));
}

int pthread_setcancelstate(int state, int* oldstate) {
    (void)state;
    if (oldstate)
        *oldstate = PTHREAD_CANCEL_ENABLE;

    return 0;
}

int pthread_setcanceltype(int type, int* oldtype) {
    (void)type;
    if (oldtype)
        *oldtype = PTHREAD_CANCEL_DEFERRED;

    return 0;
}

void pthread_testcancel(void) {
}
