#include <ALX64/GzipIO.hpp>
#include <zlib.h>

#include <cstring>
#include <fstream>
#include <stdexcept>

std::vector<uint8_t> decompressStream(z_stream &stream) {
    std::vector<uint8_t> result;
    uint8_t buffer[4096];
    int ret;

    do {
        stream.next_out = buffer;
        stream.avail_out = sizeof(buffer);

        ret = inflate(&stream, Z_NO_FLUSH);
        if (ret != Z_OK && ret != Z_STREAM_END)
            throw std::runtime_error("Decompression failed, returned zlib_error " + std::to_string(ret));

        size_t have = sizeof(buffer) - stream.avail_out;
        result.insert(result.end(), buffer, buffer + have);
    } while (ret != Z_STREAM_END);

    return result;
}

std::vector<uint8_t> DecompressVec(const std::filesystem::path &file) {
    gzFile gz = gzopen(file.c_str(), "rb");
    if (!gz)
        throw std::runtime_error("Failed to open gzip file \"" + file.string() + "\"");

    std::vector<uint8_t> result;
    uint8_t buffer[4096];
    int bytes_read;

    while ((bytes_read = gzread(gz, buffer, sizeof(buffer))) > 0)
        result.insert(result.end(), buffer, buffer + bytes_read);

    if (bytes_read < 0) {
        gzclose(gz);
        throw std::runtime_error("Decompression failed while reading file \"" + file.string() + "\"");
    }

    gzclose(gz);
    return result;
}

std::vector<uint8_t> DecompressVec(const uint8_t *data, size_t len) {
    z_stream stream{};
    stream.next_in = const_cast<uint8_t *>(data);
    stream.avail_in = len;

    if (inflateInit2(&stream, 16 + MAX_WBITS) != Z_OK)
        throw std::runtime_error("Failed to initialize zlib");

    std::vector<uint8_t> result;

    try {
        result = decompressStream(stream);
    } catch (...) {
    }

    inflateEnd(&stream);
    return result;
}

std::string DecompressStr(const std::filesystem::path &file) {
    auto data = DecompressVec(file);
    return {data.begin(), data.end()};
}

std::string DecompressStr(const uint8_t *data, size_t len) {
    auto result = DecompressVec(data, len);
    return {result.begin(), result.end()};
}
