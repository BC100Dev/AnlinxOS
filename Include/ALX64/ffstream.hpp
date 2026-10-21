#ifndef std_ffstream
#define std_ffstream

#include <istream>
#include <ostream>
#include <memory>

namespace std {
    class fifstream : public std::istream {
    public:
        explicit fifstream(int fd);
        ~fifstream() override;

        fifstream(const fifstream&) = delete;
        fifstream& operator=(const fifstream&) = delete;

        int fd() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };

    class fofstream : public std::ostream {
    public:
        explicit fofstream(int fd);
        ~fofstream() override;

        fofstream(const fofstream&) = delete;
        fofstream& operator=(const fofstream&) = delete;

        int fd() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> m_impl;
    };
}

#endif //std_ffstream
