#ifndef ALX64_KMSG_H
#define ALX64_KMSG_H

// straight out of "linux/kern_levels.h"

#define KERN_SOH "\001"
#define KERN_SOH_ASCII '\001'

#define KERN_EMERG KERN_SOH "0"
#define KERN_ALERT KERN_SOH "1"
#define KERN_CRIT KERN_SOH "2"
#define KERN_ERR KERN_SOH "3"
#define KERN_WARNING KERN_SOH "4"
#define KERN_NOTICE	KERN_SOH "5"
#define KERN_INFO KERN_SOH "6"
#define KERN_DEBUG KERN_SOH "7"

#define KERN_DEFAULT ""

#define KERN_CONT KERN_SOH "c"

#define LOGLEVEL_SCHED (-2)
#define LOGLEVEL_DEFAULT (-1)
#define LOGLEVEL_EMERG 0
#define LOGLEVEL_ALERT 1
#define LOGLEVEL_CRIT 2
#define LOGLEVEL_ERR 3
#define LOGLEVEL_WARNING 4
#define LOGLEVEL_NOTICE 5
#define LOGLEVEL_INFO 6
#define LOGLEVEL_DEBUG 7

#ifdef __cplusplus
#include <stdbool.h>

extern "C" {
#endif

typedef struct kmsg kmsg_t;

kmsg_t* kmsg_open();

bool kmsg_is_open(kmsg_t* ctx);

void kmsg_write(kmsg_t* ctx, int level, const char* msg);

char* kmsg_read(kmsg_t* ctx, int startOffset, int size);

void kmsg_close(kmsg_t* ctx);

#ifdef __cplusplus
}
#endif

#endif //ALX64_KMSG_H
