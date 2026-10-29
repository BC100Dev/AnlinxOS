#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "file.h"
#include "fmt.h"

static size_t fmt_uint(char* buf, size_t size, unsigned long long val,
                       int base, int upper, int alt, int zero,
                       int width, int left, int prec, int has_prec) {
    static const char lower[] = "0123456789abcdef";
    static const char upper_[] = "0123456789ABCDEF";
    const char* digits = upper ? upper_ : lower;
    char tmp[64];
    int tlen = 0;

    if (val == 0 && has_prec && prec == 0) {
        tmp[tlen++] = '\0';
    } else {
        unsigned long long v = val;
        do {
            tmp[tlen++] = digits[v % (unsigned)base];
            v /= (unsigned)base;
        } while (v);
    }

    char prefix[3];
    int plen = 0;
    if (alt && base == 16 && val != 0) {
        prefix[plen++] = '0';
        prefix[plen++] = upper ? 'X' : 'x';
    } else if (alt && base == 8 && (tlen == 0 || tmp[tlen - 1] != '0')) {
        prefix[plen++] = '0';
    }

    int digits_needed = tlen;
    if (has_prec && prec > digits_needed)
        digits_needed = prec;

    int total = plen + digits_needed;
    int pad = width > total ? width - total : 0;

    size_t out = 0;
#define PUTC(c) do { if (out < size) buf[out] = (c); out++; } while(0)
    if (!left && !zero) {
        while (pad--)
            PUTC(' ');
    }

    for (int i = 0; i < plen; i++)
        PUTC(prefix[i]);

    if (!left && zero) {
        while (pad--)
            PUTC('0');
    }

    int zpad = digits_needed - tlen;
    while (zpad-- > 0)
        PUTC('0');

    for (int i = tlen - 1; i >= 0; i--)
        PUTC(tmp[i]);

    if (left) {
        while (pad--)
            PUTC(' ');
    }
#undef PUTC
    return out;
}

static size_t fmt_sint(char* buf, size_t size, long long val,
                       int plus, int space, int zero,
                       int width, int left, int prec, int has_prec) {
    char sign = 0;
    unsigned long long uval;
    if (val < 0) {
        sign = '-';
        uval = (unsigned long long)-val;
    } else if (plus) {
        sign = '+';
        uval = (unsigned long long)val;
    } else if (space) {
        sign = ' ';
        uval = (unsigned long long)val;
    } else {
        uval = (unsigned long long)val;
    }

    char tmp[64];
    int tlen = 0;
    if (uval == 0 && has_prec && prec == 0) {
        tmp[tlen++] = '\0';
    } else {
        unsigned long long v = uval;
        do {
            tmp[tlen++] = '0' + (int)(v % 10);
            v /= 10;
        } while (v);
    }

    int digits_needed = tlen;
    if (has_prec && prec > digits_needed)
        digits_needed = prec;

    int total = (sign ? 1 : 0) + digits_needed;
    int pad = width > total ? width - total : 0;

    size_t out = 0;
#define PUTC(c) do { if (out < size) buf[out] = (c); out++; } while(0)
    if (!left && !zero) {
        while (pad--)
            PUTC(' ');
    }

    if (sign)
        PUTC(sign);

    if (!left && zero) {
        while (pad--)
            PUTC('0');
    }

    int zpad = digits_needed - tlen;
    while (zpad-- > 0)
        PUTC('0');

    for (int i = tlen - 1; i >= 0; i--)
        PUTC(tmp[i]);

    if (left) {
        while (pad--)
            PUTC(' ');
    }
#undef PUTC
    return out;
}

static size_t fmt_str(char* buf, size_t size, const char* s,
                      int width, int left, int prec, int has_prec) {
    if (!s)
        s = "(null)";

    size_t slen = strlen(s);

    if (has_prec && (size_t)prec < slen)
        slen = (size_t)prec;

    int pad = width > (int)slen ? width - (int)slen : 0;
    size_t out = 0;
#define PUTC(c) do { if (out < size) buf[out] = (c); out++; } while(0)
    if (!left) {
        while (pad--)
            PUTC(' ');
    }

    for (size_t i = 0; i < slen; i++)
        PUTC(s[i]);

    if (left) {
        while (pad--)
            PUTC(' ');
    }
#undef PUTC
    return out;
}

static size_t fmt_char(char* buf, size_t size, int c, int width, int left) {
    int pad = width > 1 ? width - 1 : 0;
    size_t out = 0;
#define PUTC(ch) do { if (out < size) buf[out] = (ch); out++; } while(0)
    if (!left) {
        while (pad--)
            PUTC(' ');
    }

    PUTC((char)c);
    if (left) {
        while (pad--)
            PUTC(' ');
    }
#undef PUTC
    return out;
}

