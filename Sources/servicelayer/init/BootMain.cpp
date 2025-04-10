#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>

#include <unistd.h>
#include <sys/mount.h>
#include <sys/reboot.h>
#include <linux/reboot.h>

namespace fs = std::filesystem;

struct BinaryInfo {
    std::string binPath;
    bool executable = false;
};

std::vector<std::string> translateCmdLine(const std::string &cmdLine) {
    if (cmdLine.empty() || cmdLine.find_first_not_of(' ') == std::string::npos)
        return {};

    const int normal = 0;
    const int inQuote = 1;
    const int inDoubleQuote = 2;
    int state = normal;

    std::vector<std::string> result;
    std::stringstream current;
    bool lastTokenHasBeenQuoted = false;

    for (char nextTok: cmdLine) {
        switch (state) {
            case inQuote:
                if (nextTok == '\'') {
                    lastTokenHasBeenQuoted = true;
                    state = normal;
                } else
                    current << nextTok;

                break;

            case inDoubleQuote:
                if (nextTok == '"') {
                    lastTokenHasBeenQuoted = true;
                    state = normal;
                } else
                    current << nextTok;

                break;

            default:
                if (nextTok == '\'')
                    state = inQuote;
                else if (nextTok == '"')
                    state = inDoubleQuote;
                else if (nextTok == ' ') {
                    if (lastTokenHasBeenQuoted || current.tellp() > 0) {
                        result.push_back(current.str());
                        current.str("");
                        current.clear();
                    }
                } else
                    current << nextTok;

                lastTokenHasBeenQuoted = false;
                break;
        }
    }

    if (lastTokenHasBeenQuoted || current.tellp() > 0)
        result.push_back(current.str());

    if (state == inQuote || state == inDoubleQuote)
        throw std::runtime_error("unbalanced quotes in " + cmdLine);

    return result;
}

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

int main(int argc, char **argv) {
    if (is_init()) {
        if (!fs::exists("/dev"))
            fs::create_directory("/dev");

        if (!fs::exists("/proc"))
            fs::create_directory("/proc");

        if (!fs::exists("/sys"))
            fs::create_directory("/sys");

        if (mount("dev", "/dev", "devtmpfs", 0, nullptr) != 0)
            std::cerr << "Failed to mount /dev" << std::endl;

        if (mount("proc", "/proc", "proc", 0, nullptr) != 0)
            std::cerr << "Failed to mount /proc" << std::endl;

        if (mount("sysfs", "/sys", "sysfs", 0, nullptr) != 0)
            std::cerr << "Failed to mount /sys" << std::endl;

        if (fs::exists("/dev/sda1")) {
            if (!fs::exists("/mnt"))
                fs::create_directory("/mnt");

            if (mount("/dev/sda1", "/mnt", "ext4", 0, nullptr) != 0)
                std::cerr << "Failed to mount /dev/sda1 to /mnt" << std::endl;
        }
    }

    std::cout << "Started up the init process" << std::endl << std::endl;
    std::cout << "*** ANLIN OS ***" << std::endl;

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