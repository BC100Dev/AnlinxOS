#include <string.h>
#include <stdlib.h>
#include <stddef.h>

void* memcpy(void* dst, const void* src, size_t n) {
    unsigned char* d = dst;
    const unsigned char* s = src;

    while (n--)
        *d++ = *s++;

    return dst;
}

void* memmove(void* dst, const void* src, size_t n) {
    unsigned char* d = dst;
    const unsigned char* s = src;

    if (d < s)
        while (n--) *d++ = *s++;
    else {
        d += n;
        s += n;
        while (n--) *--d = *--s;
    }

    return dst;
}

void* memset(void* dst, int c, size_t n) {
    unsigned char* d = dst;
    while (n--)
        *d++ = (unsigned char)c;

    return dst;
}

int memcmp(const void* a, const void* b, size_t n) {
    const unsigned char *p = a, *q = b;
    while (n--) {
        if (*p != *q) return *p - *q;
        p++;
        q++;
    }
    return 0;
}

void* memchr(const void* s, int c, size_t n) {
    const unsigned char* p = s;
    while (n--) {
        if (*p == (unsigned char)c) return (void*)p;
        p++;
    }

    return NULL;
}

size_t strlen(const char* s) {
    size_t n = 0;
    while (*s++)
        n++;

    return n;
}

size_t strnlen(const char* s, size_t maxlen) {
    size_t n = 0;
    while (n < maxlen && *s++)
        n++;

    return n;
}

char* strcpy(char* dst, const char* src) {
    char* d = dst;
    while ((*d++ = *src++)) {
    }
    return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
    char* d = dst;
    while (n && ((*d++ = *src++)))
        n--;

    while (n--)
        *d++ = '\0';

    return dst;
}

char* strcat(char* dst, const char* src) {
    char* d = dst;
    while (*d)
        d++;

    while ((*d++ = *src++)) {
    }
    return dst;
}

char* strncat(char* dst, const char* src, size_t n) {
    char* d = dst;
    while (*d)
        d++;

    while (n-- && *src)
        *d++ = *src++;

    *d = '\0';
    return dst;
}

int strcmp(const char* a, const char* b) {
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

int strncmp(const char* a, const char* b, size_t n) {
    if (!n)
        return 0;

    while (--n && *a && *a == *b) {
        a++;
        b++;
    }

    return (unsigned char)*a - (unsigned char)*b;
}

int strcoll(const char* a, const char* b) {
    return strcmp(a, b);
}

size_t strxfrm(char* dst, const char* src, size_t n) {
    size_t len = strlen(src);
    if (n > 0) {
        strncpy(dst, src, n - 1);
        dst[n - 1] = '\0';
    }
    return len;
}

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == (char)c) return (char*)s;
        s++;
    }
    return c == '\0' ? (char*)s : NULL;
}

char* strrchr(const char* s, int c) {
    const char* last = NULL;
    while (*s) {
        if (*s == (char)c) last = s;
        s++;
    }

    if (c == '\0')
        return (char*)s;

    return (char*)last;
}

size_t strcspn(const char* s, const char* reject) {
    size_t n = 0;
    while (*s) {
        const char* r = reject;
        while (*r) {
            if (*s == *r)
                return n;

            r++;
        }

        s++;
        n++;
    }
    return n;
}

size_t strspn(const char* s, const char* accept) {
    size_t n = 0;
    while (*s) {
        const char* a = accept;
        int found = 0;
        while (*a) {
            if (*s == *a++) {
                found = 1;
                break;
            }
        }
        if (!found)
            return n;

        s++;
        n++;
    }
    return n;
}

char* strpbrk(const char* s, const char* accept) {
    while (*s) {
        const char* a = accept;
        while (*a) {
            if (*s == *a) return (char*)s;
            a++;
        }
        s++;
    }
    return NULL;
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle)
        return (char*)haystack;

    size_t nlen = strlen(needle);
    while (*haystack) {
        if (*haystack == *needle && memcmp(haystack, needle, nlen) == 0)
            return (char*)haystack;

        haystack++;
    }
    return NULL;
}

char* strtok_r(char* s, const char* delim, char** saveptr) {
    if (!s) s = *saveptr;
    s += strspn(s, delim);
    if (!*s) {
        *saveptr = s;
        return NULL;
    }
    char* end = s + strcspn(s, delim);
    if (*end) {
        *end = '\0';
        *saveptr = end + 1;
    } else
        *saveptr = end;

    return s;
}

char* strtok(char* s, const char* delim) {
    static char* saved;
    return strtok_r(s, delim, &saved);
}

char* strerror(int errnum) {
    switch (errnum) {
    case 0: return "Success";
    case 1: return "Operation not permitted";
    case 2: return "No such file or directory";
    case 3: return "No such process";
    case 4: return "Interrupted system call";
    case 5: return "Input/output error";
    case 6: return "No such device or address";
    case 7: return "Argument list too long";
    case 8: return "Exec format error";
    case 9: return "Bad file descriptor";
    case 10: return "No child processes";
    case 11: return "Resource temporarily unavailable";
    case 12: return "Cannot allocate memory";
    case 13: return "Permission denied";
    case 14: return "Bad address";
    case 16: return "Device or resource busy";
    case 17: return "File exists";
    case 18: return "Invalid cross-device link";
    case 19: return "No such device";
    case 20: return "Not a directory";
    case 21: return "Is a directory";
    case 22: return "Invalid argument";
    case 23: return "Too many open files in system";
    case 24: return "Too many open files";
    case 25: return "Inappropriate ioctl for device";
    case 26: return "Text file busy";
    case 27: return "File too large";
    case 28: return "No space left on device";
    case 29: return "Illegal seek";
    case 30: return "Read-only file system";
    case 31: return "Too many links";
    case 32: return "Broken pipe";
    case 33: return "Numerical argument out of domain";
    case 34: return "Numerical result out of range";
    case 36: return "Resource deadlock avoided";
    case 38: return "Function not implemented";
    case 39: return "Directory not empty";
    case 40: return "Too many levels of symbolic links";
    case 42: return "No message of desired type";
    case 43: return "Identifier removed";
    case 62: return "Timer expired";
    case 63: return "Out of streams resources";
    case 75: return "Value too large for defined data type";
    case 84: return "Invalid or incomplete multibyte or wide character";
    case 98: return "Address already in use";
    case 99: return "Cannot assign requested address";
    case 100: return "Network is down";
    case 101: return "Network is unreachable";
    case 104: return "Connection reset by peer";
    case 107: return "Transport endpoint is not connected";
    case 110: return "Connection timed out";
    case 111: return "Connection refused";
    case 113: return "No route to host";
    case 115: return "Operation now in progress";
    default: return "Unknown error";
    }
}

char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* copy = malloc(len);
    if (!copy)
        return NULL;

    memcpy(copy, s, len);
    return copy;
}

char* strndup(const char* s, size_t n) {
    size_t len = strnlen(s, n);
    char* copy = malloc(len + 1);
    if (!copy)
        return NULL;

    memcpy(copy, s, len);
    copy[len] = '\0';
    return copy;
}
