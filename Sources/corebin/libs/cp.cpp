#include "FuncHeader.hpp"

#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>

int cp_func(int argc, char** argv, char** env) {
    if (argc == 3) {
        std::ifstream in(argv[1]);
        if (!in.is_open()) {
            std::cerr << "file input open failed, " << std::strerror(errno) << std::endl;
            return 1;
        }

        std::ofstream out(argv[2]);
        if (!out.is_open()) {
            std::cerr << "file output open failed, " << std::strerror(errno) << std::endl;
            return 1;
        }

        out << in.rdbuf();
        out.close();
        in.close();
    } else {
        std::cerr << "provide input and output bruh" << std::endl;
        return 1;
    }

    return 0;
}
