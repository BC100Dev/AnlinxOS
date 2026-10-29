#ifndef PTHREAD_H
#define PTHREAD_H

#include <stdint.h>
#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long pthread_t;
typedef unsigned int pthread_key_t;
typedef int pthread_once_t;

typedef struct {
    int detachstate;
    int schedpolicy;
    int inheritsched;
    int scope;
    size_t stacksize;
    void* stackaddr;
    int guardsize;
} pthread_attr_t;

typedef struct {
    int state;
    int type;
    int shared;
    unsigned int waiters;
    unsigned int owner;
    int futex;
} pthread_mutex_t;

typedef struct {
    int type;
    int shared;
    int robust;
    int protocol;
    int prioceiling;
} pthread_mutexattr_t;

typedef struct {
    int shared;
    unsigned int seq;
    int futex;
    unsigned int waiters_r;
    unsigned int waiters_w;
} pthread_rwlock_t;

typedef struct {
    int shared;
    int kind;
} pthread_rwlockattr_t;

typedef struct {
    int shared;
    unsigned int seq;
    int futex;
} pthread_cond_t;

typedef struct {
    int shared;
    int clock;
} pthread_condattr_t;

typedef struct {
    unsigned int count;
    unsigned int waiting;
    int shared;
    int futex;
} pthread_barrier_t;

typedef struct {
    int shared;
} pthread_barrierattr_t;

typedef struct {
    int futex;
} pthread_spinlock_t;

#define PTHREAD_CREATE_JOINABLE     0
#define PTHREAD_CREATE_DETACHED     1

#define PTHREAD_MUTEX_DEFAULT       0
#define PTHREAD_MUTEX_NORMAL        0
#define PTHREAD_MUTEX_ERRORCHECK    1
#define PTHREAD_MUTEX_RECURSIVE     2

#define PTHREAD_PROCESS_PRIVATE     0
#define PTHREAD_PROCESS_SHARED      1

#define PTHREAD_SCOPE_SYSTEM        0
#define PTHREAD_SCOPE_PROCESS       1

#define PTHREAD_INHERIT_SCHED       0
#define PTHREAD_EXPLICIT_SCHED      1

#define PTHREAD_CANCEL_ENABLE       0
#define PTHREAD_CANCEL_DISABLE      1
#define PTHREAD_CANCEL_DEFERRED     0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1
#define PTHREAD_CANCELED            ((void*)-1)

#define PTHREAD_ONCE_INIT           0

#define PTHREAD_MUTEX_INITIALIZER   {0, 0, 0, 0, 0, 0}
#define PTHREAD_RWLOCK_INITIALIZER  {0, 0, 0, 0, 0}
#define PTHREAD_COND_INITIALIZER    {0, 0, 0}

int pthread_create(pthread_t* thread, const pthread_attr_t* attr,
                   void*(*start_routine)(void*), void* arg);
int pthread_join(pthread_t thread, void** retval);
int pthread_detach(pthread_t thread);
pthread_t pthread_self(void);
int pthread_equal(pthread_t t1, pthread_t t2);
void pthread_exit(void* retval);

int pthread_cancel(pthread_t thread);
int pthread_setcancelstate(int state, int* oldstate);
int pthread_setcanceltype(int type, int* oldtype);
void pthread_testcancel(void);

int pthread_attr_init(pthread_attr_t* attr);
int pthread_attr_destroy(pthread_attr_t* attr);
int pthread_attr_setdetachstate(pthread_attr_t* attr, int detachstate);
int pthread_attr_getdetachstate(const pthread_attr_t* attr, int* detachstate);
int pthread_attr_setstacksize(pthread_attr_t* attr, size_t stacksize);
int pthread_attr_getstacksize(const pthread_attr_t* attr, size_t* stacksize);
int pthread_attr_setstackaddr(pthread_attr_t* attr, void* stackaddr);
int pthread_attr_getstackaddr(const pthread_attr_t* attr, void** stackaddr);
int pthread_attr_setguardsize(pthread_attr_t* attr, size_t guardsize);
int pthread_attr_getguardsize(const pthread_attr_t* attr, size_t* guardsize);
int pthread_attr_setscope(pthread_attr_t* attr, int scope);
int pthread_attr_getscope(const pthread_attr_t* attr, int* scope);
int pthread_attr_setschedpolicy(pthread_attr_t* attr, int policy);
int pthread_attr_getschedpolicy(const pthread_attr_t* attr, int* policy);
int pthread_attr_setinheritsched(pthread_attr_t* attr, int inheritsched);
int pthread_attr_getinheritsched(const pthread_attr_t* attr, int* inheritsched);

