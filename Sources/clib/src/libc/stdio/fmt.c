#include "fmt.h"

lenmod_t parse_lenmod(const char** fmt) {
    if ((*fmt)[0] == 'h' && (*fmt)[1] == 'h') {
        *fmt += 2;
        return LEN_HH;
    }
    if ((*fmt)[0] == 'h') {
        *fmt += 1;
        return LEN_H;
    }
    if ((*fmt)[0] == 'l' && (*fmt)[1] == 'l') {
        *fmt += 2;
        return LEN_LL;
    }
    if ((*fmt)[0] == 'l') {
        *fmt += 1;
        return LEN_L;
    }
    if ((*fmt)[0] == 'j') {
        *fmt += 1;
        return LEN_J;
    }
    if ((*fmt)[0] == 'z') {
        *fmt += 1;
        return LEN_Z;
    }
    if ((*fmt)[0] == 't') {
        *fmt += 1;
        return LEN_T;
    }
    if ((*fmt)[0] == 'L') {
        *fmt += 1;
        return LEN_BIG_L;
    }
    return LEN_NONE;
}

int parse_width(const char** fmt) {
    int width = 0;
    while (**fmt >= '0' && **fmt <= '9')
        width = width * 10 + (*(*fmt)++ - '0');

    return width;
}

void parse_fmtflags(const char** fmt, fmtflags_t* f) {
    f->left = 0;
    f->plus = 0;
    f->space = 0;
    f->zero = 0;
    f->alt = 0;
    f->width = 0;
    f->prec = -1;
    f->has_prec = 0;
    f->len = LEN_NONE;

    while (1) {
        if (**fmt == '-') {
            f->left = 1;
            (*fmt)++;
        } else if (**fmt == '+') {
            f->plus = 1;
            (*fmt)++;
        } else if (**fmt == ' ') {
            f->space = 1;
            (*fmt)++;
        } else if (**fmt == '0') {
            f->zero = 1;
            (*fmt)++;
        } else if (**fmt == '#') {
            f->alt = 1;
            (*fmt)++;
        } else
            break;
    }
}
