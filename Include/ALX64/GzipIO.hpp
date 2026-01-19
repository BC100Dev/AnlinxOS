#ifndef ALX64_GZIPIO_HPP
#define ALX64_GZIPIO_HPP

#include <cstdint>
#include <filesystem>
#include <vector>

std::vector<uint8_t> DecompressVec(const std::filesystem::path &file);

std::vector<uint8_t> DecompressVec(const uint8_t *data, size_t len);

std::string DecompressStr(const std::filesystem::path &file);

std::string DecompressStr(const uint8_t *data, size_t len);

#endif //ALX64_GZIPIO_HPP
