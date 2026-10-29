#ifndef SYS_WAIT_H
#define SYS_WAIT_H

#include <signal.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WNOHANG    1
#define WUNTRACED  2
#define WCONTINUED 8
#define WEXITED    4
#define WSTOPPED   2
#define WNOWAIT    0x01000000

#define WIFEXITED(s)    (((s) & 0x7f) == 0)
#define WEXITSTATUS(s)  (((s) >> 8) & 0xff)
#define WIFSIGNALED(s)  (((s) & 0x7f) != 0 && ((s) & 0x7f) != 0x7f)
#define WTERMSIG(s)     ((s) & 0x7f)
#define WIFSTOPPED(s)   (((s) & 0xff) == 0x7f)
#define WSTOPSIG(s)     (((s) >> 8) & 0xff)
#define WIFCONTINUED(s) ((s) == 0xffff)
#define WCOREDUMP(s)    ((s) & 0x80)

typedef enum {
    P_ALL = 0,
    P_PID = 1,
    P_PGID = 2
} idtype_t;

pid_t wait(int* status);
pid_t waitpid(pid_t pid, int* status, int options);
int waitid(idtype_t idtype, id_t id, siginfo_t* infop, int options);

#ifdef __cplusplus
}
#endif

#endif
