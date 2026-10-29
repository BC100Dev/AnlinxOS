#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

static void write_str(const char* s) {
    write(STDERR_FILENO, s, strlen(s));
}

static void write_int(int n) {
    char buf[32];
    int i = 0;
    if (n == 0) {
        buf[i++] = '0';
    } else {
        while (n > 0) {
            buf[i++] = '0' + n % 10;
            n /= 10;
        }
    }
    char out[32];
    for (int k = 0; k < i; k++)
        out[k] = buf[i - 1 - k];

    write(STDERR_FILENO, out, (size_t)i);
}

void __assert_fail(const char* expr, const char* file, int line, const char* func) {
    write_str(file);
    write_str(":");
    write_int(line);
    write_str(": ");
    write_str(func);
    write_str(": Assertion `");
    write_str(expr);
    write_str("' failed.\n");
    abort();
}
