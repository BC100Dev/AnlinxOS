#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <bits/confname.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

#include <mdlc/archit.h>

#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <sys/resource.h>

#include "internal/syscall.h"
#include "internal/syscall_nr.h"

#define UNISTD_STUB_MSG write(STDERR_FILENO, "unistd.c: call not implemented", 30);

char** environ = NULL;
char* optarg = NULL;
int opterr = 1, optind = 1, optopt = 0;

int access(const char* name, int type) {
    return (int)SET_ERRNO(SYSCALL2(__NR_access, (long)name, type));
}

unsigned int alarm(unsigned int seconds) {
    return (unsigned int)SYSCALL1(__NR_alarm, seconds);
}

int chdir(const char* path) {
    return (int)SET_ERRNO(SYSCALL1(__NR_chdir, (long)path));
}

int chown(const char* file, uid_t owner, gid_t group) {
    return (int)SET_ERRNO(SYSCALL3(__NR_chown, (long)file, owner, group));
}

int close(int fd) {
    return (int)SET_ERRNO(SYSCALL1(__NR_close, fd));
}

size_t confstr(int name, char* buf, size_t len) {
    const char* val = NULL;

    switch (name) {
    case _CS_PATH: {
        const char* env = getenv("PATH");
        val = env ? env : "/System/bin:" SYSTEM_FS_PATH ":/vendor/bin:/vendor/" SYSTEM_FS_NAME;
        break;
    }
    default:
        return 0;
    }

    size_t needed = 0;
    const char* p = val;
    while (*p++) needed++;
    needed++;

    if (buf && len > 0) {
        size_t i = 0;
        while (i < len - 1 && val[i]) {
            buf[i] = val[i];
            i++;
        }
        buf[i] = '\0';
    }
    return needed;
}

int dup(int fd) {
    return (int)SET_ERRNO(SYSCALL1(__NR_dup, fd));
}

int dup2(int fd, int fd2) {
    return (int)SET_ERRNO(SYSCALL2(__NR_dup2, fd, fd2));
}

int dup3(int fd, int fd2, int flags) {
    return (int)SET_ERRNO(SYSCALL3(__NR_dup3, fd, fd2, flags));
}

void _exit(int status) {
    SYSCALL1(__NR_exit_group, status);
    __builtin_unreachable();
}

int execve(const char* path, char* const argv[], char* const envp[]) {
    return (int)SET_ERRNO(SYSCALL3(__NR_execve, (long)path, (long)argv, (long)envp));
}

int execv(const char* path, char* const argv[]) {
    return execve(path, argv, environ);
}

int execvp(const char* file, char* const argv[]) {
    return execve(file, argv, environ);
}

int execvpe(const char* file, char* const argv[], char* const envp[]) {
    return execve(file, argv, envp);
}

int execl(const char* path, const char* arg, ...) {
    va_list ap;
    int argc = 1;
    va_start(ap, arg);
    while (va_arg(ap, const char*))
        argc++;

    va_end(ap);

    char** argv = malloc((argc + 1) * sizeof(char*));
    if (!argv) {
        errno = ENOMEM;
        return -1;
    }

    argv[0] = (char*)arg;
    va_start(ap, arg);
    for (int i = 1; i <= argc; i++)
        argv[i] = va_arg(ap, char*);

    va_end(ap);

    int ret = execve(path, argv, environ);
    free(argv);
    return ret;
}

int execle(const char* path, const char* arg, ...) {
    va_list ap;
    int argc = 1;
    va_start(ap, arg);
    while (va_arg(ap, const char*))
        argc++;

    va_end(ap);

    char** argv = malloc((argc + 1) * sizeof(char*));
    if (!argv) {
        errno = ENOMEM;
        return -1;
    }

    argv[0] = (char*)arg;
    va_start(ap, arg);
    for (int i = 1; i <= argc; i++)
        argv[i] = va_arg(ap, char*);

    char** envp = va_arg(ap, char**);
    va_end(ap);

    int ret = execve(path, argv, envp);
    free(argv);
    return ret;
}

int execlp(const char* file, const char* arg, ...) {
    va_list ap;
    int argc = 1;
    va_start(ap, arg);
    while (va_arg(ap, const char*))
        argc++;

    va_end(ap);

    char** argv = malloc((argc + 1) * sizeof(char*));
    if (!argv) {
        errno = ENOMEM;
        return -1;
    }

    argv[0] = (char*)arg;
    va_start(ap, arg);
    for (int i = 1; i <= argc; i++)
        argv[i] = va_arg(ap, char*);

    va_end(ap);

    int ret = execvp(file, argv);
    free(argv);
    return ret;
}

