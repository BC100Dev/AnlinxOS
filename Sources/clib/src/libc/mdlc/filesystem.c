#include <mdlc/filesystem.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

struct dir_iter {
    DIR* handle;
};

static file_type_t mode_to_file_type(mode_t mode) {
    if (S_ISREG(mode))
        return FILE_TYPE_REGULAR;
    if (S_ISDIR(mode))
        return FILE_TYPE_DIRECTORY;
    if (S_ISLNK(mode))
        return FILE_TYPE_SYMLINK;
    if (S_ISBLK(mode))
        return FILE_TYPE_BLOCK;
    if (S_ISCHR(mode))
        return FILE_TYPE_CHAR;
    if (S_ISFIFO(mode))
        return FILE_TYPE_FIFO;
    if (S_ISSOCK(mode))
        return FILE_TYPE_SOCKET;

    return FILE_TYPE_UNKNOWN;
}

int fs_stat(const char* path, file_info_t* out) {
    if (!path || !out) {
        errno = EINVAL;
        return -1;
    }
    struct stat st;
    if (lstat(path, &st) != 0)
        return -1;

    out->type = mode_to_file_type(st.st_mode);
    out->size = (uint64_t)st.st_size;
    out->permissions = (uint32_t)(st.st_mode & 0777);
    out->uid = st.st_uid;
    out->gid = st.st_gid;
    out->atime.tv_sec = (long)st.st_atime;
    out->atime.tv_nsec = (long)st.st_atime_nsec;
    out->mtime.tv_sec = (long)st.st_mtime;
    out->mtime.tv_nsec = (long)st.st_mtime_nsec;
    out->ctime.tv_sec = (long)st.st_ctime;
    out->ctime.tv_nsec = (long)st.st_ctime_nsec;
    return 0;
}

bool fs_exists(const char* path) {
    if (!path)
        return false;

    struct stat st;
    return lstat(path, &st) == 0;
}

bool fs_is_dir(const char* path) {
    if (!path)
        return false;

    struct stat st;
    if (lstat(path, &st) != 0)
        return false;

    return S_ISDIR(st.st_mode);
}

bool fs_is_file(const char* path) {
    if (!path)
        return false;

    struct stat st;
    if (lstat(path, &st) != 0)
        return false;

    return S_ISREG(st.st_mode);
}

bool fs_is_symlink(const char* path) {
    if (!path)
        return false;

    struct stat st;
    if (lstat(path, &st) != 0)
        return false;

    return S_ISLNK(st.st_mode);
}

int fs_create_dir(const char* path, mode_t mode) {
    if (!path) {
        errno = EINVAL;
        return -1;
    }
    return mkdir(path, mode);
}

int fs_create_dirs(const char* path, mode_t mode) {
    if (!path) {
        errno = EINVAL;
        return -1;
    }
    char tmp[4096];
    strncpy(tmp, path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    size_t len = strlen(tmp);
    if (len > 0 && tmp[len - 1] == '/')
        tmp[len - 1] = '\0';

    for (char* p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            if (mkdir(tmp, mode) != 0 && errno != EEXIST)
                return -1;

            *p = '/';
        }
    }
    if (mkdir(tmp, mode) != 0 && errno != EEXIST)
        return -1;

    return 0;
}

int fs_remove(const char* path) {
    if (!path) {
        errno = EINVAL;
        return -1;
    }
    return remove(path);
}

int fs_remove_all(const char* path) {
    if (!path) {
        errno = EINVAL;
        return -1;
    }
    struct stat st;
    if (lstat(path, &st) != 0) return -1;
    if (!S_ISDIR(st.st_mode)) return unlink(path);
    DIR* d = opendir(path);
    if (!d) return -1;
    struct dirent* entry;
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char child[4096];
        size_t plen = strlen(path);
        size_t nlen = strlen(entry->d_name);
        if (plen + nlen + 2 > sizeof(child)) {
            closedir(d);
            errno = ENAMETOOLONG;
            return -1;
        }
        memcpy(child, path, plen);
        child[plen] = '/';
        memcpy(child + plen + 1, entry->d_name, nlen + 1);
        if (fs_remove_all(child) != 0) {
            closedir(d);
            return -1;
        }
    }
    closedir(d);
    return rmdir(path);
}

int fs_rename(const char* from, const char* to) {
    if (!from || !to) {
        errno = EINVAL;
        return -1;
    }
    return rename(from, to);
}

