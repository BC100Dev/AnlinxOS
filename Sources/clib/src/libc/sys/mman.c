#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    long ret = SYSCALL6(__NR_mmap, (long)addr, (long)length, prot, flags, fd, (long)offset);
    if (ret < 0 && ret > -4096) {
        errno = (int)-ret;
        return MAP_FAILED;
    }
    return (void*)ret;
}

int munmap(void* addr, size_t length) {
    return (int)SET_ERRNO(SYSCALL2(__NR_munmap, (long)addr, (long)length));
}

int mprotect(void* addr, size_t length, int prot) {
    return (int)SET_ERRNO(SYSCALL3(__NR_mprotect, (long)addr, (long)length, prot));
}

int msync(void* addr, size_t length, int flags) {
    return (int)SET_ERRNO(SYSCALL3(__NR_msync, (long)addr, (long)length, flags));
}

int madvise(void* addr, size_t length, int advice) {
    return (int)SET_ERRNO(SYSCALL3(__NR_madvise, (long)addr, (long)length, advice));
}

int mlock(const void* addr, size_t length) {
    return (int)SET_ERRNO(SYSCALL2(__NR_mlock, (long)addr, (long)length));
}

int munlock(const void* addr, size_t length) {
    return (int)SET_ERRNO(SYSCALL2(__NR_munlock, (long)addr, (long)length));
}

int mlockall(int flags) {
    return (int)SET_ERRNO(SYSCALL1(__NR_mlockall, flags));
}

int munlockall(void) {
    return (int)SET_ERRNO(SYSCALL0(__NR_munlockall));
}

void* mremap(void* old_addr, size_t old_size, size_t new_size, int flags, ...) {
    void* new_addr = NULL;
    if (flags & MREMAP_FIXED) {
        va_list ap;
        va_start(ap, flags);
        new_addr = va_arg(ap, void *);
        va_end(ap);
    }
    long ret = SYSCALL5(__NR_mremap, (long)old_addr, (long)old_size, (long)new_size, flags, (long)new_addr);
    if (ret < 0 && ret > -4096) {
        errno = (int)-ret;
        return MAP_FAILED;
    }

    return (void*)ret;
}

int posix_madvise(void* addr, size_t length, int advice) {
    return madvise(addr, length, advice);
}
