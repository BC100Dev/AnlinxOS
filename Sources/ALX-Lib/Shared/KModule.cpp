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

        if ((modinfo_size == -1 && modinfo_offset == -1) && modinfo_size > 0) {
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

        struct stat st{};
        if (fstat(fd, &st) < 0) {
            close(fd);
            return MODULE_INVALID_FORMAT;
        }

        void* image = mmap(nullptr, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        if (image == MAP_FAILED) {
            close(fd);
            return MODULE_INVALID_FORMAT;
        }

        int ret = MODULE_ACTION_SUCCESS;
        long sysret = -1;

        if (mfd.forceLoad) {
            auto* ehdr = reinterpret_cast<Elf64_Ehdr*>(image);
            if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) == 0 && ehdr->e_ident[EI_CLASS] == ELFCLASS64) {
                auto* shdrs = reinterpret_cast<Elf64_Shdr*>(
                        reinterpret_cast<uint8_t*>(image) + ehdr->e_shoff);
                Elf64_Shdr* sh_str = &shdrs[ehdr->e_shstrndx];
                const char* shstrtab = reinterpret_cast<const char*>(
                        reinterpret_cast<uint8_t*>(image) + sh_str->sh_offset);

                for (int i = 0; i < ehdr->e_shnum; ++i) {
                    std::string secname(&shstrtab[shdrs[i].sh_name]);
                    if (secname == ".modinfo") {
                        char* modinfo = reinterpret_cast<char*>(
                                reinterpret_cast<uint8_t*>(image) + shdrs[i].sh_offset);

                        size_t modinfo_size = shdrs[i].sh_size;
                        size_t off = 0;
                        while (off < modinfo_size) {
                            size_t len = strnlen(&modinfo[off], modinfo_size - off);
                            if (len == 0) { off++; continue; }
                            std::string entry(&modinfo[off], len);
                            if (entry.find("vermagic=") == 0) {
                                memset(&modinfo[off], 0, len);
                                break;
                            }

                            off += len + 1;
                        }
                        break;
                    }
                }
            }
        }
    }

}