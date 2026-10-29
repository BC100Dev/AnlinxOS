#ifndef SETJMP_H
#define SETJMP_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__x86_64__)
typedef unsigned long jmp_buf[8];
typedef unsigned long sigjmp_buf[9];
#elif defined(__aarch64__)
typedef unsigned long jmp_buf[22];
typedef unsigned long sigjmp_buf[23];
#else
#error "Unsupported architecture"
#endif

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int val);
int sigsetjmp(sigjmp_buf env, int savesigs);
void siglongjmp(sigjmp_buf env, int val);

#ifdef __cplusplus
}
#endif

#endif
