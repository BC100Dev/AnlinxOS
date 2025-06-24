#ifndef ANLINXOS_KMODULE_HPP
#define ANLINXOS_KMODULE_HPP

#define MODULE_ACTION_SUCCESS 0x100000
#define MODULE_VERMAGIC_MISMATCH 0x100001
#define MODULE_SYMBOL_MISSING 0x100002
#define MODULE_PERMISSION_DENIED 0x100003
#define MODULE_INVALID_FORMAT 0x100004
#define MODULE_NOT_FOUND 0x100005
#define MODULE_INSERTION_DISCOURAGED 0x100006

#include <string>
#include <vector>
#include <fstream>

namespace ALX::Linux {

    enum SyscallInsertion {
        FINIT_MODULE = 0,
        INIT_MODULE = 1
    };

    struct Module {
        std::string name;
        std::string description;
        std::string author;
        std::string license;
        std::string srcVersion;
        std::vector<std::string> depends;
        std::string vermagic;
    };

    struct ModuleFD {
        Module mod;
        int fd;
        std::string path;
        std::vector<std::string> params;
        SyscallInsertion insertion = FINIT_MODULE;

        /**
         * UNSAFE!!! This will patch out the vermagic temporarily due to the kernel expecting the EXACT kernel version.
         * This has unintended side effects, if set to true. One side effect comes immediately, being kernel tainting.
         * See other possible effects: <a href="https://ccrma.stanford.edu/planetccrma/man/man8/insmod.8.html">insmod.8</a>
         */
        bool forceLoad = false;
    };

    ModuleFD GetModuleData(const std::string &path);

    int LoadModule(const ModuleFD &fd);

    int LoadModule(const ModuleFD &fd, int flags);

    void UnloadModule(const std::string &name);

    std::vector<Module> ListModules();

}

#endif //ANLINXOS_KMODULE_HPP
