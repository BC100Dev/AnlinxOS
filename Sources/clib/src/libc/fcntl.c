#include <fcntl.h>
#include <stdarg.h>
#include "internal/syscall.h"
#include "internal/syscall_nr.h"

int open(const char* path, int flags, ...) {
    mode_t mode = 0;
    if (flags & O_CREAT || flags & O_TMPFILE) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, mode_t);
        va_end(ap);
    }

    return (int)SYSCALL3(__NR_open, (long)path, flags, (long)mode);
}

int openat(int fd, const char* path, int flags, ...) {
    mode_t mode = 0;
    if (flags & O_CREAT || flags & O_TMPFILE) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, mode_t);
        va_end(ap);
    }

    return (int)SYSCALL4(__NR_openat, fd, (long)path, flags, (long)mode);
}

int creat(const char* path, mode_t mode) {
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

int fcntl(int fd, int cmd, ...) {
    va_list ap;
    va_start(ap, cmd);
    long arg = va_arg(ap, long);
    va_end(ap);

    return (int)SYSCALL3(__NR_fcntl, fd, cmd, arg);
}