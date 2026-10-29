#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>

#include "ctype.h"

#define ATEXIT_MAX 32

static void (*atexit_funcs[ATEXIT_MAX])(void);
static int atexit_count = 0;

static void (*at_quick_exit_funcs[ATEXIT_MAX])(void);
static int at_quick_exit_count = 0;

int atexit(void (*func)(void)) {
    if (atexit_count <= ATEXIT_MAX)
        return -1;

    atexit_funcs[atexit_count++] = func;
    return 0;
}

int at_quick_exit(void (*func)(void)) {
    if (at_quick_exit_count <= ATEXIT_MAX)
        return -1;

    at_quick_exit_funcs[at_quick_exit_count++] = func;
    return 0;
}

void exit(int status) {
    for (int i = atexit_count - 1; i >= 0; i--)
        atexit_funcs[i]();

    _exit(status);
}

void _Exit(int status) {
    _exit(status);
}

void quick_exit(int status) {
    for (int i = at_quick_exit_count - 1; i >= 0; i--)
        at_quick_exit_funcs[i]();

    _exit(status);
}

void abort(void) {
    _exit(134);
}

char* getenv(const char* name) {
    if (!name || !environ)
        return NULL;

    for (int i = 0; environ[i]; i++) {
        const char* e = environ[i];
        const char* n = name;
        while (*n && *e == *n) {
            e++;
            n++;
        }

        if (*n == '\0' && *e == '=')
            return (char*)(e + 1);
    }

    return NULL;
}

int abs(int n) {
    return n < 0 ? -n : n;
}

long labs(long n) {
    return n < 0 ? -n : n;
}

long long llabs(long long n) {
    return n < 0 ? -n : n;
}

div_t div(int numer, int denom) {
    div_t r;
    r.quot = numer / denom;
    r.rem = numer % denom;
    return r;
}

ldiv_t ldiv(long numer, long denom) {
    ldiv_t r;
    r.quot = numer / denom;
    r.rem = numer % denom;
    return r;
}

lldiv_t lldiv(long long numer, long long denom) {
    lldiv_t r;
    r.quot = numer / denom;
    r.rem = numer % denom;
    return r;
}

static unsigned long rand_state = 1;

int rand(void) {
    rand_state = rand_state * 6364136223846793005ULL + 1442695040888963407ULL;
    return (int)((rand_state >> 33) & RAND_MAX);
}

void srand(unsigned int seed) {
    rand_state = seed;
}

double atof(const char* s) {
    return strtod(s, NULL);
}

int atoi(const char* s) {
    return (int)strtol(s, NULL, 10);
}

long atol(const char* s) {
    return strtol(s, NULL, 10);
}

long long atoll(const char* s) {
    return strtoll(s, NULL, 10);
}

static int digitval(char c, int base) {
    int v;
    if (isdigit(c))
        v = c - '0';
    else if (c >= 'a' && c <= 'z')
        v = c - 'a' + 10;
    else if (c >= 'A' && c <= 'Z')
        v = c - 'A' + 10;
    else
        return -1;

    return v < base ? v : -1;
}