static size_t fmt_double(char* buf, size_t size, double val,
                         int plus, int space, int zero,
                         int width, int left, int prec, int has_prec,
                         char spec) {
    if (!has_prec) prec = 6;

    char sign = 0;
    if (val < 0.0) {
        sign = '-';
        val = -val;
    } else if (plus)
        sign = '+';
    else if (space)
        sign = ' ';

    char tmp[64];
    int tlen = 0;

    if (spec == 'e') {
        int exp = 0;
        if (val != 0.0) {
            while (val >= 10.0) {
                val /= 10.0;
                exp++;
            }
            while (val < 1.0) {
                val *= 10.0;
                exp--;
            }
        }

        unsigned long long ipart = (unsigned long long)val;
        double fpart = val - (double)ipart;
        char ibuf[32];
        int ilen = 0;
        unsigned long long iv = ipart;
        do {
            ibuf[ilen++] = '0' + (int)(iv % 10);
            iv /= 10;
        } while (iv);
        for (int i = ilen - 1; i >= 0; i--)
            tmp[tlen++] = ibuf[i];

        if (prec > 0) {
            tmp[tlen++] = '.';
            for (int i = 0; i < prec; i++) {
                fpart *= 10.0;
                tmp[tlen++] = '0' + (int)fpart;
                fpart -= (int)fpart;
            }
        }

        tmp[tlen++] = 'e';
        tmp[tlen++] = exp < 0 ? '-' : '+';
        if (exp < 0)
            exp = -exp;

        if (exp < 10)
            tmp[tlen++] = '0';

        char ebuf[16];
        int elen = 0;
        if (exp == 0) {
            ebuf[elen++] = '0';
        } else {
            int e = exp;
            while (e) {
                ebuf[elen++] = '0' + e % 10;
                e /= 10;
            }
        }

        for (int i = elen - 1; i >= 0; i--)
            tmp[tlen++] = ebuf[i];
    } else {
        unsigned long long ipart = (unsigned long long)val;
        double fpart = val - (double)ipart;
        char ibuf[32];
        int ilen = 0;
        unsigned long long iv = ipart;
        do {
            ibuf[ilen++] = '0' + (int)(iv % 10);
            iv /= 10;
        } while (iv);

        for (int i = ilen - 1; i >= 0; i--)
            tmp[tlen++] = ibuf[i];

        if (prec > 0) {
            tmp[tlen++] = '.';
            for (int i = 0; i < prec; i++) {
                fpart *= 10.0;
                tmp[tlen++] = '0' + (int)fpart;
                fpart -= (int)fpart;
            }
        }
    }

    int total = tlen + (sign ? 1 : 0);
    int pad = width > total ? width - total : 0;
    size_t out = 0;
#define PUTC(c) do { if (out < size) buf[out] = (c); out++; } while(0)
    if (!left && !zero) {
        while (pad--)
            PUTC(' ');
    }

    if (sign)
        PUTC(sign);

    if (!left && zero) {
        while (pad--)
            PUTC('0');
    }

    for (int i = 0; i < tlen; i++)
        PUTC(tmp[i]);

    if (left) {
        while (pad--)
            PUTC(' ');
    }
#undef PUTC
    return out;
}

