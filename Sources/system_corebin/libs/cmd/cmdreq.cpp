#include <iostream>

#include "list.hpp"

int cmdreq_func(int argc, char** argv, char** envp) {
    std::cout << "cmdreq invoked" << std::endl;
    return 0;
}