int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* attr);
int pthread_mutex_destroy(pthread_mutex_t* mutex);
int pthread_mutex_lock(pthread_mutex_t* mutex);
int pthread_mutex_trylock(pthread_mutex_t* mutex);
int pthread_mutex_timedlock(pthread_mutex_t* mutex, const struct timespec* abstime);
int pthread_mutex_unlock(pthread_mutex_t* mutex);

int pthread_mutexattr_init(pthread_mutexattr_t* attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t* attr);
int pthread_mutexattr_settype(pthread_mutexattr_t* attr, int type);
int pthread_mutexattr_gettype(const pthread_mutexattr_t* attr, int* type);
int pthread_mutexattr_setpshared(pthread_mutexattr_t* attr, int pshared);
int pthread_mutexattr_getpshared(const pthread_mutexattr_t* attr, int* pshared);

int pthread_rwlock_init(pthread_rwlock_t* rwlock, const pthread_rwlockattr_t* attr);
int pthread_rwlock_destroy(pthread_rwlock_t* rwlock);
int pthread_rwlock_rdlock(pthread_rwlock_t* rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t* rwlock);
int pthread_rwlock_timedrdlock(pthread_rwlock_t* rwlock, const struct timespec* abstime);
int pthread_rwlock_wrlock(pthread_rwlock_t* rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t* rwlock);
int pthread_rwlock_timedwrlock(pthread_rwlock_t* rwlock, const struct timespec* abstime);
int pthread_rwlock_unlock(pthread_rwlock_t* rwlock);

int pthread_rwlockattr_init(pthread_rwlockattr_t* attr);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t* attr);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t* attr, int pshared);
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t* attr, int* pshared);

int pthread_cond_init(pthread_cond_t* cond, const pthread_condattr_t* attr);
int pthread_cond_destroy(pthread_cond_t* cond);
int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex);
int pthread_cond_timedwait(pthread_cond_t* cond, pthread_mutex_t* mutex,
                           const struct timespec* abstime);
int pthread_cond_signal(pthread_cond_t* cond);
int pthread_cond_broadcast(pthread_cond_t* cond);

int pthread_condattr_init(pthread_condattr_t* attr);
int pthread_condattr_destroy(pthread_condattr_t* attr);
int pthread_condattr_setpshared(pthread_condattr_t* attr, int pshared);
int pthread_condattr_getpshared(const pthread_condattr_t* attr, int* pshared);
int pthread_condattr_setclock(pthread_condattr_t* attr, int clock_id);
int pthread_condattr_getclock(const pthread_condattr_t* attr, int* clock_id);

int pthread_barrier_init(pthread_barrier_t* barrier, const pthread_barrierattr_t* attr,
                         unsigned int count);
int pthread_barrier_destroy(pthread_barrier_t* barrier);
int pthread_barrier_wait(pthread_barrier_t* barrier);

int pthread_barrierattr_init(pthread_barrierattr_t* attr);
int pthread_barrierattr_destroy(pthread_barrierattr_t* attr);
int pthread_barrierattr_setpshared(pthread_barrierattr_t* attr, int pshared);
int pthread_barrierattr_getpshared(const pthread_barrierattr_t* attr, int* pshared);

int pthread_spin_init(pthread_spinlock_t* lock, int pshared);
int pthread_spin_destroy(pthread_spinlock_t* lock);
int pthread_spin_lock(pthread_spinlock_t* lock);
int pthread_spin_trylock(pthread_spinlock_t* lock);
int pthread_spin_unlock(pthread_spinlock_t* lock);

int pthread_key_create(pthread_key_t* key, void (*destructor)(void*));
int pthread_key_delete(pthread_key_t key);
void* pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void* value);

int pthread_once(pthread_once_t* once_control, void (*init_routine)(void));

int pthread_setname_np(pthread_t thread, const char* name);
int pthread_getname_np(pthread_t thread, char* name, size_t len);

int pthread_getconcurrency(void);
int pthread_setconcurrency(int level);

#ifdef __cplusplus
}
#endif

#endif
