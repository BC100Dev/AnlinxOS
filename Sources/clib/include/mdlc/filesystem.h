#ifndef MDLC_FILESYSTEM_H
#define MDLC_FILESYSTEM_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {

#endif

typedef enum {
    FILE_TYPE_UNKNOWN = 0,
    FILE_TYPE_REGULAR = 1,
    FILE_TYPE_DIRECTORY = 2,
    FILE_TYPE_SYMLINK = 3,
    FILE_TYPE_BLOCK = 4,
    FILE_TYPE_CHAR = 5,
    FILE_TYPE_FIFO = 6,
    FILE_TYPE_SOCKET = 7
} file_type_t;

typedef struct {
    file_type_t type;
    uint64_t size;
    uint32_t permissions;
    uid_t uid;
    gid_t gid;
    struct timespec atime;
    struct timespec mtime;
    struct timespec ctime;
} file_info_t;

typedef struct dir_iter dir_iter_t;

typedef struct {
    char name[4096];
    file_type_t type;
} dir_entry_t;

int fs_stat(const char* path, file_info_t* out);
bool fs_exists(const char* path);
bool fs_is_dir(const char* path);
bool fs_is_file(const char* path);
bool fs_is_symlink(const char* path);

int fs_create_dir(const char* path, mode_t mode);
int fs_create_dirs(const char* path, mode_t mode);
int fs_remove(const char* path);
int fs_remove_all(const char* path);
int fs_rename(const char* from, const char* to);

int fs_copy(const char* from, const char* to, mode_t mode);
int fs_copy_all(const char* from, const char* to, mode_t mode);

int fs_symlink(const char* target, const char* link);
int fs_read_symlink(const char* path, char* out, size_t out_size);

int fs_dir_open(const char* path, dir_iter_t** out);
int fs_dir_next(dir_iter_t* iter, dir_entry_t* out);
void fs_dir_close(dir_iter_t* iter);

uint32_t fs_permissions(const char* path);
int fs_set_permissions(const char* path, uint32_t perms);

uint64_t fs_file_size(const char* path);
uint64_t fs_space_available(const char* path);
uint64_t fs_space_total(const char* path);

int fs_resolve_path(const char* path, char* out, size_t out_size);

#ifdef __cplusplus
}
#endif

#endif
