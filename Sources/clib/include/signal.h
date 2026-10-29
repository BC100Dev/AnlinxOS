#ifndef SIGNAL_H
#define SIGNAL_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int sig_atomic_t;

typedef struct {
    unsigned long __bits[128 / sizeof(unsigned long)];
} sigset_t;

#define SIGHUP    1
#define SIGINT    2
#define SIGQUIT   3
#define SIGILL    4
#define SIGTRAP   5
#define SIGABRT   6
#define SIGIOT    6
#define SIGBUS    7
#define SIGFPE    8
#define SIGKILL   9
#define SIGUSR1   10
#define SIGSEGV   11
#define SIGUSR2   12
#define SIGPIPE   13
#define SIGALRM   14
#define SIGTERM   15
#define SIGSTKFLT 16
#define SIGCHLD   17
#define SIGCONT   18
#define SIGSTOP   19
#define SIGTSTP   20
#define SIGTTIN   21
#define SIGTTOU   22
#define SIGURG    23
#define SIGXCPU   24
#define SIGXFSZ   25
#define SIGVTALRM 26
#define SIGPROF   27
#define SIGWINCH  28
#define SIGIO     29
#define SIGPOLL   29
#define SIGPWR    30
#define SIGSYS    31
#define SIGUNUSED 31
#define SIGRTMIN  32
#define SIGRTMAX  64
#define NSIG      65

#define SIG_DFL  ((void (*)(int))0)
#define SIG_IGN  ((void (*)(int))1)
#define SIG_ERR  ((void (*)(int))-1)

#define SA_NOCLDSTOP 0x00000001
#define SA_NOCLDWAIT 0x00000002
#define SA_SIGINFO   0x00000004
#define SA_ONSTACK   0x08000000
#define SA_RESTART   0x10000000
#define SA_NODEFER   0x40000000
#define SA_RESETHAND 0x80000000
#define SA_RESTORER  0x04000000

#define SIG_BLOCK   0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

#define SI_USER    0
#define SI_KERNEL  0x80
#define SI_QUEUE   -1
#define SI_TIMER   -2
#define SI_MESGQ   -3
#define SI_ASYNCIO -4
#define SI_SIGIO   -5
#define SI_TKILL   -6

typedef union sigval {
    int sival_int;
    void* sival_ptr;
} sigval_t;

typedef struct {
    int si_signo;
    int si_errno;
    int si_code;

    union {
        struct {
            pid_t si_pid;
            uid_t si_uid;
        } _kill;

        struct {
            int si_tid;
            int si_overrun;
            sigval_t si_sigval;
        } _timer;

        struct {
            pid_t si_pid;
            uid_t si_uid;
            sigval_t si_sigval;
        } _rt;

        struct {
            pid_t si_pid;
            uid_t si_uid;
            int si_status;
            long si_utime;
            long si_stime;
        } _sigchld;

        struct {
            void* si_addr;
        } _sigfault;

        struct {
            long si_band;
            int si_fd;
        } _sigpoll;
    } _sifields;
} siginfo_t;

#define si_pid    _sifields._kill.si_pid
#define si_uid    _sifields._kill.si_uid
#define si_status _sifields._sigchld.si_status
#define si_addr   _sifields._sigfault.si_addr
#define si_band   _sifields._sigpoll.si_band
#define si_fd     _sifields._sigpoll.si_fd
#define si_sigval _sifields._rt.si_sigval

struct sigaction {
    union {
        void (*sa_handler)(int);
        void (*sa_sigaction)(int, siginfo_t*, void*);
    } __sa_handler;

    sigset_t sa_mask;
    unsigned long sa_flags;
    void (*sa_restorer)(void);
};

#define sa_handler   __sa_handler.sa_handler
#define sa_sigaction __sa_handler.sa_sigaction

typedef struct {
    void* ss_sp;
    int ss_flags;
    size_t ss_size;
} stack_t;

#define SS_ONSTACK 1
#define SS_DISABLE 2
#define MINSIGSTKSZ 2048
#define SIGSTKSZ    8192

void (* signal(int signum, void (*handler)(int)))(int);
int sigaction(int signum, const struct sigaction* act, struct sigaction* oldact);

int kill(pid_t pid, int sig);
int killpg(pid_t pgrp, int sig);
int raise(int sig);
int tgkill(pid_t tgid, pid_t tid, int sig);

int sigemptyset(sigset_t* set);
int sigfillset(sigset_t* set);
int sigaddset(sigset_t* set, int signum);
int sigdelset(sigset_t* set, int signum);
int sigismember(const sigset_t* set, int signum);

int sigprocmask(int how, const sigset_t* set, sigset_t* oldset);
int sigpending(sigset_t* set);
int sigsuspend(const sigset_t* mask);
int sigwait(const sigset_t* set, int* sig);
int sigwaitinfo(const sigset_t* set, siginfo_t* info);
int sigtimedwait(const sigset_t* set, siginfo_t* info, const struct timespec* timeout);

int sigaltstack(const stack_t* ss, stack_t* old_ss);

#ifdef __cplusplus
}
#endif

#endif
