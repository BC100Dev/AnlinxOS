#include <signal.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include "internal/syscall.h"
#include "internal/syscall_nr.h"

void (* signal(int signum, void (*handler)(int)))(int) {
    struct sigaction act, oldact;
    act.sa_handler = handler;
    act.sa_flags = SA_RESTART;
    act.sa_restorer = NULL;
    sigemptyset(&act.sa_mask);
    if (sigaction(signum, &act, &oldact) < 0)
        return SIG_ERR;

    return oldact.sa_handler;
}

int sigaction(int signum, const struct sigaction* act, struct sigaction* oldact) {
    return (int)SET_ERRNO(SYSCALL4(__NR_rt_sigaction, signum,
        (long)act, (long)oldact,
        sizeof(sigset_t)));
}

int kill(pid_t pid, int sig) {
    return (int)SET_ERRNO(SYSCALL2(__NR_kill, (long)pid, sig));
}

int killpg(pid_t pgrp, int sig) {
    return kill(-pgrp, sig);
}

int raise(int sig) {
    return (int)SET_ERRNO(SYSCALL2(__NR_tkill,
        SYSCALL0(__NR_gettid), sig));
}

int tgkill(pid_t tgid, pid_t tid, int sig) {
    return (int)SET_ERRNO(SYSCALL3(__NR_tgkill, (long)tgid, (long)tid, sig));
}

int sigemptyset(sigset_t* set) {
    if (!set)
        return -1;

    memset(set, 0, sizeof(sigset_t));
    return 0;
}

int sigfillset(sigset_t* set) {
    if (!set)
        return -1;

    memset(set, 0xff, sizeof(sigset_t));
    return 0;
}

int sigaddset(sigset_t* set, int signum) {
    if (!set || signum < 1 || signum >= NSIG) {
        errno = EINVAL;
        return -1;
    }
    signum--;
    set->__bits[signum / (8 * sizeof(unsigned long))] |=
        1UL << (signum % (8 * sizeof(unsigned long)));
    return 0;
}

int sigdelset(sigset_t* set, int signum) {
    if (!set || signum < 1 || signum >= NSIG) {
        errno = EINVAL;
        return -1;
    }
    signum--;
    set->__bits[signum / (8 * sizeof(unsigned long))] &=
        ~(1UL << (signum % (8 * sizeof(unsigned long))));
    return 0;
}

int sigismember(const sigset_t* set, int signum) {
    if (!set || signum < 1 || signum >= NSIG) {
        errno = EINVAL;
        return -1;
    }
    signum--;
    return (set->__bits[signum / (8 * sizeof(unsigned long))] >>
        (signum % (8 * sizeof(unsigned long)))) & 1;
}

int sigprocmask(int how, const sigset_t* set, sigset_t* oldset) {
    return (int)SET_ERRNO(SYSCALL4(__NR_rt_sigprocmask, how,
        (long)set, (long)oldset,
        sizeof(sigset_t)));
}

int sigpending(sigset_t* set) {
    return (int)SET_ERRNO(SYSCALL2(__NR_rt_sigpending,
        (long)set, sizeof(sigset_t)));
}

int sigsuspend(const sigset_t* mask) {
    return (int)SET_ERRNO(SYSCALL2(__NR_rt_sigsuspend,
        (long)mask, sizeof(sigset_t)));
}

int sigwait(const sigset_t* set, int* sig) {
    siginfo_t info;
    int r = sigwaitinfo(set, &info);
    if (r < 0)
        return errno;

    *sig = info.si_signo;
    return 0;
}

int sigwaitinfo(const sigset_t* set, siginfo_t* info) {
    return (int)SET_ERRNO(SYSCALL4(__NR_rt_sigtimedwait,
        (long)set, (long)info, 0,
        sizeof(sigset_t)));
}

int sigtimedwait(const sigset_t* set, siginfo_t* info, const struct timespec* timeout) {
    return (int)SET_ERRNO(SYSCALL4(__NR_rt_sigtimedwait,
        (long)set, (long)info, (long)timeout,
        sizeof(sigset_t)));
}

int sigaltstack(const stack_t* ss, stack_t* old_ss) {
    return (int)SET_ERRNO(SYSCALL2(__NR_sigaltstack,
        (long)ss, (long)old_ss));
}
