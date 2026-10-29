#include <sys/statvfs.h>
#include <errno.h>
#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

struct kernel_statfs {
    long f_type;
    long f_bsize;
    long f_blocks;
    long f_bfree;
    long f_bavail;
    long f_files;
    long f_ffree;
    long f_fsid[2];
    long f_namelen;
    long f_frsize;
    long f_flags;
    long f_spare[4];
};

static void kernel_statfs_to_statvfs(const struct kernel_statfs* kfs, struct statvfs* out) {
    out->f_bsize = (unsigned long)kfs->f_bsize;
    out->f_frsize = (unsigned long)(kfs->f_frsize ? kfs->f_frsize : kfs->f_bsize);
    out->f_blocks = (fsblkcnt_t)kfs->f_blocks;
    out->f_bfree = (fsblkcnt_t)kfs->f_bfree;
    out->f_bavail = (fsblkcnt_t)kfs->f_bavail;
    out->f_files = (fsfilcnt_t)kfs->f_files;
    out->f_ffree = (fsfilcnt_t)kfs->f_ffree;
    out->f_favail = (fsfilcnt_t)kfs->f_ffree;
    out->f_fsid = (unsigned long)kfs->f_fsid[0];
    out->f_flag = (unsigned long)kfs->f_flags;
    out->f_namemax = (unsigned long)kfs->f_namelen;
}

int statvfs(const char* path, struct statvfs* buf) {
    struct kernel_statfs kfs;
    int ret = (int)SET_ERRNO(SYSCALL2(__NR_statfs, (long)path, (long)&kfs));
    if (ret == 0)
        kernel_statfs_to_statvfs(&kfs, buf);
    return ret;
}

int fstatvfs(int fd, struct statvfs* buf) {
    struct kernel_statfs kfs;
    int ret = (int)SET_ERRNO(SYSCALL2(__NR_fstatfs, (long)fd, (long)&kfs));
    if (ret == 0)
        kernel_statfs_to_statvfs(&kfs, buf);
    return ret;
}