int faccessat(int fd, const char* file, int type, int flag) {
    return (int)SET_ERRNO(SYSCALL4(__NR_faccessat, fd, (long)file, type, flag));
}

int fchdir(int fd) {
    return (int)SET_ERRNO(SYSCALL1(__NR_fchdir, fd));
}

int fchown(int fd, uid_t owner, gid_t group) {
    return (int)SET_ERRNO(SYSCALL3(__NR_fchown, fd, owner, group));
}

int fchownat(int fd, const char* file, uid_t owner, gid_t group, int flag) {
    return (int)SET_ERRNO(SYSCALL5(__NR_fchownat, fd, (long)file, owner, group, flag));
}

int fdatasync(int fd) {
    return (int)SET_ERRNO(SYSCALL1(__NR_fdatasync, fd));
}

int fexecve(int fd, char* const argv[], char* const envp[]) {
    return (int)SET_ERRNO(SYSCALL5(__NR_execveat, fd, (long)"", (long)argv, (long)envp, 0x1000));
}

pid_t fork(void) {
    return (pid_t)SET_ERRNO(SYSCALL0(__NR_fork));
}

int fsync(int fd) {
    return (int)SET_ERRNO(SYSCALL1(__NR_fsync, fd));
}

int ftruncate(int fd, off_t length) {
    return (int)SET_ERRNO(SYSCALL2(__NR_ftruncate, fd, (long)length));
}

char* getcwd(char* buf, size_t size) {
    sysret_t ret = SYSCALL2(__NR_getcwd, (long)buf, (long)size);
    if (ret < 0) {
        errno = (int)-ret;
        return NULL;
    }
    return buf;
}

gid_t getegid(void) {
    return (gid_t)SYSCALL0(__NR_getegid);
}

uid_t geteuid(void) {
    return (uid_t)SYSCALL0(__NR_geteuid);
}

gid_t getgid(void) {
    return (gid_t)SYSCALL0(__NR_getgid);
}

int getgroups(int size, gid_t list[]) {
    return (int)SET_ERRNO(SYSCALL2(__NR_getgroups, size, (long)list));
}

long gethostid(void) {
    UNISTD_STUB_MSG
    return 0;
}

int gethostname(char* name, size_t len) {
    struct utsname u;
    if (uname(&u) != 0)
        return -1;

    size_t nlen = 0;
    while (u.nodename[nlen] && nlen < sizeof(u.nodename))
        nlen++;

    if (nlen >= len) {
        errno = ENAMETOOLONG;
        return -1;
    }

    memcpy(name, u.nodename, nlen + 1);
    return 0;
}

char* getlogin(void) {
    UNISTD_STUB_MSG
    return NULL;
}

int getlogin_r(char* name, size_t len) {
    UNISTD_STUB_MSG
    return -1;
}

int getopt(int argc, char* const argv[], const char* shortopts) {
    if (optind >= argc || !argv[optind] || argv[optind][0] != '-' || argv[optind][1] == '\0')
        return -1;

    if (argv[optind][1] == '-' && argv[optind][2] == '\0') {
        optind++;
        return -1;
    }

    static int pos = 1;
    int c = argv[optind][pos];
    if (c == '\0') {
        optind++;
        pos = 1;
        return getopt(argc, argv, shortopts);
    }

    const char* opt = shortopts;
    while (*opt && *opt != c) opt++;
    if (!*opt) {
        optopt = c;
        pos++;
        errno = EINVAL;
        return '?';
    }

    if (*(opt + 1) == ':') {
        if (argv[optind][pos + 1] != '\0') {
            optarg = &argv[optind][pos + 1];
        } else if (optind + 1 < argc) {
            optarg = argv[++optind];
        } else {
            optopt = c;
            optind++;
            pos = 1;
            return (shortopts[0] == ':') ? ':' : '?';
        }
        optind++;
        pos = 1;
    } else {
        optarg = NULL;
        pos++;
    }

    return c;
}

pid_t getpgid(pid_t pid) {
    return (pid_t)SET_ERRNO(SYSCALL1(__NR_getpgid, pid));
}

pid_t getpgrp(void) {
    return getpgid(0);
}

