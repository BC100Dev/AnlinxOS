#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "file.h"

#include "../internal/syscall.h"
#include "../internal/syscall_nr.h"

static struct _FILE _stdin_file = {0, FILE_FL_READ, UNGET_NONE, NULL, 0, 0, 0};
static struct _FILE _stdout_file = {1, FILE_FL_WRITE, UNGET_NONE, NULL, 0, 0, 0};
static struct _FILE _stderr_file = {2, FILE_FL_WRITE | FILE_FL_NOBUF, UNGET_NONE, NULL, 0, 0, 0};

FILE* stdin = &_stdin_file;
FILE* stdout = &_stdout_file;
FILE* stderr = &_stderr_file;

static int parse_mode(const char* mode, int* flags) {
    int f = 0;
    switch (*mode) {
    case 'r': f = FILE_FL_READ;
        break;
    case 'w': f = FILE_FL_WRITE;
        break;
    case 'a': f = FILE_FL_WRITE | FILE_FL_APPEND;
        break;
    default: return -1;
    }
    mode++;
    while (*mode) {
        if (*mode == '+') f |= FILE_FL_READ | FILE_FL_WRITE;
        mode++;
    }
    *flags = f;
    return 0;
}

static int mode_to_oflags(int flags) {
    int oflags = 0;
    int rd = flags & FILE_FL_READ;
    int wr = flags & FILE_FL_WRITE;
    if (rd && wr) oflags = O_RDWR;
    else if (wr) oflags = O_WRONLY;
    else oflags = O_RDONLY;
    if (flags & FILE_FL_WRITE) oflags |= O_CREAT | O_TRUNC;
    if (flags & FILE_FL_APPEND) oflags = (oflags & ~O_TRUNC) | O_APPEND;
    return oflags;
}

FILE* fopen(const char* path, const char* mode) {
    int flags;
    if (parse_mode(mode, &flags) < 0)
        return NULL;

    int fd = open(path, mode_to_oflags(flags), 0666);
    if (fd < 0)
        return NULL;

    FILE* f = malloc(sizeof(struct _FILE));
    if (!f) {
        close(fd);
        return NULL;
    }

    f->fd = fd;
    f->flags = flags;
    f->ungot = UNGET_NONE;
    f->buf = NULL;
    f->buf_size = 0;
    f->buf_pos = 0;
    f->buf_len = 0;
    return f;
}

FILE* fdopen(int fd, const char* mode) {
    int flags;
    if (parse_mode(mode, &flags) < 0)
        return NULL;

    FILE* f = malloc(sizeof(struct _FILE));
    if (!f)
        return NULL;

    f->fd = fd;
    f->flags = flags;
    f->ungot = UNGET_NONE;
    f->buf = NULL;
    f->buf_size = 0;
    f->buf_pos = 0;
    f->buf_len = 0;
    return f;
}

FILE* freopen(const char* path, const char* mode, FILE* stream) {
    fclose(stream);
    return fopen(path, mode);
}

int fclose(FILE* stream) {
    if (!stream)
        return EOF;

    fflush(stream);
    int ret = close(stream->fd);
    if (stream->flags & FILE_FL_OWN_BUF && stream->buf)
        free(stream->buf);

    if (stream != stdin && stream != stdout && stream != stderr)
        free(stream);

    return ret < 0 ? EOF : 0;
}

int fflush(FILE* stream) {
    if (!stream)
        return 0;

    if (!(stream->flags & FILE_FL_WRITE))
        return 0;

    if (!stream->buf || stream->buf_pos == 0)
        return 0;

    size_t written = 0;
    while (written < stream->buf_pos) {
        ssize_t r = write(stream->fd, stream->buf + written, stream->buf_pos - written);
        if (r < 0) {
            stream->flags |= FILE_FL_ERR;
            return EOF;
        }

        written += (size_t)r;
    }

    stream->buf_pos = 0;
    return 0;
}

