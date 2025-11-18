#ifndef ANLINXOS_DEVICES_HPP
#define ANLINXOS_DEVICES_HPP

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace BootStage {

    struct BootParam {
        bool flag;
        std::string key;
        std::string value;
    };

    /**
     * Processes arguments from <code>/proc/cmdline</code>
     *
     * @return a list bunch of boot parameters
     */
    std::vector<BootParam> getKernelBootArguments();

    /**
     * Scans the devices that are currently plugged in via USB and PCI.
     * This call is required to
     * @return
     */
    std::vector<std::pair<int, int>> scanDevices();

    void loadDevice(int vecID, int prodID);

    bool isFilesystemEncrypted(const fs::path& devPath);

    void decryptDevice(const fs::path& devPath, bool beautifyRender);

}

#endif //ANLINXOS_DEVICES_HPP
