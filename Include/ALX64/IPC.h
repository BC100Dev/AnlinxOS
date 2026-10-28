#ifndef ALX64_IPC_H
#define ALX64_IPC_H

#include <unistd.h>

typedef struct {
    long long ident;
    pid_t pid;
    char* name;
    int max_uses;
} IPC_Server;

typedef struct {
} IPC_ClientConnection;

#endif //ALX64_IPC_H
