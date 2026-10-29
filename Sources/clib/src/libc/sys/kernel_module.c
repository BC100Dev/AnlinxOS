#include <sys/kernel_module.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

int init_module(void* module_image, unsigned long len, const char* param_values) {
    return (int)SET_ERRNO(SYSCALL3(__NR_init_module, (long)module_image,
        (long)len, (long)param_values));
}

int finit_module(int fd, const char* param_values, int flags) {
    return (int)SET_ERRNO(SYSCALL3(__NR_finit_module, (long)fd,
        (long)param_values, (long)flags));
}

int delete_module(const char* name, unsigned int flags) {
    return (int)SET_ERRNO(SYSCALL2(__NR_delete_module, (long)name, (long)flags));
}
