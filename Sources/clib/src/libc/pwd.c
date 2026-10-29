#include <pwd.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_FILE_SIZE 2048
#define USERS_BASE "/Data/userdata"

static struct passwd pw_entry;
static char pw_buf[MAX_FILE_SIZE];
static DIR* pw_dir = NULL;

static int is_sep(const char* p) {
    return (unsigned char)p[0] == 0x1F && (unsigned char)p[1] == 0xAE;
}

static char* next_field(char* p, char* end) {
    while (p + 1 < end) {
        if (is_sep(p)) {
            p[0] = '\0';
            return p + 2;
        }
        p++;
    }
    return NULL;
}

static int parse_user_config(char* buf, size_t len, struct passwd* pwd) {
    char* end = buf + len;
    char* p = buf;
    char* fields[7];
    int i = 0;

    fields[i++] = p;
    while (i < 7) {
        p = next_field(p, end);
        if (!p)
            break;

        fields[i++] = p;
        p = fields[i - 1];
    }
    if (i < 7)
        return -1;

    pwd->pw_uid = (uid_t)atoi(fields[0]);
    pwd->pw_gid = (gid_t)atoi(fields[1]);
    pwd->pw_name = fields[2];
    pwd->pw_gecos = fields[3];
    pwd->pw_shell = fields[4];
    pwd->pw_passwd = (char*)"x";
    pwd->pw_dir = fields[6];
    return 0;
}

static int read_user_config(const char* path, struct passwd* pwd, char* buf, size_t bufsize) {
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return -1;

    ssize_t n = read(fd, buf, bufsize - 1);
    close(fd);
    if (n <= 0)
        return -1;

    buf[n] = '\0';
    return parse_user_config(buf, (size_t)n, pwd);
}

static void build_config_path(uid_t uid, char* out, size_t out_size) {
    char uidstr[32];
    int i = 0,
        n = (int)uid;
    char tmp[32];
    int d = 0;
    if (n == 0) {
        tmp[d++] = '0';
    } else {
        while (n > 0) {
            tmp[d++] = '0' + n % 10;
            n /= 10;
        }
    }

    while (d > 0) uidstr[i++] = tmp[--d];
    uidstr[i] = '\0';

    size_t base_len = strlen(USERS_BASE);
    size_t uid_len = (size_t)i;
    const char* suffix = "/config";
    size_t suf_len = strlen(suffix);
    if (base_len + 1 + uid_len + suf_len + 1 > out_size) {
        out[0] = '\0';
        return;
    }
    memcpy(out, USERS_BASE, base_len);
    out[base_len] = '/';
    memcpy(out + base_len + 1, uidstr, uid_len);
    memcpy(out + base_len + 1 + uid_len, suffix, suf_len + 1);
}

static int is_uid_dir(const char* s) {
    if (!s || !*s)
        return 0;

    while (*s) {
        if (!isdigit((unsigned char)*s))
            return 0;

        s++;
    }
    return 1;
}

struct passwd* getpwuid(uid_t uid) {
    char path[512];
    build_config_path(uid, path, sizeof(path));
    if (read_user_config(path, &pw_entry, pw_buf, sizeof(pw_buf)) != 0)
        return NULL;

    return &pw_entry;
}

int getpwuid_r(uid_t uid, struct passwd* pwd, char* buf, size_t buflen, struct passwd** result) {
    *result = NULL;
    char path[512];
    build_config_path(uid, path, sizeof(path));
    if (read_user_config(path, pwd, buf, buflen) != 0)
        return ENOENT;

    *result = pwd;
    return 0;
}

void setpwent(void) {
    if (pw_dir) {
        closedir(pw_dir);
        pw_dir = NULL;
    }
    pw_dir = opendir(USERS_BASE);
}

void endpwent(void) {
    if (pw_dir) {
        closedir(pw_dir);
        pw_dir = NULL;
    }
}

struct passwd* getpwent(void) {
    if (!pw_dir)
        setpwent();

    if (!pw_dir)
        return NULL;

    struct dirent* entry;
    while ((entry = readdir(pw_dir)) != NULL) {
        if (!is_uid_dir(entry->d_name))
            continue;

        uid_t uid = (uid_t)atoi(entry->d_name);
        char path[512];
        build_config_path(uid, path, sizeof(path));
        if (read_user_config(path, &pw_entry, pw_buf, sizeof(pw_buf)) == 0)
            return &pw_entry;
    }
    return NULL;
}

struct passwd* getpwnam(const char* name) {
    if (!name)
        return NULL;

    setpwent();
    struct passwd* pw;
    while ((pw = getpwent()) != NULL) {
        if (strcmp(pw->pw_name, name) == 0) {
            endpwent();
            return pw;
        }
    }

    endpwent();
    return NULL;
}

int getpwnam_r(const char* name, struct passwd* pwd, char* buf, size_t buflen, struct passwd** result) {
    *result = NULL;
    if (!name)
        return EINVAL;

    DIR* d = opendir(USERS_BASE);
    if (!d)
        return errno;

    struct dirent* entry;
    int ret = ENOENT;
    while ((entry = readdir(d)) != NULL) {
        if (!is_uid_dir(entry->d_name))
            continue;

        uid_t uid = (uid_t)atoi(entry->d_name);
        char path[512];
        build_config_path(uid, path, sizeof(path));
        if (read_user_config(path, pwd, buf, buflen) == 0 && strcmp(pwd->pw_name, name) == 0) {
            *result = pwd;
            ret = 0;
            break;
        }
    }
    closedir(d);
    return ret;
}
