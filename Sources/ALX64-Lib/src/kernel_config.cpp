#include <ALX64/kernel_config.h>
#include <ALX64/errno.h>

#include <fstream>
#include <filesystem>
#include <format>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <sys/utsname.h>
#include <zlib.h>

namespace fs = std::filesystem;

#define CONFIG_FILE_PROC "/proc/config.gz"
#define CONFIG_FILE_BOOTDF "/boot/config-{}"
#define CONFIG_FILE_BOOTLD "/bootld/linux/config-{}"

using fn_inflateInit2 = int(*)(z_streamp, int, const char*, int);
using fn_inflate = int(*)(z_streamp, int);
using fn_inflateEnd = int(*)(z_streamp);

struct kernel_config_t {
    fs::path configFile;
    int configHandle;
    char* buffer;
    size_t bufferSize;
};

namespace KernelConfigHelper {
    static char* read_raw_file(const fs::path& path, size_t* outSize) {
        std::ifstream f(path, std::ios::binary);
        if (!f.is_open())
            return nullptr;

        f.seekg(0, std::ios::end);
        auto sz = f.tellg();
        f.seekg(0, std::ios::beg);

        if (sz <= 0)
            return nullptr;

        auto buf = static_cast<char*>(malloc(sz));
        if (!buf)
            return nullptr;

        if (!f.read(buf, sz)) {
            free(buf);
            return nullptr;
        }

        *outSize = static_cast<size_t>(sz);
        return buf;
    }

    static char* decompress_gz(const char* compressed, size_t compSize, size_t* outSize) {
        void* zlib = dlopen("libz.so.1", RTLD_LAZY);
        if (!zlib)
            return nullptr;

        auto inflateInit2_ = reinterpret_cast<fn_inflateInit2>(dlsym(zlib, "inflateInit2_"));
        auto inflate_ = reinterpret_cast<fn_inflate>(dlsym(zlib, "inflate"));
        auto inflateEnd_ = reinterpret_cast<fn_inflateEnd>(dlsym(zlib, "inflateEnd"));

        if (!inflateInit2_ || !inflate_ || !inflateEnd_) {
            dlclose(zlib);
            return nullptr;
        }

        z_stream zs{};
        zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressed));
        zs.avail_in = static_cast<uInt>(compSize);

        if (inflateInit2_(&zs, 16 + MAX_WBITS, ZLIB_VERSION, static_cast<int>(sizeof(z_stream))) != Z_OK) {
            dlclose(zlib);
            return nullptr;
        }

        size_t cap = compSize * 8;
        char* out = static_cast<char*>(malloc(cap));
        size_t used = 0;
        int zret = Z_OK;

        while (zret == Z_OK) {
            if (used >= cap) {
                cap *= 2;
                char* tmp = static_cast<char*>(realloc(out, cap));
                if (!tmp) {
                    free(out);
                    out = nullptr;
                    break;
                }
                out = tmp;
            }

            zs.next_out = reinterpret_cast<Bytef*>(out + used);
            zs.avail_out = static_cast<uInt>(cap - used);

            zret = inflate_(&zs, Z_NO_FLUSH);
            used = cap - zs.avail_out;
        }

        inflateEnd_(&zs);
        dlclose(zlib);

        if (zret != Z_STREAM_END || !out) {
            free(out);
            return nullptr;
        }

        *outSize = used;
        return out;
    }

    static char* read_plain_config(const fs::path& path, size_t* outSize) {
        return read_raw_file(path, outSize);
    }
}

kernel_config kernel_config__load() {
    kernel_config ctx{};
    ctx.configHandle = -1;
    ctx.buffer = nullptr;
    ctx.bufferSize = 0;

    if (fs::exists(CONFIG_FILE_PROC)) {
        size_t rawSize = 0;
        char* raw = KernelConfigHelper::read_raw_file(CONFIG_FILE_PROC, &rawSize);
        if (raw) {
            size_t decompSize = 0;
            char* decompData = KernelConfigHelper::decompress_gz(raw, rawSize, &decompSize);
            free(raw);

            if (decompData) {
                ctx.configFile = CONFIG_FILE_PROC;
                ctx.buffer = decompData;
                ctx.bufferSize = decompSize;
                return ctx;
            }
        }
    }

    utsname uts{};
    if (uname(&uts) == 0) {
        const std::string release = uts.release;
        const fs::path candidates[] = {
            fs::path(std::format(CONFIG_FILE_BOOTDF, release)),
            fs::path(std::format(CONFIG_FILE_BOOTLD, release))
        };

        for (const auto& candidate : candidates) {
            if (!fs::exists(candidate))
                continue;

            size_t sz = 0;
            if (char* data = KernelConfigHelper::read_plain_config(candidate, &sz); data) {
                ctx.configFile = candidate;
                ctx.buffer = data;
                ctx.bufferSize = sz;
                return ctx;
            }
        }
    }

    return ctx;
}

kernel_config_lookup kernel_config__find(kernel_config* ctx, const char* key) {
    if (!ctx || !ctx->buffer || !key) {
        errno = EINVAL;
        return KERNEL_CONFIG_ERROR;
    }

    const std::string notSetPattern = std::format("# {} is not set", key);
    const std::string keyEquals = std::format("{}=", key);

    const char* buf = ctx->buffer;
    const char* end = buf + ctx->bufferSize;

    const char* p = buf;
    while (p < end) {
        const char* nl = static_cast<const char*>(memchr(p, '\n', end - p));
        size_t lineLen = nl ? static_cast<size_t>(nl - p) : static_cast<size_t>(end - p);

        if (lineLen >= notSetPattern.size() && memcmp(p, notSetPattern.c_str(), notSetPattern.size()) == 0)
            return KERNEL_CONFIG_NOT_DEFINED;

        if (lineLen >= keyEquals.size() && memcmp(p, keyEquals.c_str(), keyEquals.size()) == 0)
            return KERNEL_CONFIG_DEFINED;

        p = nl ? nl + 1 : end;
    }

    return KERNEL_CONFIG_NOT_FOUND;
}

char* kernel_config__read_config(kernel_config* ctx, const char* key) {
    if (!ctx || !ctx->buffer || !key) {
        errno = EINVAL;
        return nullptr;
    }

    kernel_config_lookup lku = kernel_config__find(ctx, key);
    if (lku == KERNEL_CONFIG_ERROR)
        return nullptr;

    if (lku != KERNEL_CONFIG_DEFINED) {
        errno = ENODATA;
        return nullptr;
    }

    const std::string keyEquals = std::format("{}=", key);
    const char* buf = ctx->buffer;
    const char* end = buf + ctx->bufferSize;
    const char* p = buf;

    while (p < end) {
        auto nl = static_cast<const char*>(memchr(p, '\n', end - p));

        if (size_t lineLen = nl ? static_cast<size_t>(nl - p) : static_cast<size_t>(end - p);
            lineLen >= keyEquals.size() && memcmp(p, keyEquals.c_str(), keyEquals.size()) == 0) {
            const char* valStart = p + keyEquals.size();
            size_t valLen = lineLen - keyEquals.size();

            char* val = static_cast<char*>(malloc(valLen + 1));
            if (!val)
                return nullptr;

            memcpy(val, valStart, valLen);
            val[valLen] = '\0';
            return val;
        }

        p = nl ? nl + 1 : end;
    }

    return nullptr;
}

void kernel_config__close(kernel_config* ctx) {
    if (!ctx)
        return;

    free(ctx->buffer);
    ctx->buffer = nullptr;
    ctx->bufferSize = 0;
    ctx->configHandle = -1;
    ctx->configFile.clear();
}
