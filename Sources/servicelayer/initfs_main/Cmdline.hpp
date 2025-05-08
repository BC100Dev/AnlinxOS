#ifndef ANLINOS_CMDLINE_HPP
#define ANLINOS_CMDLINE_HPP

#include <vector>
#include <string>

struct BootParam {
    bool flag;
    std::string key;
    std::string value;
};

enum CmdLineFileLookup {
    // Use "/System/Runtime/cmdline"
    NEW_PATH = 0,

    // Use "/proc/cmdline"
    OLD_PATH = 1
};

std::vector<BootParam> parseCommandLineFile(const CmdLineFileLookup& lookup);

#endif //ANLINOS_CMDLINE_HPP
