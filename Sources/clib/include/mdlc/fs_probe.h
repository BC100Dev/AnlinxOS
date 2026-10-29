#ifndef MDLC_FS_PROBE_H
#define MDLC_FS_PROBE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {



#endif

typedef enum {
    FS_TYPE_UNKNOWN = 0,
    FS_TYPE_EXT2 = 1,
    FS_TYPE_EXT3 = 2,
    FS_TYPE_EXT4 = 3,
    FS_TYPE_XFS = 4,
    FS_TYPE_BTRFS = 5,
    FS_TYPE_F2FS = 6,
    FS_TYPE_NTFS = 7,
    FS_TYPE_FAT12 = 8,
    FS_TYPE_FAT16 = 9,
    FS_TYPE_FAT32 = 10,
    FS_TYPE_EXFAT = 11,
    FS_TYPE_EROFS = 12,
    FS_TYPE_SQUASHFS = 13,
    FS_TYPE_ISO9660 = 14
} fs_type_t;

typedef struct {
    fs_type_t type;
    const char* name;
    const char* mount_name;
} fs_probe_result_t;

typedef struct {
    fs_type_t type;
    const char* name;
    const char* mount_name;
} fs_probe_ext_result_t;

fs_type_t fs_probe(const char* device);
fs_probe_result_t fs_probe_full(const char* device);
fs_probe_ext_result_t* fs_probe_external(const char* device, const char* db_file);

const char* fs_type_name(fs_type_t type);
const char* fs_type_mount_name(fs_type_t type);

#ifdef __cplusplus
}
#endif

#endif
