#include <ALX64/kmsg.h>

#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>

namespace fs = std::filesystem;

struct kmsg {
    int stream_fd;
};

kmsg_t* kmsg_open() {
    kmsg* ctx = new kmsg();
    if (!fs::exists("/dev/kmsg"))
        return nullptr;

    int fd = open("/dev/kmsg", O_RDWR | O_CLOEXEC);
    if (fd < 0)
        return nullptr;

    ctx->stream_fd = fd;
    return ctx;
}

bool kmsg_is_open(kmsg_t* ctx) {
    if (ctx == nullptr)
        return false;

    if (fcntl(ctx->stream_fd, F_GETFD) != -1 || errno != EBADF)
        return false;

    return true;
}

static void kmsg_write_impl(kmsg_t* ctx, const char* prefix, const char* msg) {
    std::string data = std::string(prefix) + std::string(msg);
    write(ctx->stream_fd, data.c_str(), data.size());
}

void kmsg_write(kmsg_t* ctx, int level, const char* msg) {
    if (!kmsg_is_open(ctx)) return;

    const char* prefix;

    switch (level) {
    case LOGLEVEL_EMERG:
        prefix = KERN_EMERG;
        break;
    case LOGLEVEL_ALERT:
        prefix = KERN_ALERT;
        break;
    case LOGLEVEL_CRIT:
        prefix = KERN_CRIT;
        break;
    case LOGLEVEL_ERR:
        prefix = KERN_ERR;
        break;
    case LOGLEVEL_WARNING:
        prefix = KERN_WARNING;
        break;
    case LOGLEVEL_NOTICE:
        prefix = KERN_NOTICE;
        break;
    case LOGLEVEL_INFO:
        prefix = KERN_INFO;
        break;
    case LOGLEVEL_DEBUG:
        prefix = KERN_DEBUG;
        break;
    default:
        prefix = KERN_DEFAULT;
        break;
    }

    kmsg_write_impl(ctx, prefix, msg);
}

char* kmsg_read(kmsg_t* ctx, int startOffset, int size) {
    if (!kmsg_is_open(ctx))
        return nullptr;

    if (startOffset != -1)
        lseek(ctx->stream_fd, startOffset, SEEK_SET);

    auto buf = static_cast<char*>(malloc(size));
    if (!buf)
        return nullptr;

    if (ssize_t ret = read(ctx->stream_fd, buf, size); ret < 0) {
        free(buf);
        return nullptr;
    }

    return buf;
}

void kmsg_close(kmsg_t* ctx) {
    if (kmsg_is_open(ctx)) {
        if (fcntl(ctx->stream_fd, F_GETFD) != -1 || errno != EBADF) {
            free(ctx);
            return;
        }
        close(ctx->stream_fd);
        free(ctx);
    }
}
