#ifndef TIME_H
#define TIME_H

#include <sys/types.h>

struct timespec {
    long tv_sec;
    long tv_nsec;
};

struct timeval {
    long tv_sec;
    long tv_usec;
};

#endif //TIME_H
