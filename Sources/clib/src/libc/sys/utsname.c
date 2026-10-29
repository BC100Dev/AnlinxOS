#include <sys/utsname.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

int uname(struct utsname *buf) {
    return (int)SET_ERRNO(SYSCALL1(__NR_uname, (long)buf));
}