#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cerrno>
#include <filesystem>

#include <miniz/miniz.h>

namespace fs = std::filesystem;

int main() {
    if (!fs::exists("/proc/config.gz")) {
        std::cerr << "Error: /proc/config.gz not found" << std::endl;
        std::cerr << "Make sure that CONFIG_IKCONFIG and CONFIG_IKCONFIG_PROC is enabled during the kernel building process" << std::endl;
        return 1;
    }

    std::ifstream in("/proc/config.gz", std::ios::binary);
    if (!in) {
        std::cerr << "Error: failed to open /proc/config.gz, error " << std::strerror(errno) << std::endl;
        return 1;
    }

    std::vector<unsigned char> gz_data((std::istreambuf_iterator<char>(in)),
                                       std::istreambuf_iterator<char>());
    in.close();

    if (gz_data.size() < 18) {
        std::cerr << "Error: /proc/config.gz too small or invalid" << std::endl;
        return 1;
    }

    unsigned char* comp_ptr = gz_data.data() + 10;
    size_t comp_size = gz_data.size() - 18;

    size_t decomp_size = 0;
    void* decomp_ptr = tinfl_decompress_mem_to_heap(comp_ptr, comp_size, &decomp_size, 0);
    if (!decomp_ptr) {
        std::cerr << "Error: decompression failed" << std::endl;
        return 1;
    }

    std::string cfg((char*)decomp_ptr, decomp_size);
    free(decomp_ptr);

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