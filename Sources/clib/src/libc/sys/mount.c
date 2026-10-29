#include <sys/mount.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

int mount(const char* special_file, const char* dir, const char* fstype,
          unsigned long int rwflag, const void* data) {
    return (int)SET_ERRNO(SYSCALL5(__NR_mount, (long)special_file, (long)dir,
        (long)fstype, (long)rwflag, (long)data));
}

int umount(const char* special_file) {
    return (int)SET_ERRNO(SYSCALL1(__NR_umount2, (long)special_file));
}

int umount2(const char* special_file, int flags) {
    return (int)SET_ERRNO(SYSCALL2(__NR_umount2, (long)special_file, flags));
}