pid_t getpid(void) {
    return (pid_t)SYSCALL0(__NR_getpid);
}

pid_t getppid(void) {
    return (pid_t)SYSCALL0(__NR_getppid);
}

pid_t getsid(pid_t pid) {
    return (pid_t)SET_ERRNO(SYSCALL1(__NR_getsid, pid));
}

uid_t getuid(void) {
    return (uid_t)SYSCALL0(__NR_getuid);
}

int getresuid(uid_t* ruid, uid_t* euid, uid_t* suid) {
    return (int)SET_ERRNO(SYSCALL3(__NR_getresuid, (long)ruid, (long)euid, (long)suid));
}

int getresgid(gid_t* rgid, gid_t* egid, gid_t* sgid) {
    return (int)SET_ERRNO(SYSCALL3(__NR_getresgid, (long)rgid, (long)egid, (long)sgid));
}

int isatty(int fd) {
    struct termios t;
    return ioctl(fd, TCGETS, &t) == 0;
}

int lchown(const char* file, uid_t owner, gid_t group) {
    return (int)SET_ERRNO(SYSCALL3(__NR_lchown, (long)file, owner, group));
}

int link(const char* from, const char* to) {
    return (int)SET_ERRNO(SYSCALL2(__NR_link, (long)from, (long)to));
}

int linkat(int fromfd, const char* from, int tofd, const char* to, int flags) {
    return (int)SET_ERRNO(SYSCALL5(__NR_linkat, fromfd, (long)from, tofd, (long)to, flags));
}

int lockf(int fd, int cmd, off_t len) {
    struct flock fl;
    fl.l_whence = SEEK_CUR;
    fl.l_start = 0;
    fl.l_len = len;
    fl.l_pid = 0;

    switch (cmd) {
    case F_ULOCK:
        fl.l_type = F_UNLCK;
        return fcntl(fd, F_SETLK, &fl);
    case F_LOCK:
        fl.l_type = F_WRLCK;
        return fcntl(fd, F_SETLKW, &fl);
    case F_TLOCK:
        fl.l_type = F_WRLCK;
        return fcntl(fd, F_SETLK, &fl);
    case F_TEST:
        fl.l_type = F_RDLCK;
        if (fcntl(fd, F_GETLK, &fl) != 0) return -1;
        if (fl.l_type == F_UNLCK || fl.l_pid == getpid()) return 0;
        errno = EACCES;
        return -1;
    default:
        errno = EINVAL;
        return -1;
    }
}

off_t lseek(int fd, off_t offset, int whence) {
    sysret_t r = SYSCALL3(__NR_lseek, fd, (long)offset, whence);
    if (r < 0) {
        errno = (int)-r;
        return (off_t)-1;
    }
    return (off_t)r;
}

int nice(int inc) {
    int prio = getpriority(PRIO_PROCESS, 0);
    if (prio == -1 && errno != 0)
        return -1;

    return (int)SET_ERRNO(SYSCALL3(__NR_setpriority, PRIO_PROCESS, 0, prio + inc));
}

long pathconf(const char* path, int name) {
    UNISTD_STUB_MSG
    return -1;
}

long fpathconf(int fd, int name) {
    UNISTD_STUB_MSG
    return -1;
}

int pause(void) {
    return (int)SET_ERRNO(SYSCALL0(__NR_pause));
}

int pipe(int pipedes[2]) {
    return (int)SET_ERRNO(SYSCALL1(__NR_pipe, (long)pipedes));
}

int pipe2(int pipedes[2], int flags) {
    return (int)SET_ERRNO(SYSCALL2(__NR_pipe2, (long)pipedes, flags));
}

ssize_t pread(int fd, void* buf, size_t nbytes, off_t offset) {
    return (ssize_t)SET_ERRNO(SYSCALL4(__NR_pread64, fd, (long)buf, (long)nbytes, (long)offset));
}

ssize_t pwrite(int fd, const void* buf, size_t n, off_t offset) {
    return (ssize_t)SET_ERRNO(SYSCALL4(__NR_pwrite64, fd, (long)buf, (long)n, (long)offset));
}

ssize_t read(int fd, void* buf, size_t nbytes) {
    return (ssize_t)SET_ERRNO(SYSCALL3(__NR_read, fd, (long)buf, (long)nbytes));
}

