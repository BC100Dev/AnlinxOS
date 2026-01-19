#ifndef ALX64_PROCESS_HPP
#define ALX64_PROCESS_HPP

#include <filesystem>
#include <string>
#include <vector>

struct ProcessInfo {
    int id;
    int uid;
    int tty;
    int threadCount;
};

#endif //ALX64_PROCESS_HPP
