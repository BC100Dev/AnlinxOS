#include <ALX64/ffstream.hpp>

#include <streambuf>
#include <vector>
#include <unistd.h>
#include <cerrno>

namespace {
    class FdStreamBuf : public std::streambuf {
    public:
        explicit FdStreamBuf(int fd) : m_fd(fd), m_readBuf(8192), m_writeBuf(8192) {
            setp(m_writeBuf.data(), m_writeBuf.data() + m_writeBuf.size());
            setg(m_readBuf.data(), m_readBuf.data(), m_readBuf.data());
        }

        ~FdStreamBuf() override {
            flushWrite();
        }

        FdStreamBuf(const FdStreamBuf&) = delete;
        FdStreamBuf& operator=(const FdStreamBuf&) = delete;

        int fd() const {
            return m_fd;
        }

    protected:
        int_type overflow(int_type c = traits_type::eof()) override {
            if (!flushWrite())
                return traits_type::eof();

            if (!traits_type::eq_int_type(c, traits_type::eof())) {
                *pptr() = traits_type::to_char_type(c);
                pbump(1);
            }

            return traits_type::not_eof(c);
        }

        std::streamsize xsputn(const char* s, std::streamsize n) override {
            const std::ptrdiff_t space = epptr() - pptr();
            if (n <= space) {
                std::copy(s, s + n, pptr());
                pbump(static_cast<int>(n));
                return n;
            }

            if (!flushWrite())
                return 0;

            std::streamsize remaining = n;
            const char* ptr = s;

            while (remaining > 0) {
                const ssize_t written = ::write(m_fd, ptr, static_cast<std::size_t>(remaining));
                if (written < 0) {
                    if (errno == EINTR) continue;
                    return n - remaining;
                }
                ptr += written;
                remaining -= written;
            }

            return n;
        }

        int sync() override {
            return flushWrite() ? 0 : -1;
        }

        int_type underflow() override {
            if (gptr() < egptr())
                return traits_type::to_int_type(*gptr());

            const ssize_t nread = ::read(m_fd, m_readBuf.data(), m_readBuf.size());
            if (nread <= 0)
                return traits_type::eof();

            setg(m_readBuf.data(),
                 m_readBuf.data(),
                 m_readBuf.data() + nread);

            return traits_type::to_int_type(*gptr());
        }

    private:
        bool flushWrite() {
            const std::ptrdiff_t pending = pptr() - pbase();
            if (pending <= 0)
                return true;

            const char* ptr = pbase();
            std::ptrdiff_t remaining = pending;

            while (remaining > 0) {
                const ssize_t written = ::write(m_fd, ptr, static_cast<std::size_t>(remaining));
                if (written < 0) {
                    if (errno == EINTR) continue;
                    return false;
                }
                ptr += written;
                remaining -= written;
            }

            setp(m_writeBuf.data(), m_writeBuf.data() + m_writeBuf.size());
            return true;
        }

        int m_fd;
        std::vector<char> m_readBuf;
        std::vector<char> m_writeBuf;
    };
}

namespace std {
    struct fifstream::Impl {
        FdStreamBuf buf;

        explicit Impl(int fd) : buf(fd) {
        }
    };

    struct fofstream::Impl {
        FdStreamBuf buf;

        explicit Impl(int fd) : buf(fd) {
        }
    };

    fifstream::fifstream(int fd) : istream(nullptr), m_impl(std::make_unique<Impl>(fd)) {
        rdbuf(&m_impl->buf);
    }

    fifstream::~fifstream() = default;

    int fifstream::fd() const {
        return m_impl->buf.fd();
    }

    fofstream::fofstream(int fd) : std::ostream(nullptr), m_impl(std::make_unique<Impl>(fd)) {
        rdbuf(&m_impl->buf);
    }

    fofstream::~fofstream() {
        flush();
    }

    int fofstream::fd() const {
        return m_impl->buf.fd();
    }
}
