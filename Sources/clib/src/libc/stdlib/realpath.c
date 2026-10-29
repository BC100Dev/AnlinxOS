#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

char* realpath(const char* path, char* resolved) {
    if (!path) {
        errno = EINVAL;
        return NULL;
    }

    char tmp[4096];
    char out[4096];
    size_t out_len = 0;
    int link_depth = 0;

    if (path[0] != '/') {
        if (!getcwd(tmp, sizeof(tmp))) return NULL;
        out_len = strlen(tmp);
        memcpy(out, tmp, out_len);
    } else {
        out_len = 0;
    }

    const char* p = path;
    while (*p) {
        while (*p == '/')
            p++;

        if (*p == '\0')
            break;

        const char* start = p;
        while (*p && *p != '/')
            p++;

        size_t comp_len = (size_t)(p - start);

        if (comp_len == 1 && start[0] == '.')
            continue;

        if (comp_len == 2 && start[0] == '.' && start[1] == '.') {
            while (out_len > 0 && out[out_len - 1] != '/')
                out_len--;

            if (out_len > 0)
                out_len--;

            continue;
        }

        if (out_len + 1 + comp_len >= sizeof(out)) {
            errno = ENAMETOOLONG;
            return NULL;
        }

        out[out_len++] = '/';
        memcpy(out + out_len, start, comp_len);
        out_len += comp_len;
        out[out_len] = '\0';

        struct stat st;
        if (lstat(out, &st) != 0) return NULL;

        if (S_ISLNK(st.st_mode)) {
            if (++link_depth > 40) {
                errno = ELOOP;
                return NULL;
            }
            char link_target[4096];
            ssize_t n = readlink(out, link_target, sizeof(link_target) - 1);
            if (n < 0) return NULL;
            link_target[n] = '\0';

            char rest[4096];
            size_t rest_len = strlen(p);
            if (rest_len >= sizeof(rest)) {
                errno = ENAMETOOLONG;
                return NULL;
            }
            memcpy(rest, p, rest_len + 1);

            if (link_target[0] == '/') {
                out_len = 0;
            } else {
                while (out_len > 0 && out[out_len - 1] != '/')
                    out_len--;

                if (out_len > 0)
                    out_len--;
            }

            size_t tlen = strlen(link_target);
            if (tlen + 1 + rest_len >= sizeof(tmp)) {
                errno = ENAMETOOLONG;
                return NULL;
            }
            memcpy(tmp, link_target, tlen);
            if (rest_len > 0) {
                tmp[tlen] = '/';
                memcpy(tmp + tlen + 1, rest, rest_len + 1);
            } else {
                tmp[tlen] = '\0';
            }
            p = tmp;
        }
    }

    if (out_len == 0) {
        out[0] = '/';
        out_len = 1;
    }
    out[out_len] = '\0';

    if (resolved) {
        memcpy(resolved, out, out_len + 1);
        return resolved;
    }

    char* ret = malloc(out_len + 1);
    if (!ret) {
        errno = ENOMEM;
        return NULL;
    }
    memcpy(ret, out, out_len + 1);
    return ret;
}
