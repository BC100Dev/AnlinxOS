#include <ALX/linux_ext/KModule.hpp>

#include <AnlinxOS/Shared/Utils.hpp>

#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <vector>
#include <sstream>
#include <fstream>
#include <cerrno>
#include <elf.h>
#include <sys/utsname.h>
#include <iostream>


static std::string vermagic_data() {
    struct utsname u{};
    if (uname(&u) != 0)
        return "";

    return u.release;
}

namespace ALX::Linux {

    std::vector<Module> ListModules() {
        std::vector<Module> modules;
        std::ifstream file("/proc/modules");
        if (!file.is_open())
            throw std::runtime_error("file read failed: /proc/modules (errno " + std::to_string(errno) + ")");

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string name, size, instances, dependencies, state, address;

            if (!(iss >> name >> size >> instances >> dependencies >> state >> address))
                continue;

            Module mod;
            mod.name = name;

            if (dependencies != "-") {
                std::stringstream deps(dependencies);
                std::string dep;
                while (std::getline(deps, dep, ','))
                    mod.depends.push_back(TrimString(dep));
            }

            modules.push_back(mod);
        }

        return modules;
    }

    ModuleFD GetModuleData(const std::filesystem::path& path) {
        if (!std::filesystem::exists(path))
            throw std::runtime_error("module at \"" + path.string() + "\" not found");

        ModuleFD fd;
        fd.path = path;

        std::ifstream file(path, std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("module open failed, errno: " + std::to_string(errno));

        unsigned char e_ident[EI_NIDENT];
        file.read(reinterpret_cast<char*>(e_ident), EI_NIDENT);
        if (memcmp(e_ident, ELFMAG, SELFMAG) != 0)
            return fd;

        if (e_ident[EI_CLASS] != ELFCLASS64) {
            file.close();

            throw std::runtime_error("module open failed: not a 64-bit kernel module");
        }

        file.seekg(0, std::ios::beg);
        Elf64_Ehdr ehdr;
        file.read(reinterpret_cast<char*>(&ehdr), sizeof(Elf64_Ehdr));

        uint64_t shoff = ehdr.e_shoff;
        uint16_t shentsize = ehdr.e_shentsize;
        uint16_t shnum = ehdr.e_shnum;
        uint16_t shstrndx = ehdr.e_shstrndx;

        file.seekg(shoff, std::ios::beg);
        std::vector<char> sh_buf(shentsize * shnum);
        file.read(sh_buf.data(), sh_buf.size());

        const auto* shdrs = reinterpret_cast<const Elf64_Shdr*>(sh_buf.data());
        uint64_t shstr_size = shdrs[shstrndx].sh_size;
        std::vector<char> shstrtab(shstr_size);
        file.seekg(shdrs[shstrndx].sh_offset, std::ios::beg);
        file.read(shstrtab.data(), shstr_size);

        uint64_t modinfo_offset = -1;
        uint64_t modinfo_size = -1;

        for (uint64_t i = 0; i < shnum; i++) {
            std::string secname(&shstrtab[shdrs[i].sh_name]);
            if (secname == ".modinfo") {
                modinfo_offset = shdrs[i].sh_offset;
                modinfo_size = shdrs[i].sh_size;
                break;
            }
        }

        if (modinfo_size > 0 && modinfo_offset != -1) {
            std::vector<char> modinfo(modinfo_size);
            file.seekg(modinfo_offset, std::ios::beg);
            file.read(modinfo.data(), modinfo.size());

            size_t off = 0;
            while (off < modinfo.size()) {
                size_t end = off;
                while (end < modinfo.size() && modinfo[end] != '\0') ++end;

                std::string entry(&modinfo[off], end - off);
                off = end + 1;
                auto eq = entry.find('=');
                if (eq == std::string::npos)
                    continue;

                std::string key = entry.substr(0, eq);
                std::string val = entry.substr(eq + 1);

                if (key == "name") fd.mod.name = val;
                else if (key == "description") fd.mod.description = val;
                else if (key == "author") fd.mod.author = val;
                else if (key == "license") fd.mod.license = val;
                else if (key == "srcversion") fd.mod.srcVersion = val;
                else if (key == "vermagic") fd.mod.vermagic = val;
                else if (key == "depends") {
                    std::stringstream deps(val);
                    std::string dep;

                    while (std::getline(deps, dep, ','))
                        fd.mod.depends.push_back(TrimString(dep));
                }
            }
        }

        file.close();

        return fd;
    }

    int LoadModule(const ModuleFD& mfd) {
        return LoadModule(mfd, 0);
    }

    int LoadModule(const ModuleFD& mfd, int flags) {
        if (!std::filesystem::exists(mfd.path))
            return MODULE_NOT_FOUND;

        int fd = open(mfd.path.c_str(), O_RDONLY);
        if (fd < 0)
            return MODULE_NOT_FOUND;

        int ret = MODULE_ACTION_SUCCESS;

        if (mfd.forceLoad) {
            std::cerr << "[STUB] module force load not implemented" << std::endl;
            return MODULE_INSERTION_DISCOURAGED;
        }

        std::string str;
        const char* params;
        {
            std::stringstream sParams;
            for (size_t i = 0; i < mfd.params.size(); i++) {
                sParams << mfd.params[i];

                if (i != mfd.params.size() - 1)
                    sParams << " ";
            }

            str = sParams.str();
            params = str.c_str();
        }

        long sysret = syscall(SYS_finit_module, fd, params, flags);
        if (sysret != 0) {
            switch (errno) {
                case 0:
                    ret = MODULE_ACTION_SUCCESS;
                    break;
                case EPERM:
                case EACCES:
                case ENOEXEC:
                    ret = MODULE_PERMISSION_DENIED;
                    break;
                case EINVAL:
                case EFAULT:
                    ret = MODULE_INVALID_FORMAT;
                    break;
                case ENOKEY:
                case EBUSY:
                    ret = MODULE_INSERTION_DISCOURAGED;
                    break;
                case EEXIST:
                    ret = MODULE_ALREADY_LOADED;
                    break;
                case ENOSYS:
                case ENOSPC:
                case ENOMEM:
                    ret = MODULE_SYSTEM_FAILURE;
                    break;
                default:
                    ret = MODULE_UNDEFINED_ERROR;
                    break;
            }
        }

        close(fd);

        return ret;
    }

    int UnloadModule(const std::string& name) {
        std::vector<Module> mods = ListModules();
        bool found = false;
        for (const auto& mod : mods) {
            if (mod.name == name) {
                found = true;
                break;
            }
        }

        if (!found)
            return MODULE_NOT_LOADED;

        int ret = MODULE_ACTION_SUCCESS;
        long sysret = syscall(SYS_delete_module, name.c_str(), 0);
        if (sysret != 0) {
            switch (errno) {
                case 0:
                    ret = MODULE_ACTION_SUCCESS;
                    break;
                case EPERM:
                case EACCES:
                case ENOEXEC:
                    ret = MODULE_PERMISSION_DENIED;
                    break;
                case EINVAL:
                case EFAULT:
                    ret = MODULE_INVALID_FORMAT;
                    break;
                case ENOKEY:
                case EBUSY:
                    ret = MODULE_INSERTION_DISCOURAGED;
                    break;
                case EEXIST:
                    ret = MODULE_ALREADY_LOADED;
                    break;
                case ENOSYS:
                case ENOSPC:
                case ENOMEM:
                    ret = MODULE_SYSTEM_FAILURE;
                    break;
                default:
                    ret = MODULE_UNDEFINED_ERROR;
                    break;
            }
        }

        return ret;
    }

}