#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/types.h>

#define _POSIX_VERSION  200809L
#define _POSIX2_VERSION 200809L
#define _XOPEN_VERSION  700
#define _POSIX_VDISABLE '\0'

#ifndef STDIN_FILENO
#define STDIN_FILENO  0
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define F_OK 0
#define R_OK 4
#define W_OK 2
#define X_OK 1

#define F_ULOCK 0
#define F_LOCK 1
#define F_TLOCK 2
#define F_TEST 3

extern char** environ;
extern char* optarg;
extern int opterr, optind, optopt;

int access(const char* __name, int __type);
unsigned int alarm(unsigned int __seconds);
int chdir(const char* __path);
int chown(const char* __file, uid_t __owner, gid_t __group);
int close(int __fd);
size_t confstr(int __name, char* __buf, size_t __len);
char* crypt(const char* __key, const char* __salt);
int dup(int __fd);
int dup2(int __fd, int __fd2);
int dup3(int __fd, int __fd2, int __flags);
void _exit(int __status) __attribute__((__noreturn__));
int execl(const char* __path, const char* __arg, ...);
int execle(const char* __path, const char* __arg, ...);
int execlp(const char* __file, const char* __arg, ...);
int execv(const char* __path, char* const __argv[]);
int execve(const char* __path, char* const __argv[], char* const __envp[]);
int execvp(const char* __file, char* const __argv[]);
int execvpe(const char* __file, char* const __argv[], char* const __envp[]);
int faccessat(int __fd, const char* __file, int __type, int __flag);
int fchdir(int __fd);
int fchown(int __fd, uid_t __owner, gid_t __group);
int fchownat(int __fd, const char* __file, uid_t __owner, gid_t __group, int __flag);
int fdatasync(int __fildes);
int fexecve(int __fd, char* const __argv[], char* const __envp[]);
pid_t fork(void);
long fpathconf(int __fd, int __name);
int fsync(int __fd);
int ftruncate(int __fd, off_t __length);
char* getcwd(char* __buf, size_t __size);
gid_t getegid(void);
uid_t geteuid(void);
gid_t getgid(void);
int getgroups(int __size, gid_t __list[]);
long gethostid(void);
int gethostname(char* __name, size_t __len);
char* getlogin(void);
int getlogin_r(char* __name, size_t __name_len);
int getopt(int __argc, char* const __argv[], const char* __shortopts);
pid_t getpgid(pid_t __pid);
pid_t getpgrp(void);
pid_t getpid(void);
pid_t getppid(void);
pid_t getsid(pid_t __pid);
uid_t getuid(void);
int isatty(int __fd);
int lchown(const char* __file, uid_t __owner, gid_t __group);
int link(const char* __from, const char* __to);
int linkat(int __fromfd, const char* __from, int __tofd, const char* __to, int __flags);
int lockf(int __fd, int __cmd, off_t __len);
off_t lseek(int __fd, off_t __offset, int __whence);
int nice(int __inc);
long pathconf(const char* __path, int __name);
int pause(void);
int pipe(int __pipedes[2]);
int pipe2(int __pipedes[2], int __flags);
ssize_t pread(int __fd, void* __buf, size_t __nbytes, off_t __offset);
ssize_t pwrite(int __fd, const void* __buf, size_t __n, off_t __offset);
ssize_t read(int __fd, void* __buf, size_t __nbytes);
ssize_t readlink(const char* __path, char* __buf, size_t __len);
ssize_t readlinkat(int __fd, const char* __path, char* __buf, size_t __len);
int rmdir(const char* __path);
int setegid(gid_t __gid);
int seteuid(uid_t __uid);
int setgid(gid_t __gid);
int setpgid(pid_t __pid, pid_t __pgid);
int setpgrp(void);
int setregid(gid_t __rgid, gid_t __egid);
int setreuid(uid_t __ruid, uid_t __euid);
int setresuid(uid_t __ruid, uid_t __euid, uid_t __suid);
int setresgid(gid_t __rgid, gid_t __egid, gid_t __sgid);
pid_t setsid(void);
int setuid(uid_t __uid);
unsigned int sleep(unsigned int __seconds);
void swab(const void* __from, void* __to, ssize_t __n);
int symlink(const char* __from, const char* __to);
int symlinkat(const char* __from, int __tofd, const char* __to);
void sync(void);
int syncfs(int __fd);
long sysconf(int __name);
pid_t tcgetpgrp(int __fd);
int tcsetpgrp(int __fd, pid_t __pgrp_id);
int truncate(const char* __file, off_t __length);
char* ttyname(int __fd);
int ttyname_r(int __fd, char* __buf, size_t __buflen);
int unlink(const char* __name);
int unlinkat(int __fd, const char* __name, int __flag);
int usleep(unsigned int __useconds);
pid_t vfork(void);
ssize_t write(int __fd, const void* __buf, size_t __n);
int brk(void* __addr);
void* sbrk(long __delta);
int chroot(const char* __path);
int getentropy(void* __buffer, size_t __length);
int getdtablesize(void);
int getpagesize(void);
int sethostname(const char* __name, size_t __len);
int setdomainname(const char* __name, size_t __len);
int getdomainname(char* __name, size_t __len);
int vhangup(void);
int acct(const char* __name);
int daemon(int __nochdir, int __noclose);
int getresuid(uid_t* __ruid, uid_t* __euid, uid_t* __suid);
int getresgid(gid_t* __rgid, gid_t* __egid, gid_t* __sgid);

#endif /* _UNISTD_H */
