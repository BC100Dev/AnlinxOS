#include <ALX64/Power.h>
#include <ALX64/errno.h>

#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>

#include <cerrno>
#include <string>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace Power__Impl {

    static constexpr int POWER_CALL_SHUTDOWN = 0xdeadbeef;
    static constexpr int POWER_CALL_REBOOT = 0xdeadfaac;

    static void WriteReason(const std::string& reason) {
        auto reason_write = [&](const fs::path& path) {
            std::ofstream reasonFile(path / "reboot_reason");
            if (!reasonFile.is_open())
                return;

            reasonFile << reason;
            reasonFile.close();
        };

        if (fs::exists("/Cache"))
            reason_write("/Cache");
        else if (fs::exists("/cache"))
            reason_write("/cache");
        else
            errno = ERR_FILESYSTEM_NOT_MOUNTED;
    }

    static bool IsForcedShutdown(const std::string& reason) {
        return reason.contains("{{force}}");
    }

    static void RequestPowerCall(int pCall, bool forced) {
        // TODO: call IPC to init 1 to request a shutdown call

        // stub it out with this for now
        sync();
        switch (pCall) {
            case POWER_CALL_SHUTDOWN:
            reboot(LINUX_REBOOT_CMD_POWER_OFF);
            break;
        case POWER_CALL_REBOOT:
            reboot(LINUX_REBOOT_CMD_RESTART);
            break;
        default:
            errno = ERR_VALUE_NOT_SUPPORTED;
            break;
        }
    }
}

Power__ActionAllowance Power__GetPermissionLevel() {
    if (getuid() == 0)
        return POWER_ALLOWANCE_SYSTEM_USER;

    if (getpid() == 1)
        return POWER_ALLOWANCE_SYSTEM_USER;

    return POWER_ALLOWANCE_NOT_GRANTED;
}

bool Power__PerformReboot(const char* reason) {
    if (Power__Impl::IsForcedShutdown(reason)) {
        if (Power__GetPermissionLevel() != POWER_ALLOWANCE_SYSTEM_USER) {
            errno = ERR_PERMISSION_DENIED;
            return false;
        }

        Power__Impl::RequestPowerCall(Power__Impl::POWER_CALL_REBOOT, true);
        return true;
    }

    if (Power__GetPermissionLevel() == POWER_ALLOWANCE_NOT_GRANTED) {
        errno = ERR_PERMISSION_DENIED;
        return false;
    }

    Power__Impl::WriteReason(reason);
    Power__Impl::RequestPowerCall(Power__Impl::POWER_CALL_REBOOT, false);
    if (errno != ERR_VALUE_NOT_SUPPORTED)
        return false;

    return true;
}

bool Power__PerformShutdown(const char* reason) {
    if (Power__Impl::IsForcedShutdown(reason)) {
        if (Power__GetPermissionLevel() != POWER_ALLOWANCE_SYSTEM_USER) {
            errno = ERR_PERMISSION_DENIED;
            return false;
        }

        Power__Impl::RequestPowerCall(Power__Impl::POWER_CALL_SHUTDOWN, true);
        return true;
    }

    if (Power__GetPermissionLevel() == POWER_ALLOWANCE_NOT_GRANTED) {
        errno = ERR_PERMISSION_DENIED;
        return false;
    }

    Power__Impl::WriteReason(reason);
    Power__Impl::RequestPowerCall(Power__Impl::POWER_CALL_SHUTDOWN, false);
    if (errno != ERR_VALUE_NOT_SUPPORTED)
        return false;

    return true;
}

bool Power__PerformForcedReboot() {
    return Power__PerformReboot("{{force}}");
}

bool Power__PerformForcedShutdown() {
    return Power__PerformShutdown("{{force}}");
}
