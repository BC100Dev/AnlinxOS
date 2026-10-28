#ifndef ALX64_FIRMWARE_H
#define ALX64_FIRMWARE_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef char* (*FirmwareResolveCallback)(const char* device_name, const char* firmware_name, void* userdata);

void Firmware__RegisterCallback(FirmwareResolveCallback callback, void* userdata);

char* Firmware__Lookup(const char* device_name, const char* firmware_name);

bool Firmware__HasFallbackSupport();

#ifdef __cplusplus
}

#include <functional>
#include <string>

namespace ALX64 {
    using FirmwareResolveCallback = std::function<std::string(const std::string& device_name,
                                                               const std::string& firmware_name)>;

    void RegisterFirmwareCallback(const FirmwareResolveCallback& callback);

    std::string LookupFirmware(const std::string& device_name, const std::string& firmware_name);

    bool HasFallbackSupport();
}

#endif

#endif //ALX64_FIRMWARE_H
