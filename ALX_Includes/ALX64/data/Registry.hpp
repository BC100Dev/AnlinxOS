#ifndef ANLINXOS_REGISTRY_HPP
#define ANLINXOS_REGISTRY_HPP

#include <string>
#include <filesystem>

namespace ALX64 {

    class RegistryHandle {
    public:
        virtual ~RegistryHandle() = default;

        virtual void WriteData(const std::string &key, const char* data) = 0;

        virtual const char* ReadData(const std::string& key) = 0;

        virtual bool DataExists(const std::string& key) = 0;

        virtual ssize_t DataLength(const std::string& key) = 0;

        virtual long DataIndexAt(const std::string& key) = 0;

        virtual RegistryHandle OpenHandle(const std::filesystem::path& path, int flags) = 0;

        virtual void WriteRegistryToFile(const std::filesystem::path& path) = 0;

        virtual void ReadRegistryFromFile(const std::filesystem::path& path) = 0;
    };

    class SystemRegistry : public RegistryHandle {
    };

    class ServiceRegistry : public RegistryHandle {
    };

    class UserRegistry : public RegistryHandle {
    };

    namespace Super {

        void WriteData(RegistryHandle& handle, const std::string& key, const char* data);

        const char* ReadData(RegistryHandle& handle, const std::string& key);

        bool DataExists(RegistryHandle& handle, const std::string& key);

        ssize_t DataLength(RegistryHandle& handle, const std::string& key);

        long DataIndexAt(RegistryHandle& handle, const std::string& key);

        RegistryHandle OpenHandle(RegistryHandle& handle, const std::filesystem::path& path, int flags);

        void WriteRegistryToFile(RegistryHandle& handle, const std::filesystem::path& path);

        void ReadRegistryFromFile(RegistryHandle& handle, const std::filesystem::path& path);

    }

}

#endif //ANLINXOS_REGISTRY_HPP
