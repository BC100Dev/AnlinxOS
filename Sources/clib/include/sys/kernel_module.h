#ifndef SYS_KERNEL_MODULE_H
#define SYS_KERNEL_MODULE_H

#include <stddef.h>
#include <linux/module.h>

#ifdef __cplusplus
extern "C" {
#endif

int init_module(void* module_image, unsigned long len, const char* param_values);
int finit_module(int fd, const char* param_values, int flags);
int delete_module(const char* name, unsigned int flags);

#ifdef __cplusplus
}
#endif

#endif
