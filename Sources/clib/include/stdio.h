#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EOF (-1)
#define BUFSIZ 8192
#define FOPEN_MAX 16
#define FILENAME_MAX 4096
#define L_tmpnam 20

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2

#ifndef STDIN_FILENO
#define STDIN_FILENO  0
#endif

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

typedef struct _FILE FILE;
typedef long fpos_t;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

#define stdin stdin
#define stdout stdout
#define stderr stderr

FILE* fopen(const char* path, const char* mode);
FILE* fdopen(int fd, const char* mode);
FILE* freopen(const char* path, const char* mode, FILE* stream);
int fclose(FILE* stream);
int fflush(FILE* stream);
void setbuf(FILE* stream, char* buf);
int setvbuf(FILE* stream, char* buf, int mode, size_t size);

int printf(const char* fmt, ...);
int fprintf(FILE* stream, const char* fmt, ...);
int sprintf(char* str, const char* fmt, ...);
int snprintf(char* str, size_t size, const char* fmt, ...);
int dprintf(int fd, const char* fmt, ...);
int vprintf(const char* fmt, va_list ap);
int vfprintf(FILE* stream, const char* fmt, va_list ap);
int vsprintf(char* str, const char* fmt, va_list ap);
int vsnprintf(char* str, size_t size, const char* fmt, va_list ap);
int vdprintf(int fd, const char* fmt, va_list ap);

int scanf(const char* fmt, ...);
int fscanf(FILE* stream, const char* fmt, ...);
int sscanf(const char* str, const char* fmt, ...);
int vscanf(const char* fmt, va_list ap);
int vfscanf(FILE* stream, const char* fmt, va_list ap);
int vsscanf(const char* str, const char* fmt, va_list ap);

int fgetc(FILE* stream);
int fputc(int c, FILE* stream);
int getc(FILE* stream);
int putc(int c, FILE* stream);
int getchar(void);
int putchar(int c);
int ungetc(int c, FILE* stream);

char* fgets(char* s, int n, FILE* stream);
int fputs(const char* s, FILE* stream);
char* gets(char* s);
int puts(const char* s);

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);

int fseek(FILE* stream, long offset, int whence);
long ftell(FILE* stream);
int fgetpos(FILE* stream, fpos_t* pos);
int fsetpos(FILE* stream, const fpos_t* pos);
void rewind(FILE* stream);

int feof(FILE* stream);
int ferror(FILE* stream);
void clearerr(FILE* stream);
void perror(const char* s);

int remove(const char* path);
int rename(const char* old, const char*new);
FILE* tmpfile(void);
char* tmpnam(char* s);

int fileno(FILE* stream);

#ifdef __cplusplus
}
#endif

#endif /* STDIO_H */
