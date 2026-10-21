#ifndef ANLINXOS_KERNEL_CONFIG_H
#define ANLINXOS_KERNEL_CONFIG_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef struct kernel_config_t kernel_config;

typedef enum {
    KERNEL_CONFIG_DEFINED = 0,
    KERNEL_CONFIG_NOT_DEFINED = 1,
    KERNEL_CONFIG_NOT_FOUND = 2,
    KERNEL_CONFIG_ERROR = 3
} kernel_config_lookup;

kernel_config kernel_config__load();

kernel_config_lookup kernel_config__find(kernel_config* ctx, const char* key);

char* kernel_config__read_config(kernel_config* ctx, const char* key);

void kernel_config__close(kernel_config* ctx);

#endif //ANLINXOS_KERNEL_CONFIG_H