ssize_t readlink(const char* path, char* buf, size_t len) {
    return (ssize_t)SET_ERRNO(SYSCALL3(__NR_readlink, (long)path, (long)buf, (long)len));
}

ssize_t readlinkat(int fd, const char* path, char* buf, size_t len) {
    return (ssize_t)SET_ERRNO(SYSCALL4(__NR_readlinkat, fd, (long)path, (long)buf, (long)len));
}

int rmdir(const char* path) {
    return (int)SET_ERRNO(SYSCALL1(__NR_rmdir, (long)path));
}

int setegid(gid_t gid) {
    return (int)SET_ERRNO(SYSCALL2(__NR_setregid, (gid_t)-1, gid));
}

int seteuid(uid_t uid) {
    return (int)SET_ERRNO(SYSCALL2(__NR_setreuid, (uid_t)-1, uid));
}

int setgid(gid_t gid) {
    return (int)SET_ERRNO(SYSCALL1(__NR_setgid, gid));
}

int setpgid(pid_t pid, pid_t pgid) {
    return (int)SET_ERRNO(SYSCALL2(__NR_setpgid, pid, pgid));
}

int setpgrp(void) {
    return (int)SET_ERRNO(SYSCALL2(__NR_setpgid, 0, 0));
}

int setregid(gid_t rgid, gid_t egid) {
    return (int)SET_ERRNO(SYSCALL2(__NR_setregid, rgid, egid));
}

int setreuid(uid_t ruid, uid_t euid) {
    return (int)SET_ERRNO(SYSCALL2(__NR_setreuid, ruid, euid));
}

int setresuid(uid_t ruid, uid_t euid, uid_t suid) {
    return (int)SET_ERRNO(SYSCALL3(__NR_setresuid, ruid, euid, suid));
}

int setresgid(gid_t rgid, gid_t egid, gid_t sgid) {
    return (int)SET_ERRNO(SYSCALL3(__NR_setresgid, rgid, egid, sgid));
}

pid_t setsid(void) {
    return (pid_t)SET_ERRNO(SYSCALL0(__NR_setsid));
}

int setuid(uid_t uid) {
    return (int)SET_ERRNO(SYSCALL1(__NR_setuid, uid));
}

unsigned int sleep(unsigned int seconds) {
    struct timespec req = {(long)seconds, 0};
    struct timespec rem = {0, 0};
    if (SYSCALL2(__NR_nanosleep, (long)&req, (long)&rem) < 0)
        return (unsigned int)rem.tv_sec;

    return 0;
}

void swab(const void* from, void* to, ssize_t n) {
    const unsigned char* f = from;
    unsigned char* t = to;
    for (ssize_t i = 0; i + 1 < n; i += 2) {
        t[i] = f[i + 1];
        t[i + 1] = f[i];
    }
}

int symlink(const char* from, const char* to) {
    return (int)SET_ERRNO(SYSCALL2(__NR_symlink, (long)from, (long)to));
}

int symlinkat(const char* from, int tofd, const char* to) {
    return (int)SET_ERRNO(SYSCALL3(__NR_symlinkat, (long)from, tofd, (long)to));
}

void sync(void) {
    SYSCALL0(__NR_sync);
}

int syncfs(int fd) {
    return (int)SET_ERRNO(SYSCALL1(__NR_syncfs, fd));
}

long sysconf(int name) {
    switch (name) {
    case _SC_PAGESIZE: return 4096;
    case _SC_NGROUPS_MAX: return 65536;
    case _SC_OPEN_MAX: return 1024;
    case _SC_CLK_TCK: return 100;
    case _SC_HOST_NAME_MAX: return 64;
    case _SC_LOGIN_NAME_MAX: return 256;
    case _SC_TTY_NAME_MAX: return 32;
    case _SC_CHILD_MAX: return 32768;
    case _SC_ARG_MAX: return 2097152;
    case _SC_LINE_MAX: return 2048;
    default: return -1;
    }
}

pid_t tcgetpgrp(int fd) {
    pid_t pgrp;
    if (ioctl(fd, TIOCGPGRP, &pgrp) < 0)
        return -1;

    return pgrp;
}

int tcsetpgrp(int fd, pid_t pgrp) {
    return ioctl(fd, TIOCSPGRP, &pgrp);
}

int truncate(const char* file, off_t length) {
    return (int)SET_ERRNO(SYSCALL2(__NR_truncate, (long)file, (long)length));
}

