#include <sys/stat.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

int stat(const char *path, struct stat *buf) {
    return (int)SET_ERRNO(SYSCALL2(__NR_stat, (long)path, (long)buf));
}

int lstat(const char *path, struct stat *buf) {
    return (int)SET_ERRNO(SYSCALL2(__NR_lstat, (long)path, (long)buf));
}

int fstat(int fd, struct stat *buf) {
    return (int)SET_ERRNO(SYSCALL2(__NR_fstat, (long)fd, (long)buf));
}

int chmod(const char *path, mode_t mode) {
    return (int)SET_ERRNO(SYSCALL2(__NR_chmod, (long)path, (long)mode));
}

int fchmod(int fd, mode_t mode) {
    return (int)SET_ERRNO(SYSCALL2(__NR_fchmod, (long)fd, (long)mode));
}

int mkdir(const char *path, mode_t mode) {
    return (int)SET_ERRNO(SYSCALL2(__NR_mkdir, (long)path, (long)mode));
}

int mkdirat(int fd, const char *path, mode_t mode) {
    return (int)SET_ERRNO(SYSCALL3(__NR_mkdirat, (long)fd, (long)path, (long)mode));
}

int mknod(const char *path, mode_t mode, dev_t dev) {
    return (int)SET_ERRNO(SYSCALL3(__NR_mknod, (long)path, (long)mode, (long)dev));
}