void setbuf(FILE* stream, char* buf) {
    setvbuf(stream, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
}

int setvbuf(FILE* stream, char* buf, int mode, size_t size) {
    if (!stream)
        return -1;

    fflush(stream);
    if (stream->flags & FILE_FL_OWN_BUF && stream->buf)
        free(stream->buf);

    stream->buf = NULL;
    stream->buf_size = 0;
    stream->buf_pos = 0;
    stream->buf_len = 0;
    stream->flags &= ~(FILE_FL_NOBUF | FILE_FL_LINEBUF | FILE_FL_OWN_BUF);

    switch (mode) {
    case _IONBF:
        stream->flags |= FILE_FL_NOBUF;
        return 0;
    case _IOLBF:
        stream->flags |= FILE_FL_LINEBUF;
        break;
    case _IOFBF:
        break;
    default:
        return -1;
    }

    if (!buf) {
        buf = malloc(size);
        if (!buf)
            return -1;
        stream->flags |= FILE_FL_OWN_BUF;
    }
    stream->buf = (uint8_t*)buf;
    stream->buf_size = size;
    return 0;
}

int fileno(FILE* stream) {
    if (!stream)
        return -1;

    return stream->fd;
}

int feof(FILE* stream) {
    return stream && (stream->flags & FILE_FL_EOF) ? 1 : 0;
}

int ferror(FILE* stream) {
    return stream && (stream->flags & FILE_FL_ERR) ? 1 : 0;
}

void clearerr(FILE* stream) {
    if (stream)
        stream->flags &= ~(FILE_FL_EOF | FILE_FL_ERR);
}

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    if (!stream || !ptr || size == 0 || nmemb == 0)
        return 0;

    size_t total = size * nmemb;
    size_t done = 0;
    unsigned char* dst = (unsigned char*)ptr;

    if (stream->ungot != UNGET_NONE) {
        dst[done++] = (unsigned char)stream->ungot;
        stream->ungot = UNGET_NONE;
    }

    while (done < total) {
        ssize_t r = read(stream->fd, dst + done, total - done);
        if (r == 0) {
            stream->flags |= FILE_FL_EOF;
            break;
        }
        if (r < 0) {
            stream->flags |= FILE_FL_ERR;
            break;
        }
        done += (size_t)r;
    }

    return done / size;
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) {
    if (!stream || !ptr || size == 0 || nmemb == 0)
        return 0;

    size_t total = size * nmemb;
    size_t done = 0;
    const unsigned char* src = (const unsigned char*)ptr;

    if (stream->flags & FILE_FL_NOBUF) {
        while (done < total) {
            ssize_t r = write(stream->fd, src + done, total - done);
            if (r < 0) {
                stream->flags |= FILE_FL_ERR;
                break;
            }
            done += (size_t)r;
        }
        return done / size;
    }

    while (done < total) {
        if (!stream->buf || stream->buf_pos >= stream->buf_size) {
            if (fflush(stream) == EOF)
                break;

            if (!stream->buf) {
                ssize_t r = write(stream->fd, src + done, total - done);
                if (r < 0) {
                    stream->flags |= FILE_FL_ERR;
                    break;
                }
                done += (size_t)r;
                continue;
            }
        }
        size_t space = stream->buf_size - stream->buf_pos;
        size_t chunk = total - done;
        if (chunk > space)
            chunk = space;

        unsigned char* b = stream->buf;
        const unsigned char* s = src + done;
        size_t n = chunk;
        while (n--)
            *b++ = *s++;

        stream->buf_pos += chunk;
        done += chunk;

        if (stream->flags & FILE_FL_LINEBUF) {
            const unsigned char* p = src + done - chunk;
            for (size_t i = 0; i < chunk; i++) {
                if (p[i] == '\n') {
                    fflush(stream);
                    break;
                }
            }
        }
    }

    return done / size;
}

int fseek(FILE* stream, long offset, int whence) {
    if (!stream)
        return -1;

    fflush(stream);
    stream->flags &= ~FILE_FL_EOF;
    stream->buf_pos = 0;
    stream->buf_len = 0;
    stream->ungot = UNGET_NONE;
    return lseek(stream->fd, (off_t)offset, whence) < 0 ? -1 : 0;
}

long ftell(FILE* stream) {
    if (!stream)
        return -1L;

    off_t r = lseek(stream->fd, 0, SEEK_CUR);
    return r < 0 ? -1L : (long)r;
}

int fgetpos(FILE* stream, fpos_t* pos) {
    long r = ftell(stream);
    if (r < 0)
        return -1;

    *pos = (fpos_t)r;
    return 0;
}

int fsetpos(FILE* stream, const fpos_t* pos) {
    return fseek(stream, (long)*pos, SEEK_SET);
}

void rewind(FILE* stream) {
    fseek(stream, 0, SEEK_SET);
    clearerr(stream);
}

void perror(const char* s) {
    if (s && *s) {
        fputs(s, stderr);
        fputs(": ", stderr);
    }
    fputs(strerror(ferror(stderr)), stderr);
    fputc('\n', stderr);
}

int remove(const char* path) {
    return unlink(path);
}

int rename(const char* old, const char* new) {
    return (int)SYSCALL2(__NR_rename, (long)old, (long)new);
}
