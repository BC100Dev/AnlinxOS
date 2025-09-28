#include <ALX64/Linux/KModule.hpp>

#include <iostream>

using namespace ALX::Linux;

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cerr << "Usage: " << argv[0] << " <kernel-module-file>" << std::endl;
        return 1;
    }

    if (!std::filesystem::exists(argv[1])) {
        std::cerr << argv[1] << ": kernel module file not found" << std::endl;
        return 1;
    }

    ModuleFD fd = GetModuleData(argv[1]);
    std::cout << "Found module from disk: " << fd.mod.name << std::endl;

    int res = LoadModule(fd);
    std::cout << "Loading result hex code: 0x" << std::hex << res << std::endl;

    res = UnloadModule(fd.mod.name);
    std::cout << "Unloading result hex code: 0x" << std::hex << res << std::endl;
    return 0;
}