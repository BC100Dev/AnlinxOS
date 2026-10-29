#include <sys/prctl.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

int prctl(int option, unsigned long arg2, unsigned long arg3,
          unsigned long arg4, unsigned long arg5) {
    return (int)SET_ERRNO(SYSCALL5(__NR_prctl, option, (long)arg2,
                                   (long)arg3, (long)arg4, (long)arg5));
}