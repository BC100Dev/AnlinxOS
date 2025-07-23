#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>
#include <fstream>

#include <unistd.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <linux/reboot.h>

#include "Cmdline.hpp"

#include <AnlinxOS/Shared/Utils.hpp>

namespace fs = std::filesystem;

struct BinaryInfo {
    std::string binPath;
    bool executable = false;
};

BinaryInfo find_binary(const std::string &binName) {
    BinaryInfo result;

    const char *env = std::getenv("PATH");
    std::stringstream pathEnv(
            env ? env : "/System/x64:/System/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/local/sbin");
    std::string currentPathEntry;

    while (std::getline(pathEnv, currentPathEntry, ':')) {
        fs::path path(currentPathEntry);
        if (fs::exists(path / binName)) {
            std::string fullPath = fs::canonical(path / binName).string();
            result.binPath = fullPath;
            result.executable = access(fullPath.c_str(), X_OK) == 0;

            break;
        }
    }

    return result;
}

bool is_init() {
    return getpid() == 1;
}

void print_args(const std::vector<std::string> &args) {
    for (int i = 0; i < args.size(); i++)
        std::cout << "Arg[" << i << "]: " << args[i] << std::endl;

    std::cout << std::endl;
}

int openShell(const std::vector<std::string> &args) {
    std::cout << "*** ANLIN OS Init Shell ***" << std::endl;
    int rc = 0;

    while (true) {
        std::string cmdline;

        if (rc != 0)
            std::cout << "[" << rc << "]";

        std::cout << (getuid() == 0 ? "#> " : "$> ");

        if (!std::getline(std::cin, cmdline)) {
            std::cout << "Caught EOF..." << std::endl;
            if (is_init())
                reboot(LINUX_REBOOT_CMD_RESTART);

            break;
        }

        if (cmdline.empty())
            continue;

        std::vector<std::string> tokens = translateCmdLine(cmdline);
        if (tokens.empty())
            continue;

        if (tokens[0] == "exit" || tokens[0] == "quit" || tokens[0] == "shutdown") {
            if (is_init()) {
                sync();
                rc = reboot(LINUX_REBOOT_CMD_POWER_OFF);
                if (rc != 0)
                    perror("shutdown failed");
            }

            break;
        }

        if (tokens[0] == "cliargs") {
            print_args(args);
            rc = 0;
            continue;
        }

        if (tokens[0] == "cmdline") {
            try {
                std::vector<BootParam> params = parseCommandLineFile(NEW_PATH);
                if (!params.empty()) {
                    for (const auto &param: params) {
                        if (param.flag)
                            std::cout << "[FLAG] " << param.key << std::endl;
                        else
                            std::cout << "[PAIR] " << param.key << "=" << param.value << std::endl;
                    }
                }
            } catch (const std::exception &e) {
                std::cerr << e.what() << std::endl;
                rc = 1;
            } catch (...) {
                std::cerr << "Unknown error" << std::endl;
                rc = 1;
            }

            continue;
        }

        if (tokens[0] == "_read") {
            if (tokens.size() < 2) {
                std::cerr << "Usage: _read <file>" << std::endl;
                rc = 1;
                continue;
            }

            std::ifstream file(tokens[1]);
            if (!file.is_open()) {
                std::cerr << "Failed to open " << tokens[1] << std::endl;
                rc = 1;
                continue;
            }

            std::cout << file.rdbuf() << std::endl;
            file.close();
            rc = 0;
            continue;
        }

        BinaryInfo info = find_binary(tokens[0]);
        if (info.binPath.empty()) {
            std::cerr << tokens[0] << ": command not found" << std::endl;
            rc = 127;
            continue;
        }

        if (!info.executable) {
            std::cerr << tokens[0] << ": not an executable" << std::endl;
            rc = 126;
            continue;
        }

        rc = std::system(cmdline.c_str());
    }

    return rc;
}

int mount_special(const std::string& src, const std::string& dist, const std::string& fs) {
    if (!fs::exists(dist))
        fs::create_directories(dist);

    return mount(src.c_str(), dist.c_str(), fs.c_str(), 0, nullptr);
}

int main(int argc, char **argv) {
    // order of the PATH var:
    // 1. point to the /System/bin directory (effectively gets symlinked to /System/x64 or /System/arm64, depending on the architecture)
    // 2. default path of /System/x64
    // 3. standard old-ass paths
    setenv("PATH", "/System/bin:/System/x64:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/local/sbin", 1);

    std::vector<std::string> args;
    for (int i = 0; i < argc; i++)
        args.emplace_back(argv[i]);

    if (is_init()) {
        if (mount_special("dev", "/dev", "devtmpfs") != 0)
            std::cerr << "Failed to mount dev, error: " << errno << std::endl;

        if (mount_special("proc", "/proc", "proc") != 0)
            std::cerr << "Failed to mount proc, error: " << errno << std::endl;

        if (mount_special("sysfs", "/sys", "sysfs") != 0)
            std::cerr << "Failed to mount sysfs, error: " << errno << std::endl;

        if (fs::exists("/dev/sda1")) {
            if (!fs::exists("/mnt"))
                fs::create_directory("/mnt");

            if (mount("/dev/sda1", "/mnt", "ext4", 1, nullptr) != 0)
                std::cerr << "Failed to mount /dev/sda1 to /mnt" << std::endl;
        }

        std::cout << "Started up the init process" << std::endl << std::endl;
    }

    int rc = openShell(args);
    return rc;
}
