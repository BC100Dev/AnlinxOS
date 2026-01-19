#ifndef ALX64_IPC_HPP
#define ALX64_IPC_HPP

#include <string>

struct IPCServer {
    long long ident;
    int pid;
    std::string name;
};

struct IPCClient {
};

#endif //ALX64_IPC_HPP
