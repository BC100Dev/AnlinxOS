#include <stddef.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#define DEFAULT_STACK_SIZE (2 * 1024 * 1024)
#define DEFAULT_GUARD_SIZE 4096

int pthread_attr_init(pthread_attr_t* attr) {
    if (!attr) return EINVAL;
    attr->detachstate = PTHREAD_CREATE_JOINABLE;
    attr->schedpolicy = 0;
    attr->inheritsched = PTHREAD_INHERIT_SCHED;
    attr->scope = PTHREAD_SCOPE_SYSTEM;
    attr->stacksize = DEFAULT_STACK_SIZE;
    attr->stackaddr = NULL;
    attr->guardsize = DEFAULT_GUARD_SIZE;
    return 0;
}

int pthread_attr_destroy(pthread_attr_t* attr) {
    if (!attr)
        return EINVAL;

    return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t* attr, int detachstate) {
    if (!attr)
        return EINVAL;

    if (detachstate != PTHREAD_CREATE_JOINABLE && detachstate != PTHREAD_CREATE_DETACHED)
        return EINVAL;

    attr->detachstate = detachstate;
    return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t* attr, int* detachstate) {
    if (!attr || !detachstate)
        return EINVAL;

    *detachstate = attr->detachstate;
    return 0;
}

int pthread_attr_setstacksize(pthread_attr_t* attr, size_t stacksize) {
    if (!attr || stacksize < 16384)
        return EINVAL;

    attr->stacksize = stacksize;
    return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t* attr, size_t* stacksize) {
    if (!attr || !stacksize)
        return EINVAL;

    *stacksize = attr->stacksize;
    return 0;
}

int pthread_attr_setstackaddr(pthread_attr_t* attr, void* stackaddr) {
    if (!attr)
        return EINVAL;

    attr->stackaddr = stackaddr;
    return 0;
}

int pthread_attr_getstackaddr(const pthread_attr_t* attr, void** stackaddr) {
    if (!attr || !stackaddr)
        return EINVAL;

    *stackaddr = attr->stackaddr;
    return 0;
}

int pthread_attr_setguardsize(pthread_attr_t* attr, size_t guardsize) {
    if (!attr)
        return EINVAL;

    attr->guardsize = guardsize;
    return 0;
}

int pthread_attr_getguardsize(const pthread_attr_t* attr, size_t* guardsize) {
    if (!attr || !guardsize)
        return EINVAL;

    *guardsize = attr->guardsize;
    return 0;
}

int pthread_attr_setscope(pthread_attr_t* attr, int scope) {
    if (!attr)
        return EINVAL;

    if (scope != PTHREAD_SCOPE_SYSTEM && scope != PTHREAD_SCOPE_PROCESS)
        return EINVAL;

    attr->scope = scope;
    return 0;
}

int pthread_attr_getscope(const pthread_attr_t* attr, int* scope) {
    if (!attr || !scope)
        return EINVAL;

    *scope = attr->scope;
    return 0;
}

int pthread_attr_setschedpolicy(pthread_attr_t* attr, int policy) {
    if (!attr)
        return EINVAL;

    attr->schedpolicy = policy;
    return 0;
}

int pthread_attr_getschedpolicy(const pthread_attr_t* attr, int* policy) {
    if (!attr || !policy)
        return EINVAL;

    *policy = attr->schedpolicy;
    return 0;
}

int pthread_attr_setinheritsched(pthread_attr_t* attr, int inheritsched) {
    if (!attr)
        return EINVAL;

    if (inheritsched != PTHREAD_INHERIT_SCHED && inheritsched != PTHREAD_EXPLICIT_SCHED)
        return EINVAL;

    attr->inheritsched = inheritsched;
    return 0;
}

int pthread_attr_getinheritsched(const pthread_attr_t* attr, int* inheritsched) {
    if (!attr || !inheritsched)
        return EINVAL;
    
    *inheritsched = attr->inheritsched;
    return 0;
}
