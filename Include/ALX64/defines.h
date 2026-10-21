#ifndef ANLINXOS_DEFINES_H
#define ANLINXOS_DEFINES_H

#define NAKED_METHOD __attribute__((naked))
#define EXPORT_METHOD extern "C"

#define SYSTEM_MOUNT_DEFAULT "/System"
#define SYSTEM_MOUNT_LOWERCS "/system"

#define USERDT_MOUNT_DEFAULT "/Data"
#define USERDT_MOUNT_LOWERCS "/data"

#define VENDOR_MOUNT_DEFAULT "/vendor"

#define DLKM_SYSTEM_MOUNT "/system_dlkm"
#define DLKM_VENDOR_MOUNT "/vendor_dlkm"

#define VENDOR_INIT_FILE_ROOT VENDOR_MOUNT_DEFAULT "/vendor_init"
#ifdef __x86_64__
#define VENDOR_INIT_FILE_BIN VENDOR_MOUNT_DEFAULT "/x64/vendor_init"
#elifdef __aarch64__
#define VENDOR_INIT_FILE_BIN VENDOR_MOUNT_DEFAULT "/arm64/vendor_init"
#endif
#define VENDOR_INIT_FILE_LOOKUP VENDOR_INIT_FILE_BIN ":" VENDOR_INIT_FILE_ROOT

#endif //ANLINXOS_DEFINES_H
