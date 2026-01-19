#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cerrno>
#include <filesystem>

#include <ALX64/GzipIO.hpp>

namespace fs = std::filesystem;

int main() {
    if (!fs::exists("/proc/config.gz")) {
        std::cerr << "Error: /proc/config.gz not found" << std::endl;
        std::cerr << "Make sure that CONFIG_IKCONFIG and CONFIG_IKCONFIG_PROC is enabled during the kernel building process" << std::endl;
        return 1;
    }

    std::string cfg = DecompressStr("/proc/config.gz");
    size_t pos = cfg.find("CONFIG_SMP=");
    if (pos == std::string::npos) {
        std::cout << "CONFIG_SMP not found in /proc/config.gz" << std::endl;
        return 2;
    }

    size_t end = cfg.find('\n', pos);
    std::string line = cfg.substr(pos, (end == std::string::npos ? cfg.size() : end) - pos);
    std::cout << line << std::endl;
    return 0;
}