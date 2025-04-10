#include <iostream>
#include <vector>
#include <sstream>
#include <filesystem>

#include <unistd.h>

namespace fs = std::filesystem;

struct BinaryInfo {
    std::string binPath;
    bool executable = false;
};

std::vector<std::string> translateCmdLine(const std::string& cmdLine) {
    if (cmdLine.empty() || cmdLine.find_first_not_of(' ') == std::string::npos)
        return {};

    const int normal = 0;
    const int inQuote = 1;
    const int inDoubleQuote = 2;
    int state = normal;

    std::vector<std::string> result;
    std::stringstream current;
    bool lastTokenHasBeenQuoted = false;

    for (char nextTok : cmdLine) {
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

BinaryInfo find_binary(const std::string& binName) {
    BinaryInfo result;

    const char* env = std::getenv("PATH");
    std::stringstream pathEnv(env ? env : "/System/x64:/System/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/bin:/usr/local/sbin");
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

int main(int argc, char** argv) {
    int rc = 0;
    while (true) {
        std::string cmdline;

        if (rc != 0)
            std::cout << "[" << rc << "]";

        if (getuid() == 0)
            std::cout << "# ";
        else
            std::cout << "$ ";

        std::getline(std::cin, cmdline);
        if (cmdline.empty())
            continue;

        std::vector<std::string> tokens = translateCmdLine(cmdline);
        if (tokens.empty())
            continue;

        if (tokens[0] == "exit")
            break;

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