#ifndef ALX64_GZIPIO_HPP
#define ALX64_GZIPIO_HPP

#include <cstdint>
#include <filesystem>
#include <vector>

namespace ALX64::GZ {

    std::vector<uint8_t> decompressGzip(const std::filesystem::path& file);
    std::vector<uint8_t> decompressGzip(const uint8_t* data, size_t len);

}

#endif //ALX64_GZIPIO_HPP
