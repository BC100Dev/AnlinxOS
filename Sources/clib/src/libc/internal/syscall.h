#ifndef MDL_SYSCALL_H
#define MDL_SYSCALL_H

typedef long sysret_t;

#ifdef __cplusplus
extern "C" {
#endif

#define SET_ERRNO(r) ({ long _r = (r); if (_r < 0) { errno = (int)-_r; _r = -1; } _r; })

sysret_t syscall0(long n);
sysret_t syscall1(long n, long a1);
sysret_t syscall2(long n, long a1, long a2);
sysret_t syscall3(long n, long a1, long a2, long a3);
sysret_t syscall4(long n, long a1, long a2, long a3, long a4);
sysret_t syscall5(long n, long a1, long a2, long a3, long a4, long a5);
sysret_t syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6);

#ifdef __cplusplus
}
#endif

#define SYSCALL0(n)                  syscall0((long)(n))
#define SYSCALL1(n,a)                syscall1((long)(n),(long)(a))
#define SYSCALL2(n,a,b)              syscall2((long)(n),(long)(a),(long)(b))
#define SYSCALL3(n,a,b,c)            syscall3((long)(n),(long)(a),(long)(b),(long)(c))
#define SYSCALL4(n,a,b,c,d)          syscall4((long)(n),(long)(a),(long)(b),(long)(c),(long)(d))
#define SYSCALL5(n,a,b,c,d,e)        syscall5((long)(n),(long)(a),(long)(b),(long)(c),(long)(d),(long)(e))
#define SYSCALL6(n,a,b,c,d,e,f)      syscall6((long)(n),(long)(a),(long)(b),(long)(c),(long)(d),(long)(e),(long)(f))

#endif //MDL_SYSCALL_H
