#include <dirent.h>
#include <stddef.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "internal/syscall.h"
#include "internal/syscall_nr.h"

#define DIR_BUF_SIZE 4096

struct __dir_stream {
    int fd;
    char buf[DIR_BUF_SIZE];
    int buf_pos;
    int buf_len;
    struct dirent current;
};

struct kernel_dirent64 {
    unsigned long long d_ino;
    long long d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];
};

DIR* opendir(const char* path) {
    int fd = (int)SET_ERRNO(SYSCALL3(__NR_open, (long)path, 0x10000 | 0x20000, 0));
    if (fd < 0)
        return NULL;

    DIR* dir = malloc(sizeof(DIR));
    if (!dir) {
        errno = ENOMEM;
        SYSCALL1(__NR_close, fd);
        return NULL;
    }

    dir->fd = fd;
    dir->buf_pos = 0;
    dir->buf_len = 0;
    return dir;
}

DIR* fdopendir(int fd) {
    DIR* dir = malloc(sizeof(DIR));
    if (!dir) {
        errno = ENOMEM;
        return NULL;
    }

    dir->fd = fd;
    dir->buf_pos = 0;
    dir->buf_len = 0;
    return dir;
}

struct dirent* readdir(DIR* dirp) {
    if (!dirp) {
        errno = EBADF;
        return NULL;
    }
    while (1) {
        if (dirp->buf_pos >= dirp->buf_len) {
            long n = SYSCALL3(__NR_getdents64, (long)dirp->fd, (long)dirp->buf, DIR_BUF_SIZE);
            if (n <= 0) {
                if (n < 0) errno = (int)-n;
                return NULL;
            }

            dirp->buf_pos = 0;
            dirp->buf_len = (int)n;
        }
        struct kernel_dirent64* kd = (struct kernel_dirent64*)(dirp->buf + dirp->buf_pos);
        dirp->buf_pos += kd->d_reclen;
        dirp->current.d_ino = (ino_t)kd->d_ino;
        dirp->current.d_off = (off_t)kd->d_off;
        dirp->current.d_reclen = kd->d_reclen;
        dirp->current.d_type = kd->d_type;
        memcpy(dirp->current.d_name, kd->d_name, 256);
        return &dirp->current;
    }
}

int readdir_r(DIR* dirp, struct dirent* entry, struct dirent** result) {
    if (!dirp) return EBADF;
    struct dirent* d = readdir(dirp);
    if (!d) {
        *result = NULL;
        return errno;
    }
    *entry = *d;
    *result = entry;
    return 0;
}

int closedir(DIR* dirp) {
    if (!dirp) {
        errno = EBADF;
        return -1;
    }
    int ret = (int)SET_ERRNO(SYSCALL1(__NR_close, (long)dirp->fd));
    free(dirp);
    return ret;
}

void rewinddir(DIR* dirp) {
    if (!dirp) return;
    SYSCALL3(__NR_lseek, (long)dirp->fd, 0, 0);
    dirp->buf_pos = 0;
    dirp->buf_len = 0;
}

void seekdir(DIR* dirp, long pos) {
    if (!dirp) return;
    SYSCALL3(__NR_lseek, (long)dirp->fd, pos, 0);
    dirp->buf_pos = 0;
    dirp->buf_len = 0;
}

long telldir(DIR* dirp) {
    if (!dirp) {
        errno = EBADF;
        return -1;
    }
    return (long)SYSCALL3(__NR_lseek, (long)dirp->fd, 0, 1);
}

int dirfd(DIR* dirp) {
    if (!dirp) {
        errno = EINVAL;
        return -1;
    }
    return dirp->fd;
}
