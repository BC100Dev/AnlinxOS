#include <stdio.h>
#include <unistd.h>
#include "file.h"

int fgetc(FILE* stream) {
    if (!stream)
        return EOF;

    if (stream->ungot != UNGET_NONE) {
        int c = stream->ungot;
        stream->ungot = UNGET_NONE;
        return c;
    }

    if (stream->flags & FILE_FL_EOF)
        return EOF;

    unsigned char c;
    ssize_t r = read(stream->fd, &c, 1);
    if (r == 0) {
        stream->flags |= FILE_FL_EOF;
        return EOF;
    }

    if (r < 0) {
        stream->flags |= FILE_FL_ERR;
        return EOF;
    }

    return (int)c;
}

int fputc(int c, FILE* stream) {
    if (!stream)
        return EOF;

    unsigned char ch = (unsigned char)c;

    if (stream->flags & FILE_FL_NOBUF) {
        ssize_t r = write(stream->fd, &ch, 1);
        if (r < 0) {
            stream->flags |= FILE_FL_ERR;
            return EOF;
        }
        return (int)ch;
    }

    if (stream->buf && stream->buf_pos < stream->buf_size) {
        stream->buf[stream->buf_pos++] = ch;
        if (stream->flags & FILE_FL_LINEBUF && ch == '\n')
            fflush(stream);
        return (int)ch;
    }

    if (fflush(stream) == EOF)
        return EOF;

    if (stream->buf && stream->buf_pos < stream->buf_size) {
        stream->buf[stream->buf_pos++] = ch;
        if (stream->flags & FILE_FL_LINEBUF && ch == '\n')
            fflush(stream);
        return (int)ch;
    }

    ssize_t r = write(stream->fd, &ch, 1);
    if (r < 0) {
        stream->flags |= FILE_FL_ERR;
        return EOF;
    }

    return (int)ch;
}

int getc(FILE* stream) {
    return fgetc(stream);
}

int putc(int c, FILE* stream) {
    return fputc(c, stream);
}

int getchar(void) {
    return fgetc(stdin);
}

int putchar(int c) {
    return fputc(c, stdout);
}

int ungetc(int c, FILE* stream) {
    if (!stream || c == EOF)
        return EOF;

    if (stream->ungot != UNGET_NONE)
        return EOF;

    stream->ungot = c;
    stream->flags &= ~FILE_FL_EOF;
    return c;
}

char* fgets(char* s, int n, FILE* stream) {
    if (!s || n <= 0 || !stream)
        return NULL;

    int i = 0;
    while (i < n - 1) {
        int c = fgetc(stream);
        if (c == EOF) {
            if (i == 0)
                return NULL;
            break;
        }
        s[i++] = (char)c;
        if (c == '\n')
            break;
    }
    s[i] = '\0';
    return s;
}

int fputs(const char* s, FILE* stream) {
    if (!s || !stream)
        return EOF;

    while (*s) {
        if (fputc((unsigned char)*s, stream) == EOF)
            return EOF;
        s++;
    }
    return 0;
}

char* gets(char* s) {
    if (!s)
        return NULL;

    int i = 0;
    while (1) {
        int c = fgetc(stdin);
        if (c == EOF) {
            if (i == 0)
                return NULL;
            break;
        }
        if (c == '\n')
            break;
        s[i++] = (char)c;
    }

    s[i] = '\0';
    return s;
}

int puts(const char* s) {
    if (fputs(s, stdout) == EOF)
        return EOF;

    return fputc('\n', stdout) == EOF ? EOF : 0;
}
