#ifndef STDLIB_H
#define STDLIB_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void* malloc(size_t size);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);
void* reallocarray(void* ptr, size_t nmemb, size_t size);
void free(void* ptr);
void* aligned_alloc(size_t alignment, size_t size);
int posix_memalign(void** memptr, size_t alignment, size_t size);

void abort(void);
void exit(int status);
void _Exit(int status);
int atexit(void (*func)(void));
int at_quick_exit(void (*func)(void));
void quick_exit(int status);

char* getenv(const char* name);

int abs(int n);
long labs(long n);
long long llabs(long long n);

typedef struct {
    int quot;
    int rem;
} div_t;

typedef struct {
    long quot;
    long rem;
} ldiv_t;

typedef struct {
    long long quot;
    long long rem;
} lldiv_t;

div_t div(int numer, int denom);
ldiv_t ldiv(long numer, long denom);
lldiv_t lldiv(long long numer, long long denom);

double atof(const char* s);
int atoi(const char* s);
long atol(const char* s);
long long atoll(const char* s);
long strtol(const char* s, char** endptr, int base);
long long strtoll(const char* s, char** endptr, int base);
unsigned long strtoul(const char* s, char** endptr, int base);
unsigned long long strtoull(const char* s, char** endptr, int base);
double strtod(const char* s, char** endptr);
float strtof(const char* s, char** endptr);
long double strtold(const char* s, char** endptr);

void* bsearch(const void* key, const void* base, size_t nmemb, size_t size,
              int (*compar)(const void*, const void*));
void qsort(void* base, size_t nmemb, size_t size,
           int (*compar)(const void*, const void*));

int mblen(const char* s, size_t n);
int mbtowc(int* pwc, const char* s, size_t n);
int wctomb(char* s, int wchar);
size_t mbstowcs(int* pwcs, const char* s, size_t n);
size_t wcstombs(char* s, const int* pwcs, size_t n);

int system(const char* command);
char* mktemp(char* tmpl);
int mkstemp(char* tmpl);

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define RAND_MAX 0x7fffffff
#define MB_CUR_MAX 4

int rand(void);
void srand(unsigned int seed);

#ifdef __cplusplus
}
#endif

#endif //STDLIB_H