int vsnprintf(char* str, size_t size, const char* fmt, va_list ap) {
    size_t out = 0;
    size_t lim = size > 0 ? size - 1 : (size_t)-1;

#define PUTC(c) do { if (out < lim) str[out] = (c); out++; } while(0)

    while (*fmt) {
        if (*fmt != '%') {
            PUTC(*fmt++);
            continue;
        }
        fmt++;
        if (*fmt == '%') {
            PUTC('%');
            fmt++;
            continue;
        }

        fmtflags_t f;
        parse_fmtflags(&fmt, &f);

        if (*fmt == '*') {
            f.width = va_arg(ap, int);
            if (f.width < 0) {
                f.left = 1;
                f.width = -f.width;
            }
            fmt++;
        } else
            f.width = parse_width(&fmt);

        if (*fmt == '.') {
            fmt++;
            f.has_prec = 1;
            f.prec = 0;
            if (*fmt == '*') {
                f.prec = va_arg(ap, int);
                if (f.prec < 0) {
                    f.has_prec = 0;
                    f.prec = -1;
                }
                fmt++;
            } else {
                f.prec = parse_width(&fmt);
            }
        }

        f.len = parse_lenmod(&fmt);

        char spec = *fmt++;
        char tbuf[512];
        size_t tlen = 0;

        switch (spec) {
        case 'd':
        case 'i': {
            long long val;
            if (f.len == LEN_LL)
                val = va_arg(ap, long long);
            else if (f.len == LEN_L)
                val = va_arg(ap, long);
            else if (f.len == LEN_J)
                val = va_arg(ap, intmax_t);
            else if (f.len == LEN_Z)
                val = va_arg(ap, ssize_t);
            else if (f.len == LEN_T)
                val = va_arg(ap, ptrdiff_t);
            else if (f.len == LEN_H)
                val = (short)va_arg(ap, int);
            else if (f.len == LEN_HH)
                val = (signed char)va_arg(ap, int);
            else
                val = va_arg(ap, int);

            tlen = fmt_sint(tbuf, sizeof(tbuf), val,
                            f.plus, f.space, f.zero,
                            f.width, f.left, f.prec, f.has_prec);
            break;
        }
        case 'u':
        case 'o':
        case 'x':
        case 'X': {
            unsigned long long val;
            if (f.len == LEN_LL)
                val = va_arg(ap, unsigned long long);
            else if (f.len == LEN_L)
                val = va_arg(ap, unsigned long);
            else if (f.len == LEN_J)
                val = va_arg(ap, uintmax_t);
            else if (f.len == LEN_Z)
                val = va_arg(ap, size_t);
            else if (f.len == LEN_T)
                val = (ptrdiff_t)va_arg(ap, ptrdiff_t);
            else if (f.len == LEN_H)
                val = (unsigned short)va_arg(ap, int);
            else if (f.len == LEN_HH)
                val = (unsigned char)va_arg(ap, int);
            else
                val = va_arg(ap, unsigned int);

            int base = spec == 'o' ? 8 : spec == 'u' ? 10 : 16;
            int upper = spec == 'X';
            tlen = fmt_uint(tbuf, sizeof(tbuf), val, base, upper,
                            f.alt, f.zero, f.width, f.left,
                            f.prec, f.has_prec);
            break;
        }
        case 'c':
            tlen = fmt_char(tbuf, sizeof(tbuf),
                            va_arg(ap, int), f.width, f.left);
            break;
        case 's':
            tlen = fmt_str(tbuf, sizeof(tbuf),
                           va_arg(ap, const char *),
                           f.width, f.left, f.prec, f.has_prec);
            break;
        case 'p': {
            uintptr_t val = (uintptr_t)va_arg(ap, void *);
            tbuf[0] = '0';
            tbuf[1] = 'x';
            size_t rest = fmt_uint(tbuf + 2, sizeof(tbuf) - 2,
                                   val, 16, 0, 0, 0, 0, 0, 0, 0);
            tlen = 2 + rest;
            break;
        }
        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G': {
            double val = f.len == LEN_BIG_L ? (double)va_arg(ap, long double) : va_arg(ap, double);
            char s = spec | 0x20;
            if (s == 'g')
                s = 'f';

            tlen = fmt_double(tbuf, sizeof(tbuf), val,
                              f.plus, f.space, f.zero,
                              f.width, f.left,
                              f.prec, f.has_prec, s);
            break;
        }
        case 'n': {
            if (f.len == LEN_LL)
                *va_arg(ap, long long *) = (long long)out;
            else if (f.len == LEN_L)
                *va_arg(ap, long *) = (long)out;
            else if (f.len == LEN_H)
                *va_arg(ap, short *) = (short)out;
            else if (f.len == LEN_HH)
                *va_arg(ap, signed char *) = (signed char)out;
            else
                *va_arg(ap, int *) = (int)out;

            tlen = 0;
            break;
        }
        default:
            tbuf[0] = '%';
            tbuf[1] = spec;
            tlen = 2;
            break;
        }

        for (size_t i = 0; i < tlen; i++) {
            if (out < lim) str[out] = tbuf[i];
            out++;
        }
    }

    if (size > 0)
        str[out < lim ? out : lim] = '\0';

    return (int)out;
#undef PUTC
}

int vsprintf(char* str, const char* fmt, va_list ap) {
    return vsnprintf(str, (size_t)-1, fmt, ap);
}

int vfprintf(FILE* stream, const char* fmt, va_list ap) {
    va_list ap2;
    va_copy(ap2, ap);
    int n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap2);
    if (n < 0) return n;
    char* buf = malloc((size_t)n + 1);
    if (!buf) return -1;
    va_copy(ap2, ap);
    vsnprintf(buf, (size_t)n + 1, fmt, ap2);
    va_end(ap2);
    fwrite(buf, 1, (size_t)n, stream);
    free(buf);
    return n;
}

int vprintf(const char* fmt, va_list ap) {
    return vfprintf(stdout, fmt, ap);
}

int vdprintf(int fd, const char* fmt, va_list ap) {
    va_list ap2;
    va_copy(ap2, ap);
    int n = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap2);
    if (n < 0)
        return n;

    char* buf = malloc((size_t)n + 1);
    if (!buf)
        return -1;

    va_copy(ap2, ap);
    vsnprintf(buf, (size_t)n + 1, fmt, ap2);
    va_end(ap2);

    write(fd, buf, (size_t)n);
    free(buf);
    return n;
}

int snprintf(char* str, size_t size, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(str, size, fmt, ap);
    va_end(ap);
    return n;
}

int sprintf(char* str, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vsprintf(str, fmt, ap);
    va_end(ap);
    return n;
}

int fprintf(FILE* stream, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vfprintf(stream, fmt, ap);
    va_end(ap);
    return n;
}

int printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vprintf(fmt, ap);
    va_end(ap);
    return n;
}

int dprintf(int fd, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = vdprintf(fd, fmt, ap);
    va_end(ap);
    return n;
}
