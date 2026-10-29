#ifndef STDIO_FMT_H
#define STDIO_FMT_H

#include <stdarg.h>

typedef enum {
    LEN_NONE,
    LEN_HH,
    LEN_H,
    LEN_L,
    LEN_LL,
    LEN_J,
    LEN_Z,
    LEN_T,
    LEN_BIG_L
} lenmod_t;

typedef struct {
    int left;
    int plus;
    int space;
    int zero;
    int alt;
    int width;
    int prec;
    int has_prec;
    lenmod_t len;
} fmtflags_t;

lenmod_t parse_lenmod(const char** fmt);
int parse_width(const char** fmt);
void parse_fmtflags(const char **fmt, fmtflags_t *f);

#endif /* STDIO_FMT_H */
