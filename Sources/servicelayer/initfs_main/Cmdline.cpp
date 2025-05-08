#include "Cmdline.hpp"
#include "AnlinOS/Shared/Utils.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

const std::string S_OLD_PATH = "/proc/cmdline";
const std::string S_NEW_PATH = "/System/Runtime/cmdline";

namespace fs = std::filesystem;

std::ifstream openFile() {
    std::string path = S_NEW_PATH;
    if (!fs::exists(path))
        path = S_OLD_PATH;

    if (!fs::exists(path))
        throw std::runtime_error("Failed to open cmdline file, error: No such file or directory");

    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Failed to open cmdline file, error: " + std::to_string(errno));

    return file;
}

std::vector<BootParam> parseCommandLineFile(const CmdLineFileLookup& lookup) {
    std::vector<BootParam> results;
    std::string cmdlinePath = lookup == NEW_PATH ? S_NEW_PATH : S_OLD_PATH;

    std::ifstream file = openFile();
    if (!file.is_open())
        throw std::runtime_error("Failed to open cmdline file: " + cmdlinePath + ", error: " + std::to_string(errno));

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::string cmdline = buffer.str();

    std::vector<std::string> tokens = translateCmdLine(cmdline);

    for (const auto& token : tokens) {
        auto pos = token.find('=');

        BootParam param{};
        param.flag = pos == std::string::npos;

        if (param.flag) {
            param.key = token;
        } else {
            param.key = token.substr(0, pos);
            param.value = token.substr(pos + 1);
        }

        results.push_back(param);
    }

    return results;
}