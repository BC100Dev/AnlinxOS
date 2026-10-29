#ifndef SYS_PRCTL_H
#define SYS_PRCTL_H

#include <linux/prctl.h>

#ifdef __cplusplus
extern "C" {
#endif

int prctl(int option, unsigned long arg2, unsigned long arg3,
          unsigned long arg4, unsigned long arg5);

#ifdef __cplusplus
}
#endif

#endif
