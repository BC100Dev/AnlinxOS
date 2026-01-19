#ifndef ANLINXOS_BOOTPROCESS_HPP
#define ANLINXOS_BOOTPROCESS_HPP

namespace BProc {

    enum class BootTo {
        SYSTEM = 0,
        RECOVERY = 1,
        SHELL = 2,
        INIT_SHELL = 3
    };

    enum class InitProcess {
        INITRAMFS = 0x0A,
        INIT_SYSTEM = 0x0F,
        INIT_RECOVERY = 0x0B
    };

    inline auto currentInitProcess = InitProcess::INITRAMFS;
    inline auto shouldContinueIntoSystem = BootTo::SYSTEM;

}

#endif //ANLINXOS_BOOTPROCESS_HPP