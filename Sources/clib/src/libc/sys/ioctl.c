#include <sys/ioctl.h>
#include <stdarg.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

int ioctl(int fd, unsigned long request, ...)
{
    va_list ap;
    va_start(ap, request);
    long arg = va_arg(ap, long);
    va_end(ap);
    return (int)SET_ERRNO(SYSCALL3(__NR_ioctl, fd, (long)request, arg));
}