#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

pid_t wait(int* status) {
    return waitpid(-1, status, 0);
}

pid_t waitpid(pid_t pid, int* status, int options) {
    return (pid_t)SET_ERRNO(SYSCALL4(__NR_wait4, (long)pid, (long)status, options, 0));
}

int waitid(idtype_t idtype, id_t id, siginfo_t* infop, int options) {
    return (int)SET_ERRNO(SYSCALL5(__NR_waitid, (long)idtype, (long)id, (long)infop, options, 0));
}
