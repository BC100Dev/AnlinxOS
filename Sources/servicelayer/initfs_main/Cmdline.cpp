#include "BootProcedure.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

#include <ALX64/Utilities.hpp>

namespace fs = std::filesystem;

namespace BootStage {

    std::vector<BootParam> getKernelBootArguments() {
        std::vector<BootParam> results;

        std::ifstream file("/proc/cmdline");
        if (!file.is_open())
            throw std::runtime_error("Failed to open cmdline file: /proc/cmdline, error: " + std::to_string(errno));

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        std::string cmdline = buffer.str();

        std::vector<std::string> tokens = ALX64::Utils::TranslateStringToCommandline(cmdline);

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

}

