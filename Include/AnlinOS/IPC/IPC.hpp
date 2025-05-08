#ifndef ANLINOS_IPC_HPP
#define ANLINOS_IPC_HPP

#include <string>
#include <vector>

namespace AnlinOS::IPC {

    struct Server {
        int pid;
        std::string name;
    };

}

#endif //ANLINOS_IPC_HPP
