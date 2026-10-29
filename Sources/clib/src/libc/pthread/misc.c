#include <pthread.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/prctl.h>

static void int_to_str(int n, char* buf, size_t* len) {
    char tmp[32];
    int i = 0;
    if (n == 0) {
        tmp[i++] = '0';
    } else {
        while (n > 0) {
            tmp[i++] = '0' + n % 10;
            n /= 10;
        }
    }
    *len = (size_t)i;
    for (int k = 0; k < i; k++) buf[k] = tmp[i - 1 - k];
    buf[i] = '\0';
}

static void build_proc_path(int tid, const char* suffix, char* out) {
    char num[32];
    size_t nlen;
    int_to_str(tid, num, &nlen);

    const char* prefix = "/proc/";
    size_t plen = strlen(prefix);
    size_t slen = strlen(suffix);

    memcpy(out, prefix, plen);
    memcpy(out + plen, num, nlen);
    memcpy(out + plen + nlen, suffix, slen + 1);
}

int pthread_setname_np(pthread_t thread, const char* name) {
    if (!name)
        return EINVAL;

    if (thread == pthread_self())
        return prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0);

    char path[64];
    build_proc_path((int)thread, "/comm", path);
    int fd = open(path, O_WRONLY);
    if (fd < 0)
        return errno;

    size_t len = strlen(name);
    if (len > 15)
        len = 15;

    write(fd, name, len);
    write(fd, "\n", 1);
    close(fd);
    return 0;
}

int pthread_getname_np(pthread_t thread, char* name, size_t len) {
    if (!name || len == 0)
        return EINVAL;

    if (thread == pthread_self())
        return prctl(PR_GET_NAME, (unsigned long)name, 0, 0, 0);

    char path[64];
    build_proc_path((int)thread, "/comm", path);
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return errno;

    ssize_t r = read(fd, name, len - 1);
    close(fd);
    if (r < 0)
        return errno;

    if (r > 0 && name[r - 1] == '\n')
        r--;

    name[r] = '\0';
    return 0;
}

int pthread_getconcurrency(void) {
    return 0;
}

int pthread_setconcurrency(int level) {
    (void)level;
    return 0;
}
