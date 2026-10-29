#ifndef FCNTL_H
#define FCNTL_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2

#if defined(__x86_64__) || defined(__aarch64__)
#define O_CREAT 0x00040
#define O_EXCL 0x00080
#define O_NOCTTY 0x00100
#define O_TRUNC 0x00200
#define O_APPEND 0x00400
#define O_NONBLOCK 0x00800
#define O_DSYNC 0x01000
#define O_SYNC 0x101000
#define O_RSYNC 0x101000
#define O_DIRECTORY 0x10000
#define O_NOFOLLOW 0x20000
#define O_CLOEXEC 0x80000
#define O_ASYNC 0x02000
#define O_LARGEFILE 0x08000
#define O_NOATIME 0x40000
#define O_PATH 0x200000
#define O_TMPFILE 0x410000
#endif

#define F_DUPFD 0
#define F_GETFD 1
#define F_SETFD 2
#define F_GETFL 3
#define F_SETFL 4
#define F_GETLK 5
#define F_SETLK 6
#define F_SETLKW 7
#define F_SETOWN 8
#define F_GETOWN 9
#define F_SETSIG 10
#define F_GETSIG 11
#define F_DUPFD_CLOEXEC 1030

#define FD_CLOEXEC 1

#define F_RDLCK 0
#define F_WRLCK 1
#define F_UNLCK 2

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define AT_FDCWD            (-100)
#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_REMOVEDIR        0x200
#define AT_SYMLINK_FOLLOW   0x400
#define AT_EACCESS          0x200
#define AT_EMPTY_PATH       0x1000

struct flock {
    short l_type;
    short l_whence;
    off_t l_start;
    off_t l_len;
    pid_t l_pid;
};

int open(const char* path, int flags, ...);
int openat(int fd, const char* path, int flags, ...);
int creat(const char* path, mode_t mode);
int fcntl(int fd, int cmd, ...);

#ifdef __cplusplus
}
#endif

#endif /* FCNTL_H */