int fs_copy(const char* from, const char* to, mode_t mode) {
    if (!from || !to) {
        errno = EINVAL;
        return -1;
    }
    int src = open(from, O_RDONLY);
    if (src < 0) return -1;
    int dst = open(to, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (dst < 0) {
        close(src);
        return -1;
    }
    char buf[65536];
    ssize_t n;
    int ret = 0;
    while ((n = read(src, buf, sizeof(buf))) > 0) {
        if (write(dst, buf, (size_t)n) != n) {
            ret = -1;
            break;
        }
    }
    if (n < 0) ret = -1;
    close(src);
    close(dst);
    return ret;
}

int fs_copy_all(const char* from, const char* to, mode_t mode) {
    if (!from || !to) {
        errno = EINVAL;
        return -1;
    }
    struct stat st;
    if (lstat(from, &st) != 0)
        return -1;

    if (!S_ISDIR(st.st_mode))
        return fs_copy(from, to, mode);

    if (mkdir(to, 0755) != 0 && errno != EEXIST)
        return -1;

    DIR* d = opendir(from);
    if (!d)
        return -1;

    struct dirent* entry;
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char src_child[4096], dst_child[4096];
        size_t flen = strlen(from), tlen = strlen(to), nlen = strlen(entry->d_name);
        if (flen + nlen + 2 > sizeof(src_child) || tlen + nlen + 2 > sizeof(dst_child)) {
            closedir(d);
            errno = ENAMETOOLONG;
            return -1;
        }

        memcpy(src_child, from, flen);
        src_child[flen] = '/';
        memcpy(src_child + flen + 1, entry->d_name, nlen + 1);
        memcpy(dst_child, to, tlen);
        dst_child[tlen] = '/';
        memcpy(dst_child + tlen + 1, entry->d_name, nlen + 1);
        if (fs_copy_all(src_child, dst_child, mode) != 0) {
            closedir(d);
            return -1;
        }
    }

    closedir(d);
    return 0;
}

int fs_symlink(const char* target, const char* link) {
    if (!target || !link) {
        errno = EINVAL;
        return -1;
    }
    return symlink(target, link);
}

int fs_read_symlink(const char* path, char* out, size_t out_size) {
    if (!path || !out || out_size == 0) {
        errno = EINVAL;
        return -1;
    }
    ssize_t n = readlink(path, out, out_size - 1);
    if (n < 0)
        return -1;

    out[n] = '\0';
    return 0;
}

int fs_dir_open(const char* path, dir_iter_t** out) {
    if (!path || !out) {
        errno = EINVAL;
        return -1;
    }
    dir_iter_t* iter = malloc(sizeof(dir_iter_t));
    if (!iter) {
        errno = ENOMEM;
        return -1;
    }
    iter->handle = opendir(path);
    if (!iter->handle) {
        free(iter);
        return -1;
    }
    *out = iter;
    return 0;
}

int fs_dir_next(dir_iter_t* iter, dir_entry_t* out) {
    if (!iter || !out) {
        errno = EINVAL;
        return -1;
    }
    struct dirent* entry;
    while ((entry = readdir(iter->handle)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        strncpy(out->name, entry->d_name, sizeof(out->name) - 1);
        out->name[sizeof(out->name) - 1] = '\0';
        switch (entry->d_type) {
        case DT_REG:
            out->type = FILE_TYPE_REGULAR;
            break;
        case DT_DIR:
            out->type = FILE_TYPE_DIRECTORY;
            break;
        case DT_LNK:
            out->type = FILE_TYPE_SYMLINK;
            break;
        case DT_BLK:
            out->type = FILE_TYPE_BLOCK;
            break;
        case DT_CHR:
            out->type = FILE_TYPE_CHAR;
            break;
        case DT_FIFO:
            out->type = FILE_TYPE_FIFO;
            break;
        case DT_SOCK:
            out->type = FILE_TYPE_SOCKET;
            break;
        default:
            out->type = FILE_TYPE_UNKNOWN;
            break;
        }
        return 0;
    }
    return -1;
}

void fs_dir_close(dir_iter_t* iter) {
    if (!iter)
        return;

    closedir(iter->handle);
    free(iter);
}

uint32_t fs_permissions(const char* path) {
    if (!path) {
        errno = EINVAL;
        return 0;
    }
    struct stat st;
    if (lstat(path, &st) != 0)
        return 0;

    return (uint32_t)(st.st_mode & 0777);
}

int fs_set_permissions(const char* path, uint32_t perms) {
    if (!path) {
        errno = EINVAL;
        return -1;
    }
    return chmod(path, (mode_t)perms);
}

uint64_t fs_file_size(const char* path) {
    if (!path) {
        errno = EINVAL;
        return 0;
    }
    struct stat st;
    if (lstat(path, &st) != 0)
        return 0;

    return (uint64_t)st.st_size;
}

uint64_t fs_space_available(const char* path) {
    if (!path) {
        errno = EINVAL;
        return 0;
    }
    struct statvfs sv;
    if (statvfs(path, &sv) != 0)
        return 0;

    return (uint64_t)sv.f_bavail * (uint64_t)sv.f_frsize;
}

uint64_t fs_space_total(const char* path) {
    if (!path) {
        errno = EINVAL;
        return 0;
    }
    struct statvfs sv;
    if (statvfs(path, &sv) != 0)
        return 0;

    return (uint64_t)sv.f_blocks * (uint64_t)sv.f_frsize;
}

int fs_resolve_path(const char* path, char* out, size_t out_size) {
    if (!path || !out || out_size == 0) {
        errno = EINVAL;
        return -1;
    }
    char tmp[4096];
    if (!realpath(path, tmp))
        return -1;

    size_t len = strlen(tmp);
    if (len >= out_size) {
        errno = ENAMETOOLONG;
        return -1;
    }
    memcpy(out, tmp, len + 1);
    return 0;
}
