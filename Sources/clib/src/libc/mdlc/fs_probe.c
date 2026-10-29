#include <mdlc/fs_probe.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <errno.h>

#define PROBE_BUF_SIZE 65536 + 2048

static const char* fs_names[] = {
    "unknown", "ext2", "ext3", "ext4", "xfs", "btrfs", "f2fs",
    "ntfs", "fat12", "fat16", "fat32", "exfat", "erofs",
    "squashfs", "iso9660"
};

static const char* fs_mount_names[] = {
    NULL, "ext2", "ext3", "ext4", "xfs", "btrfs", "f2fs",
    "ntfs", "vfat", "vfat", "vfat", "exfat", "erofs",
    "squashfs", "iso9660"
};

static uint16_t fs_probe_read_le16(const uint8_t* buf, int off) {
    return (uint16_t)(buf[off] | (buf[off + 1] << 8));
}

static uint32_t fs_probe_read_le32(const uint8_t* buf, int off) {
    return (uint32_t)(buf[off] | (buf[off + 1] << 8) | (buf[off + 2] << 16) | (buf[off + 3] << 24));
}

static uint32_t fs_probe_read_be32(const uint8_t* buf, int off) {
    return (uint32_t)((buf[off] << 24) | (buf[off + 1] << 16) | (buf[off + 2] << 8) | buf[off + 3]);
}

static int fs_probe_read_at(int fd, off_t offset, uint8_t* buf, size_t len) {
    if (lseek(fd, offset, SEEK_SET) < 0)
        return -1;

    ssize_t n = read(fd, buf, len);
    return (n == (ssize_t)len) ? 0 : -1;
}

static fs_type_t fs_probe_detect_fat_type(const uint8_t* buf) {
    uint16_t bytes_per_sector = fs_probe_read_le16(buf, 11);
    uint8_t sectors_per_cluster = buf[13];
    if (bytes_per_sector == 0 || sectors_per_cluster == 0)
        return FS_TYPE_UNKNOWN;

    uint16_t reserved = fs_probe_read_le16(buf, 14);
    uint8_t num_fats = buf[16];
    uint16_t root_entry_count = fs_probe_read_le16(buf, 17);
    uint16_t total_sectors_16 = fs_probe_read_le16(buf, 19);
    uint16_t fat_size_16 = fs_probe_read_le16(buf, 22);
    uint32_t total_sectors_32 = fs_probe_read_le32(buf, 32);
    uint32_t fat_size_32 = fs_probe_read_le32(buf, 36);

    if (fat_size_16 == 0 && fat_size_32 != 0)
        return FS_TYPE_FAT32;

    uint32_t fat_size = fat_size_16 ? fat_size_16 : fat_size_32;
    uint32_t total_sectors = total_sectors_16 ? total_sectors_16 : total_sectors_32;
    uint32_t root_dir_sectors = ((root_entry_count * 32) + (bytes_per_sector - 1)) / bytes_per_sector;
    uint32_t data_sectors = total_sectors - (reserved + (num_fats * fat_size) + root_dir_sectors);
    uint32_t clusters = data_sectors / sectors_per_cluster;

    if (clusters < 4085)
        return FS_TYPE_FAT12;

    if (clusters < 65525)
        return FS_TYPE_FAT16;

    return FS_TYPE_FAT32;
}

static fs_type_t fs_probe_with_fd(int fd) {
    uint8_t buf[512];

    if (fs_probe_read_at(fd, 1080, buf, 2) == 0) {
        uint16_t ext_magic = fs_probe_read_le16(buf, 0);
        if (ext_magic == 0xEF53) {
            uint8_t feat[8];
            if (fs_probe_read_at(fd, 1116, feat, 8) == 0) {
                uint32_t feat_incompat = fs_probe_read_le32(feat, 4);
                if (feat_incompat & 0x40)
                    return FS_TYPE_EXT4;

                if (feat_incompat & 0x02)
                    return FS_TYPE_EXT3;
            }
            return FS_TYPE_EXT2;
        }
    }

    if (fs_probe_read_at(fd, 0, buf, 4) == 0) {
        if (buf[0] == 'X' && buf[1] == 'F' && buf[2] == 'S' && buf[3] == 'B')
            return FS_TYPE_XFS;
    }

    if (fs_probe_read_at(fd, 65536, buf, 8) == 0) {
        if (memcmp(buf, "_BHRfS_M", 8) == 0)
            return FS_TYPE_BTRFS;
    }

    if (fs_probe_read_at(fd, 1024, buf, 4) == 0) {
        if (fs_probe_read_le32(buf, 0) == 0xF2F52010)
            return FS_TYPE_F2FS;

        if (fs_probe_read_be32(buf, 0) == 0xE0F5E1E2)
            return FS_TYPE_EROFS;
    }

    if (fs_probe_read_at(fd, 3, buf, 8) == 0) {
        if (memcmp(buf, "NTFS    ", 8) == 0)
            return FS_TYPE_NTFS;

        if (memcmp(buf, "EXFAT   ", 8) == 0)
            return FS_TYPE_EXFAT;
    }

    if (fs_probe_read_at(fd, 0, buf, 4) == 0) {
        uint32_t sq = fs_probe_read_le32(buf, 0);
        if (sq == 0x73717368 || sq == 0x68737173)
            return FS_TYPE_SQUASHFS;
    }

    if (fs_probe_read_at(fd, 32769, buf, 5) == 0) {
        if (memcmp(buf, "CD001", 5) == 0)
            return FS_TYPE_ISO9660;
    }

    if (fs_probe_read_at(fd, 0, buf, 512) == 0) {
        if (buf[510] == 0x55 && buf[511] == 0xAA && buf[21] >= 0xF0)
            return fs_probe_detect_fat_type(buf);
    }

    return FS_TYPE_UNKNOWN;
}

fs_type_t fs_probe(const char* device) {
    if (!device)
        return FS_TYPE_UNKNOWN;

    int fd = open(device, O_RDONLY);
    if (fd < 0)
        return FS_TYPE_UNKNOWN;

    fs_type_t result = fs_probe_with_fd(fd);
    close(fd);
    return result;
}

fs_probe_result_t fs_probe_full(const char* device) {
    fs_probe_result_t r;
    r.type = fs_probe(device);
    r.name = fs_type_name(r.type);
    r.mount_name = fs_type_mount_name(r.type);
    return r;
}

const char* fs_type_name(fs_type_t type) {
    if ((unsigned)type >= sizeof(fs_names) / sizeof(fs_names[0]))
        return "unknown";

    return fs_names[type];
}

const char* fs_type_mount_name(fs_type_t type) {
    if ((unsigned)type >= sizeof(fs_mount_names) / sizeof(fs_mount_names[0]))
        return NULL;

    return fs_mount_names[type];
}

fs_probe_ext_result_t* fs_probe_external(const char* device, const char* db_file) {
    fprintf(stderr, "fs_probe_external(%s, %s): not implemented\n", device, db_file);
    errno = ENOSYS;
    return NULL;
}
