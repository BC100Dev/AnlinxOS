#ifndef SYS_MOUNT_H
#define SYS_MOUNT_H

#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BLOCK_SIZE      1024
#define BLOCK_SIZE_BITS 10

enum {
    MS_RDONLY = 1,
    MS_NOSUID = 2,
    MS_NODEV = 4,
    MS_NOEXEC = 8,
    MS_SYNCHRONOUS = 16,
    MS_REMOUNT = 32,
    MS_MANDLOCK = 64,
    MS_DIRSYNC = 128,
    MS_NOSYMFOLLOW = 256,
    MS_NOATIME = 1024,
    MS_NODIRATIME = 2048,
    MS_BIND = 4096,
    MS_MOVE = 8192,
    MS_REC = 16384,
    MS_SILENT = 32768,
    MS_POSIXACL = 1 << 16,
    MS_UNBINDABLE = 1 << 17,
    MS_PRIVATE = 1 << 18,
    MS_SLAVE = 1 << 19,
    MS_SHARED = 1 << 20,
    MS_RELATIME = 1 << 21,
    MS_KERNMOUNT = 1 << 22,
    MS_I_VERSION = 1 << 23,
    MS_STRICTATIME = 1 << 24,
    MS_LAZYTIME = 1 << 25,
    MS_ACTIVE = 1 << 30,
    MS_NOUSER = 1 << 31
};

#define MS_RDONLY       MS_RDONLY
#define MS_NOSUID       MS_NOSUID
#define MS_NODEV        MS_NODEV
#define MS_NOEXEC       MS_NOEXEC
#define MS_SYNCHRONOUS  MS_SYNCHRONOUS
#define MS_REMOUNT      MS_REMOUNT
#define MS_MANDLOCK     MS_MANDLOCK
#define MS_DIRSYNC      MS_DIRSYNC
#define MS_NOSYMFOLLOW  MS_NOSYMFOLLOW
#define MS_NOATIME      MS_NOATIME
#define MS_NODIRATIME   MS_NODIRATIME
#define MS_BIND         MS_BIND
#define MS_MOVE         MS_MOVE
#define MS_REC          MS_REC
#define MS_SILENT       MS_SILENT
#define MS_POSIXACL     MS_POSIXACL
#define MS_UNBINDABLE   MS_UNBINDABLE
#define MS_PRIVATE      MS_PRIVATE
#define MS_SLAVE        MS_SLAVE
#define MS_SHARED       MS_SHARED
#define MS_RELATIME     MS_RELATIME
#define MS_KERNMOUNT    MS_KERNMOUNT
#define MS_I_VERSION    MS_I_VERSION
#define MS_STRICTATIME  MS_STRICTATIME
#define MS_LAZYTIME     MS_LAZYTIME
#define MS_ACTIVE       MS_ACTIVE
#define MS_NOUSER       MS_NOUSER

#define MS_RMT_MASK (MS_RDONLY | MS_SYNCHRONOUS | MS_MANDLOCK | MS_I_VERSION | MS_LAZYTIME)

#define MS_MGC_VAL 0xc0ed0000
#define MS_MGC_MSK 0xffff0000

#define BLKROSET        _IO(0x12, 93)
#define BLKROGET        _IO(0x12, 94)
#define BLKRRPART       _IO(0x12, 95)
#define BLKGETSIZE      _IO(0x12, 96)
#define BLKFLSBUF       _IO(0x12, 97)
#define BLKRASET        _IO(0x12, 98)
#define BLKRAGET        _IO(0x12, 99)
#define BLKFRASET       _IO(0x12, 100)
#define BLKFRAGET       _IO(0x12, 101)
#define BLKSECTSET      _IO(0x12, 102)
#define BLKSECTGET      _IO(0x12, 103)
#define BLKSSZGET       _IO(0x12, 104)
#define BLKBSZGET       _IOR(0x12, 112, size_t)
#define BLKBSZSET       _IOW(0x12, 113, size_t)
#define BLKGETSIZE64    _IOR(0x12, 114, size_t)

enum {
    MNT_FORCE = 1,
    MNT_DETACH = 2,
    MNT_EXPIRE = 4,
    UMOUNT_NOFOLLOW = 8
};

#define MNT_FORCE       MNT_FORCE
#define MNT_DETACH      MNT_DETACH
#define MNT_EXPIRE      MNT_EXPIRE
#define UMOUNT_NOFOLLOW UMOUNT_NOFOLLOW

int mount(const char* special_file, const char* dir, const char* fstype,
          unsigned long int rwflag, const void* data);
int umount(const char* special_file);
int umount2(const char* special_file, int flags);

#ifdef __cplusplus
}
#endif

#endif
