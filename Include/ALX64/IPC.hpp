#ifndef ALX64_IPC_HPP
#define ALX64_IPC_HPP

#include <string>

namespace ALX64 {

    struct IPCServer {
        int pid;
        std::string name;
    };

    struct IPCClient {};

}

#endif //ALX64_IPC_HPP
