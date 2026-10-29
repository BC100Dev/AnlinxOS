#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "file.h"
#include "fmt.h"

static int match_scanset(const char* set, int c, int negated) {
    const char* p = set;
    int matched = 0;
    while (*p && *p != ']') {
        if (*(p + 1) == '-' && *(p + 2) && *(p + 2) != ']') {
            if (c >= (unsigned char)*p && c <= (unsigned char)*(p + 2))
                matched = 1;

            p += 3;
        } else {
            if ((unsigned char)*p == c)
                matched = 1;

            p++;
        }
    }
    return negated ? !matched : matched;
}

int vsscanf(const char* str, const char* fmt, va_list ap) {
    const char* s = str;
    int assigned = 0;

    while (*fmt) {
        if (isspace((unsigned char)*fmt)) {
            while (isspace((unsigned char)*s))
                s++;

            fmt++;
            continue;
        }

        if (*fmt != '%') {
            if (*s != *fmt)
                return assigned;

            s++;
            fmt++;
            continue;
        }

        fmt++;

        int suppress = 0;
        if (*fmt == '*') {
            suppress = 1;
            fmt++;
        }

        int width = parse_width(&fmt);
        lenmod_t len = parse_lenmod(&fmt);

        char spec = *fmt++;

        if (spec == '%') {
            while (isspace((unsigned char)*s))
                s++;

            if (*s != '%')
                return assigned;

            s++;
            continue;
        }

        if (spec != 'c' && spec != '[' && spec != 'n')
            while (isspace((unsigned char)*s)) s++;

        switch (spec) {
        case 'd':
        case 'i':
        case 'u':
        case 'o':
        case 'x':
        case 'X':
        case 'p': {
            if (!*s)
                return assigned;

            char tmp[64];
            int tlen = 0;
            int lim = width > 0 ? width : (int)sizeof(tmp) - 1;

            if ((*s == '-' || *s == '+') && tlen < lim)
                tmp[tlen++] = *s++;

            if (spec == 'i' && *s == '0' && tlen < lim) {
                tmp[tlen++] = *s++;
                if ((*s == 'x' || *s == 'X') && tlen < lim)
                    tmp[tlen++] = *s++;
            } else if ((spec == 'x' || spec == 'X' || spec == 'p') &&
                *s == '0' && tlen < lim) {
                tmp[tlen++] = *s++;
                if ((*s == 'x' || *s == 'X') && tlen < lim)
                    tmp[tlen++] = *s++;
            }

            while (*s && tlen < lim && isxdigit((unsigned char)*s))
                tmp[tlen++] = *s++;

            tmp[tlen] = '\0';

            if (tlen == 0)
                return assigned;

            if (!suppress) {
                int base = 10;
                if (spec == 'o')
                    base = 8;
                else if (spec == 'x' || spec == 'X' || spec == 'p')
                    base = 16;
                else if (spec == 'i')
                    base = 0;

                if (spec == 'u' || spec == 'o' || spec == 'x' || spec == 'X') {
                    unsigned long long val = strtoull(tmp, NULL, base);
                    if (len == LEN_LL)
                        *va_arg(ap, unsigned long long *) = val;
                    else if (len == LEN_L)
                        *va_arg(ap, unsigned long *) = (unsigned long)val;
                    else if (len == LEN_J)
                        *va_arg(ap, uintmax_t *) = (uintmax_t)val;
                    else if (len == LEN_Z)
                        *va_arg(ap, size_t *) = (size_t)val;
                    else if (len == LEN_H)
                        *va_arg(ap, unsigned short *) = (unsigned short)val;
                    else if (len == LEN_HH)
                        *va_arg(ap, unsigned char *) = (unsigned char)val;
                    else
                        *va_arg(ap, unsigned int *) = (unsigned int)val;
                } else if (spec == 'p') {
                    *va_arg(ap, void **) = (void*)(uintptr_t)strtoull(tmp, NULL, 16);
                } else {
                    long long val = strtoll(tmp, NULL, base);
                    if (len == LEN_LL)
                        *va_arg(ap, long long *) = val;
                    else if (len == LEN_L)
                        *va_arg(ap, long *) = (long)val;
                    else if (len == LEN_J)
                        *va_arg(ap, intmax_t *) = (intmax_t)val;
                    else if (len == LEN_Z)
                        *va_arg(ap, ssize_t *) = (ssize_t)val;
                    else if (len == LEN_T)
                        *va_arg(ap, ptrdiff_t *) = (ptrdiff_t)val;
                    else if (len == LEN_H)
                        *va_arg(ap, short *) = (short)val;
                    else if (len == LEN_HH)
                        *va_arg(ap, signed char *) = (signed char)val;
                    else
                        *va_arg(ap, int *) = (int)val;
                }
                assigned++;
            }
            break;
        }
        case 'f':
        case 'e':
        case 'g':
        case 'E':
        case 'G': {
            if (!*s)
                return assigned;

            char tmp[64];
            int tlen = 0;
            int lim = width > 0 ? width : (int)sizeof(tmp) - 1;

            if ((*s == '-' || *s == '+') && tlen < lim)
                tmp[tlen++] = *s++;
            while (*s && tlen < lim && (isdigit((unsigned char)*s) || *s == '.'))
                tmp[tlen++] = *s++;
            if (*s && tlen < lim && (*s == 'e' || *s == 'E')) {
                tmp[tlen++] = *s++;
                if (*s && tlen < lim && (*s == '+' || *s == '-'))
                    tmp[tlen++] = *s++;
                while (*s && tlen < lim && isdigit((unsigned char)*s))
                    tmp[tlen++] = *s++;
            }
            tmp[tlen] = '\0';

            if (tlen == 0)
                return assigned;

            if (!suppress) {
                if (len == LEN_BIG_L)
                    *va_arg(ap, long double *) = strtold(tmp, NULL);
                else if (len == LEN_L)
                    *va_arg(ap, double *) = strtod(tmp, NULL);
                else
                    *va_arg(ap, float *) = strtof(tmp, NULL);

                assigned++;
            }
            break;
        }
        case 'c': {
            int lim = width > 0 ? width : 1;
            if (!*s)
                return assigned;

            if (!suppress) {
                char* dst = va_arg(ap, char *);
                for (int i = 0; i < lim && *s; i++)
                    dst[i] = *s++;

                assigned++;
            } else {
                for (int i = 0; i < lim && *s; i++)
                    s++;
            }
            break;
        }
        case 's': {
            if (!*s) return assigned;
            int lim = width > 0 ? width : -1;
            if (!suppress) {
                char* dst = va_arg(ap, char *);
                int i = 0;
                while (*s && !isspace((unsigned char)*s) && (lim < 0 || i < lim))
                    dst[i++] = *s++;

                dst[i] = '\0';
                assigned++;
            } else {
                while (*s && !isspace((unsigned char)*s) && (lim < 0 || lim-- > 0))
                    s++;
            }
            break;
        }
        case '[': {
            int negated = 0;
            if (*fmt == '^') {
                negated = 1;
                fmt++;
            }
            const char* set = fmt;
            if (*fmt == ']')
                fmt++;

            while (*fmt && *fmt != ']')
                fmt++;

            if (*fmt == ']')
                fmt++;

            int lim = width > 0 ? width : -1;
            if (!*s)
                return assigned;

            if (!suppress) {
                char* dst = va_arg(ap, char *);
                int i = 0;
                while (*s && (lim < 0 || i < lim) && match_scanset(set, (unsigned char)*s, negated))
                    dst[i++] = *s++;

                if (i == 0)
                    return assigned;

                dst[i] = '\0';
                assigned++;
            } else {
                while (*s && (lim < 0 || lim-- > 0) && match_scanset(set, (unsigned char)*s, negated))
                    s++;
            }
            break;
        }
        case 'n':
            if (!suppress) {
                if (len == LEN_LL)
                    *va_arg(ap, long long *) = (long long)(s - str);
                else if (len == LEN_L)
                    *va_arg(ap, long *) = (long)(s - str);
                else if (len == LEN_H)
                    *va_arg(ap, short *) = (short)(s - str);
                else if (len == LEN_HH)
                    *va_arg(ap, signed char *) = (signed char)(s - str);
                else
                    *va_arg(ap, int *) = (int)(s - str);
            }
            break;
        default:
            return assigned;
        }
    }

    return assigned;
}

