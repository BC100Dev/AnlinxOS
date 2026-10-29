#include <sys/resource.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

int getpriority(int which, id_t who) {
    int ret = (int)SYSCALL2(__NR_getpriority, which, (long)who);
    if (ret < 0 && -ret < 256) {
        errno = -ret;
        return -1;
    }
    return 20 - ret;
}

int setpriority(int which, id_t who, int prio) {
    return (int)SET_ERRNO(SYSCALL3(__NR_setpriority, which, (long)who, prio));
}

int getrlimit(int resource, struct rlimit* rlim) {
    return (int)SET_ERRNO(SYSCALL2(__NR_getrlimit, resource, (long)rlim));
}

int setrlimit(int resource, const struct rlimit* rlim) {
    return (int)SET_ERRNO(SYSCALL2(__NR_setrlimit, resource, (long)rlim));
}

int getrusage(int who, struct rusage* usage) {
    return (int)SET_ERRNO(SYSCALL2(__NR_getrusage, who, (long)usage));
}