char* ttyname(int fd) {
    static char buf[4096];
    int err = ttyname_r(fd, buf, sizeof(buf));
    if (err) {
        errno = err;
        return NULL;
    }
    return buf;
}

int ttyname_r(int fd, char* buf, size_t buflen) {
    char link[64];
    char tmp[4096];

    if (buflen == 0)
        return ERANGE;

    size_t i = 0;
    const char* prefix = "/proc/self/fd/";
    while (prefix[i]) {
        link[i] = prefix[i];
        i++;
    }

    int n = fd;
    if (n == 0) {
        link[i++] = '0';
    } else {
        char digits[20];
        int d = 0;
        while (n > 0) {
            digits[d++] = '0' + (n % 10);
            n /= 10;
        }
        while (d > 0) link[i++] = digits[--d];
    }
    link[i] = '\0';

    ssize_t len = readlink(link, tmp, sizeof(tmp) - 1);
    if (len < 0)
        return errno;

    tmp[len] = '\0';
    if ((size_t)len >= buflen)
        return ERANGE;

    memcpy(buf, tmp, len + 1);
    return 0;
}

int unlink(const char* name) {
    return (int)SET_ERRNO(SYSCALL1(__NR_unlink, (long)name));
}

int unlinkat(int fd, const char* name, int flag) {
    return (int)SET_ERRNO(SYSCALL3(__NR_unlinkat, fd, (long)name, flag));
}

int usleep(unsigned int useconds) {
    struct timespec req = {(long)(useconds / 1000000), (long)(useconds % 1000000) * 1000};
    return (int)SET_ERRNO(SYSCALL2(__NR_nanosleep, (long)&req, 0));
}

pid_t vfork(void) {
    return (pid_t)SET_ERRNO(SYSCALL0(__NR_vfork));
}

ssize_t write(int fd, const void* buf, size_t n) {
    return (ssize_t)SET_ERRNO(SYSCALL3(__NR_write, fd, (long)buf, (long)n));
}

int brk(void* addr) {
    return (int)SET_ERRNO(SYSCALL1(__NR_brk, (long)addr));
}

void* sbrk(long delta) {
    long cur = SYSCALL1(__NR_brk, 0);
    if (delta == 0)
        return (void*)cur;

    long next = SYSCALL1(__NR_brk, cur + delta);
    if (next != cur + delta) {
        errno = ENOMEM;
        return (void*)-1;
    }

    return (void*)cur;
}

int chroot(const char* path) {
    return (int)SET_ERRNO(SYSCALL1(__NR_chroot, (long)path));
}

int getentropy(void* buffer, size_t length) {
    return (int)SET_ERRNO(SYSCALL3(__NR_getrandom, (long)buffer, (long)length, 0));
}

int getdtablesize(void) {
    return (int)sysconf(_SC_OPEN_MAX);
}

int getpagesize(void) {
    return 4096;
}

int sethostname(const char* name, size_t len) {
    return (int)SET_ERRNO(SYSCALL2(__NR_sethostname, (long)name, (long)len));
}

int setdomainname(const char* name, size_t len) {
    return (int)SET_ERRNO(SYSCALL2(__NR_setdomainname, (long)name, (long)len));
}

int getdomainname(char* name, size_t len) {
    struct utsname u;
    if (uname(&u) != 0)
        return -1;

    size_t nlen = 0;
    while (u.domainname[nlen] && nlen < sizeof(u.domainname))
        nlen++;

    if (nlen >= len) {
        errno = ENAMETOOLONG;
        return -1;
    }

    memcpy(name, u.domainname, nlen + 1);
    return 0;
}

int vhangup(void) {
    return (int)SET_ERRNO(SYSCALL0(__NR_vhangup));
}

int acct(const char* name) {
    return (int)SET_ERRNO(SYSCALL1(__NR_acct, (long)name));
}

int daemon(int nochdir, int noclose) {
    pid_t pid = fork();
    if (pid < 0)
        return -1;

    if (pid > 0)
        _exit(0);

    if (setsid() < 0)
        return -1;

    if (!nochdir) {
        if (chdir("/") < 0)
            return -1;
    }

    if (!noclose) {
        int fd = open("/dev/null", O_RDWR);
        if (fd < 0)
            return -1;

        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO) close(fd);
    }

    return 0;
}

char* crypt(const char* key, const char* salt) {
    UNISTD_STUB_MSG
    return NULL;
}