int vfscanf(FILE* stream, const char* fmt, va_list ap) {
    const char* f = fmt;
    int assigned = 0;
    long start_pos = ftell(stream);

    while (*f) {
        if (isspace((unsigned char)*f)) {
            int c;
            while ((c = fgetc(stream)) != EOF && isspace((unsigned char)c)) {
            }

            if (c != EOF)
                ungetc(c, stream);

            f++;
            continue;
        }

        if (*f != '%') {
            int c = fgetc(stream);
            if (c == EOF || c != (unsigned char)*f)
                return assigned;

            f++;
            continue;
        }

        f++;

        int suppress = 0;
        if (*f == '*') {
            suppress = 1;
            f++;
        }

        int width = parse_width(&f);
        lenmod_t len = parse_lenmod(&f);
        char spec = *f++;

        if (spec == '%') {
            int c;
            while ((c = fgetc(stream)) != EOF && isspace((unsigned char)c)) {
            }

            if (c == EOF || c != '%')
                return assigned;

            continue;
        }

        if (spec != 'c' && spec != '[' && spec != 'n') {
            int c;
            while ((c = fgetc(stream)) != EOF && isspace((unsigned char)c)) {
            }

            if (c != EOF)
                ungetc(c, stream);
        }

        switch (spec) {
        case 'd':
        case 'i':
        case 'u':
        case 'o':
        case 'x':
        case 'X':
        case 'p': {
            char tmp[64];
            int tlen = 0;
            int lim = width > 0 ? width : (int)sizeof(tmp) - 1;
            int c;

            c = fgetc(stream);
            if (c == EOF)
                return assigned;

            if ((c == '-' || c == '+') && tlen < lim) {
                tmp[tlen++] = (char)c;
                c = fgetc(stream);
            }
            if (spec == 'i' && c == '0' && tlen < lim) {
                tmp[tlen++] = (char)c;
                c = fgetc(stream);

                if ((c == 'x' || c == 'X') && tlen < lim) {
                    tmp[tlen++] = (char)c;
                    c = fgetc(stream);
                }
            } else if ((spec == 'x' || spec == 'X' || spec == 'p') && c == '0' && tlen < lim) {
                tmp[tlen++] = (char)c;
                c = fgetc(stream);

                if ((c == 'x' || c == 'X') && tlen < lim) {
                    tmp[tlen++] = (char)c;
                    c = fgetc(stream);
                }
            }
            while (c != EOF && tlen < lim && isxdigit((unsigned char)c)) {
                tmp[tlen++] = (char)c;
                c = fgetc(stream);
            }

            if (c != EOF)
                ungetc(c, stream);

            tmp[tlen] = '\0';
            if (tlen == 0)
                return assigned;

            if (!suppress) {
                int base = 10;
                if (spec == 'o')
                    base = 8;
                else if (spec == 'x' || spec == 'X' || spec == 'p')
                    base = 16;
                else if (spec == 'i')
                    base = 0;

                if (spec == 'u' || spec == 'o' || spec == 'x' || spec == 'X') {
                    unsigned long long val = strtoull(tmp, NULL, base);
                    if (len == LEN_LL)
                        *va_arg(ap, unsigned long long *) = val;
                    else if (len == LEN_L)
                        *va_arg(ap, unsigned long *) = (unsigned long)val;
                    else if (len == LEN_J)
                        *va_arg(ap, uintmax_t *) = (uintmax_t)val;
                    else if (len == LEN_Z)
                        *va_arg(ap, size_t *) = (size_t)val;
                    else if (len == LEN_H)
                        *va_arg(ap, unsigned short *) = (unsigned short)val;
                    else if (len == LEN_HH)
                        *va_arg(ap, unsigned char *) = (unsigned char)val;
                    else
                        *va_arg(ap, unsigned int *) = (unsigned int)val;
                } else if (spec == 'p') {
                    *va_arg(ap, void **) = (void*)(uintptr_t)strtoull(tmp, NULL, 16);
                } else {
                    long long val = strtoll(tmp, NULL, base);
                    if (len == LEN_LL)
                        *va_arg(ap, long long *) = val;
                    else if (len == LEN_L)
                        *va_arg(ap, long *) = (long)val;
                    else if (len == LEN_J)
                        *va_arg(ap, intmax_t *) = (intmax_t)val;
                    else if (len == LEN_Z)
                        *va_arg(ap, ssize_t *) = (ssize_t)val;
                    else if (len == LEN_T)
                        *va_arg(ap, ptrdiff_t *) = (ptrdiff_t)val;
                    else if (len == LEN_H)
                        *va_arg(ap, short *) = (short)val;
                    else if (len == LEN_HH)
                        *va_arg(ap, signed char *) = (signed char)val;
                    else
                        *va_arg(ap, int *) = (int)val;
                }
                assigned++;
            }
            break;
        }
        case 'f':
        case 'e':
        case 'g':
        case 'E':
        case 'G': {
            char tmp[64];
            int tlen = 0;
            int lim = width > 0 ? width : (int)sizeof(tmp) - 1;
            int c;

            c = fgetc(stream);
            if (c == EOF) return assigned;
            if ((c == '-' || c == '+') && tlen < lim) {
                tmp[tlen++] = (char)c;
                c = fgetc(stream);
            }
            while (c != EOF && tlen < lim && (isdigit((unsigned char)c) || c == '.')) {
                tmp[tlen++] = (char)c;
                c = fgetc(stream);
            }
            if (c != EOF && tlen < lim && (c == 'e' || c == 'E')) {
                tmp[tlen++] = (char)c;
                c = fgetc(stream);
                if (c != EOF && tlen < lim && (c == '+' || c == '-')) {
                    tmp[tlen++] = (char)c;
                    c = fgetc(stream);
                }
                while (c != EOF && tlen < lim && isdigit((unsigned char)c)) {
                    tmp[tlen++] = (char)c;
                    c = fgetc(stream);
                }
            }
            if (c != EOF)
                ungetc(c, stream);

            tmp[tlen] = '\0';
            if (tlen == 0)
                return assigned;

            if (!suppress) {
                if (len == LEN_BIG_L)
                    *va_arg(ap, long double *) = strtold(tmp, NULL);
                else if (len == LEN_L)
                    *va_arg(ap, double *) = strtod(tmp, NULL);
                else
                    *va_arg(ap, float *) = strtof(tmp, NULL);

                assigned++;
            }
            break;
        }
        case 'c': {
            int lim = width > 0 ? width : 1;
            int c = fgetc(stream);
            if (c == EOF)
                return assigned;
            if (!suppress) {
                char* dst = va_arg(ap, char *);
                int i = 0;
                do {
                    dst[i++] = (char)c;
                    if (i >= lim)
                        break;

                    c = fgetc(stream);
                } while (c != EOF);

                assigned++;
            } else {
                int i = 1;
                while (i < lim && (c = fgetc(stream)) != EOF)
                    i++;
            }
            break;
        }
        case 's': {
            int lim = width > 0 ? width : -1;
            int c = fgetc(stream);
            if (c == EOF)
                return assigned;
            if (!suppress) {
                char* dst = va_arg(ap, char *);
                int i = 0;
                while (c != EOF && !isspace((unsigned char)c) && (lim < 0 || i < lim)) {
                    dst[i++] = (char)c;
                    c = fgetc(stream);
                }
                if (c != EOF)
                    ungetc(c, stream);

                if (i == 0)
                    return assigned;

                dst[i] = '\0';
                assigned++;
            } else {
                int i = 0;
                while (c != EOF && !isspace((unsigned char)c) && (lim < 0 || i < lim)) {
                    c = fgetc(stream);
                    i++;
                }
                if (c != EOF)
                    ungetc(c, stream);
            }
            break;
        }
        case '[': {
            int negated = 0;
            if (*f == '^') {
                negated = 1;
                f++;
            }
            const char* set = f;
            if (*f == ']')
                f++;

            while (*f && *f != ']')
                f++;

            if (*f == ']')
                f++;

            int lim = width > 0 ? width : -1;
            int c = fgetc(stream);
            if (c == EOF)
                return assigned;

            if (!suppress) {
                char* dst = va_arg(ap, char *);
                int i = 0;
                while (c != EOF && (lim < 0 || i < lim) &&
                    match_scanset(set, (unsigned char)c, negated)) {
                    dst[i++] = (char)c;
                    c = fgetc(stream);
                }
                if (c != EOF)
                    ungetc(c, stream);

                if (i == 0)
                    return assigned;

                dst[i] = '\0';
                assigned++;
            } else {
                int i = 0;
                while (c != EOF && (lim < 0 || i < lim) &&
                    match_scanset(set, (unsigned char)c, negated)) {
                    c = fgetc(stream);
                    i++;
                }
                if (c != EOF)
                    ungetc(c, stream);
            }
            break;
        }
        case 'n':
            if (!suppress) {
                long pos = ftell(stream) - start_pos;
                if (len == LEN_LL)
                    *va_arg(ap, long long *) = (long long)pos;
                else if (len == LEN_L)
                    *va_arg(ap, long *) = pos;
                else if (len == LEN_H)
                    *va_arg(ap, short *) = (short)pos;
                else if (len == LEN_HH)
                    *va_arg(ap, signed char *) = (signed char)pos;
                else
                    *va_arg(ap, int *) = (int)pos;
            }
            break;
        default:
            return assigned;
        }
    }

    return assigned;
}

int vscanf(const char* fmt, va_list ap) {
    return vfscanf(stdin, fmt, ap);
}

int sscanf(const char* str, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vsscanf(str, fmt, ap);
    va_end(ap);
    return n;
}

int fscanf(FILE* stream, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vfscanf(stream, fmt, ap);
    va_end(ap);
    return n;
}

int scanf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vscanf(fmt, ap);
    va_end(ap);
    return n;
}