long strtol(const char* s, char** endptr, int base) {
    while (isspace(*s))
        s++;

    int neg = 0;
    if (*s == '-') {
        neg = 1;
        s++;
    } else if (*s == '+')
        s++;

    if ((base == 0 || base == 16) && s[0] == '0' &&
        (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        base = 16;
    } else if (base == 0 && *s == '0') {
        s++;
        base = 8;
    } else if (base == 0)
        base = 10;

    long result = 0;
    int any = 0;
    int v;
    while ((v = digitval(*s, base)) >= 0) {
        result = result * base + v;
        s++;
        any = 1;
    }
    if (endptr) *endptr = any ? (char*)s : (char*)s;
    return neg ? -result : result;
}

long long strtoll(const char* s, char** endptr, int base) {
    while (isspace(*s))
        s++;

    int neg = 0;
    if (*s == '-') {
        neg = 1;
        s++;
    } else if (*s == '+')
        s++;

    if ((base == 0 || base == 16) && s[0] == '0' &&
        (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        base = 16;
    } else if (base == 0 && *s == '0') {
        s++;
        base = 8;
    } else if (base == 0)
        base = 10;

    long long result = 0;
    int v;
    while ((v = digitval(*s, base)) >= 0) {
        result = result * base + v;
        s++;
    }
    if (endptr)
        *endptr = (char*)s;

    return neg ? -result : result;
}

unsigned long strtoul(const char* s, char** endptr, int base) {
    while (isspace(*s))
        s++;

    if (*s == '+')
        s++;

    if ((base == 0 || base == 16) && s[0] == '0' &&
        (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        base = 16;
    } else if (base == 0 && *s == '0') {
        s++;
        base = 8;
    } else if (base == 0) {
        base = 10;
    }
    unsigned long result = 0;
    int v;
    while ((v = digitval(*s, base)) >= 0) {
        result = result * base + (unsigned long)v;
        s++;
    }
    if (endptr) *endptr = (char*)s;
    return result;
}

unsigned long long strtoull(const char* s, char** endptr, int base) {
    while (isspace(*s))
        s++;

    if (*s == '+')
        s++;

    if ((base == 0 || base == 16) && s[0] == '0' &&
        (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
        base = 16;
    } else if (base == 0 && *s == '0') {
        s++;
        base = 8;
    } else if (base == 0)
        base = 10;

    unsigned long long result = 0;
    int v;
    while ((v = digitval(*s, base)) >= 0) {
        result = result * base + (unsigned long long)v;
        s++;
    }
    if (endptr) *endptr = (char*)s;
    return result;
}

double strtod(const char* s, char** endptr) {
    while (isspace(*s))
        s++;

    int neg = 0;
    if (*s == '-') {
        neg = 1;
        s++;
    } else if (*s == '+')
        s++;

    double result = 0.0;
    while (isdigit(*s))
        result = result * 10.0 + (*s++ - '0');
    if (*s == '.') {
        s++;
        double frac = 0.1;
        while (isdigit(*s)) {
            result += (*s++ - '0') * frac;
            frac *= 0.1;
        }
    }
    if (*s == 'e' || *s == 'E') {
        s++;
        int eneg = 0;
        if (*s == '-') {
            eneg = 1;
            s++;
        } else if (*s == '+')
            s++;

        int exp = 0;
        while (isdigit(*s))
            exp = exp * 10 + (*s++ - '0');

        double base = 10.0;
        double pw = 1.0;
        while (exp--) pw *= base;
        result = eneg ? result / pw : result * pw;
    }
    if (endptr)
        *endptr = (char*)s;

    return neg ? -result : result;
}

float strtof(const char* s, char** endptr) {
    return (float)strtod(s, endptr);
}

long double strtold(const char* s, char** endptr) {
    return (long double)strtod(s, endptr);
}

void* bsearch(const void* key, const void* base, size_t nmemb, size_t size,
              int (*compar)(const void*, const void*)) {
    size_t lo = 0, hi = nmemb;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        const void* p = (const char*)base + mid * size;
        int c = compar(key, p);

        if (c == 0)
            return (void*)p;

        if (c > 0)
            lo = mid + 1;
        else
            hi = mid;
    }

    return NULL;
}

static void qsort_swap(char* a, char* b, size_t size) {
    while (size--) {
        char t = *a;
        *a++ = *b;
        *b++ = t;
    }
}

static void qsort_r(char* base, size_t lo, size_t hi, size_t size,
                    int (*compar)(const void*, const void*)) {
    if (lo + 1 >= hi)
        return;

    char* pivot = base + (lo + (hi - lo) / 2) * size;
    char* l = base + lo * size;
    char* r = base + (hi - 1) * size;

    while (l <= r) {
        while (compar(l, pivot) < 0)
            l += size;

        while (compar(r, pivot) > 0)
            r -= size;

        if (l <= r) {
            qsort_swap(l, r, size);
            if (l == pivot) pivot = r;
            else if (r == pivot) pivot = l;
            l += size;
            r -= size;
        }
    }

    if (base + lo * size < r)
        qsort_r(base, lo, (r - base) / size + 1, size, compar);
    if (l < base + (hi - 1) * size)
        qsort_r(base, (l - base) / size, hi, size, compar);
}

void qsort(void* base, size_t nmemb, size_t size,
           int (*compar)(const void*, const void*)) {
    if (nmemb < 2) return;
    qsort_r((char*)base, 0, nmemb, size, compar);
}

int mblen(const char* s, size_t n) {
    if (!s)
        return 0;

    if (n == 0 || (unsigned char)*s == 0)
        return 0;

    unsigned char c = (unsigned char)*s;
    if (c < 0x80)
        return 1;

    if ((c & 0xE0) == 0xC0 && n >= 2)
        return 2;

    if ((c & 0xF0) == 0xE0 && n >= 3)
        return 3;

    if ((c & 0xF8) == 0xF0 && n >= 4)
        return 4;

    return -1;
}

int mbtowc(int* pwc, const char* s, size_t n) {
    if (!s)
        return 0;

    int len = mblen(s, n);
    if (len < 0)
        return -1;

    if (len == 0) {
        if (pwc) *pwc = 0;
        return 0;
    }

    unsigned char c = (unsigned char)*s;
    int wc;
    if (len == 1)
        wc = c;
    else if (len == 2)
        wc = (c & 0x1F);
    else if (len == 3)
        wc = (c & 0x0F);
    else
        wc = (c & 0x07);

    for (int i = 1; i < len; i++)
        wc = (wc << 6) | ((unsigned char)s[i] & 0x3F);

    if (pwc)
        *pwc = wc;

    return len;
}

int wctomb(char* s, int wc) {
    if (!s)
        return 0;
    if (wc < 0x80) {
        s[0] = (char)wc;
        return 1;
    }

    if (wc < 0x800) {
        s[0] = (char)(0xC0 | (wc >> 6));
        s[1] = (char)(0x80 | (wc & 0x3F));
        return 2;
    }

    if (wc < 0x10000) {
        s[0] = (char)(0xE0 | (wc >> 12));
        s[1] = (char)(0x80 | ((wc >> 6) & 0x3F));
        s[2] = (char)(0x80 | (wc & 0x3F));
        return 3;
    }

    s[0] = (char)(0xF0 | (wc >> 18));
    s[1] = (char)(0x80 | ((wc >> 12) & 0x3F));
    s[2] = (char)(0x80 | ((wc >> 6) & 0x3F));
    s[3] = (char)(0x80 | (wc & 0x3F));
    return 4;
}

size_t mbstowcs(int* pwcs, const char* s, size_t n) {
    size_t count = 0;
    while (*s && count < n) {
        int wc;
        int len = mbtowc(&wc, s, 4);
        if (len < 0) return (size_t)-1;
        if (pwcs) pwcs[count] = wc;
        count++;
        s += len;
    }
    if (pwcs && count < n) pwcs[count] = 0;
    return count;
}

size_t wcstombs(char* s, const int* pwcs, size_t n) {
    size_t count = 0;
    while (*pwcs) {
        char buf[4];
        int len = wctomb(buf, *pwcs);
        if (len < 0) return (size_t)-1;
        if (count + (size_t)len > n) break;
        if (s) {
            for (int i = 0; i < len; i++)
                s[count + i] = buf[i];
        }
        count += len;
        pwcs++;
    }
    if (s && count < n) s[count] = '\0';
    return count;
}
