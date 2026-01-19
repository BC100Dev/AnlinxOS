#ifndef ALX64_STORAGE_HPP
#define ALX64_STORAGE_HPP

#include <filesystem>
#include <stdexcept>

namespace fs = std::filesystem;

class StorageAccessError : public std::runtime_error {
public:
    explicit StorageAccessError(const std::string& what) : std::runtime_error(what) {}

    explicit StorageAccessError(const char* what) : std::runtime_error(what) {}
};

class StorageAccess {
public:
    enum class PublicDirectory {
        DIR_ALARMS = 0xff90,
        DIR_AUDIOBOOKS = 0xff91,
        DIR_DCIM = 0xff92,
        DIR_DOCUMENTS = 0xff93,
        DIR_DOWNLOADS = 0xff94,
        DIR_MOVIES = 0xff95,
        DIR_MUSIC = 0xff96,
        DIR_NOTIFICATIONS = 0xff97,
        DIR_PICTURES = 0xff98,
        DIR_PODCASTS = 0xff99,
        DIR_RECORDINGS = 0xff9a,
        DIR_RINGTONES = 0xff9b,
        DIR_SCREENSHOTS = 0xff9c,
        DIR_TEMPLATES = 0xff9d
    };

    bool hasFile(const fs::path& rootDir, const fs::path& inPath);

    bool hasFile(const fs::path& path);

    std::string readFile(const fs::path& rootDir, const fs::path& inPath);

    std::string readFile(const fs::path& path);

    fs::path getHomeDirectory();

    fs::path getApplicationDirectory();

    fs::path getConfigurationDirectory();

    fs::path getUserSharedDirectory();

    fs::path getCacheDirectory(bool inHome);

    fs::path getPublicDirectory(const PublicDirectory& pd);
};

class UserAccess {
public:
    StorageAccess getStorageAccess(int userID);
};

#endif //ALX64_STORAGE_HPP
