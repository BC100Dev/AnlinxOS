#ifndef ALX64_PERMISSIONS_HPP
#define ALX64_PERMISSIONS_HPP

#include <functional>
#include <string>
#include <vector>

enum class PermissionStatus {
    GRANTED = 0xF1A,              // Granted permission
    GRANTED_TEMPORARILY = 0xF1B,  // Granted permission one-time
    DENIED = 0xF0A,               // Denied permission
    DENIED_PERMANENTLY = 0xF0D    // Permission permanently denied (req. Settings to grant permission)
};

enum class Permission {
    READ_STORAGE,
    READ_EXTERNAL_STORAGE,
    WRITE_STORAGE,
    WRITE_EXTERNAL_STORAGE,

    READ_REGISTRY,
    READ_SERVICE_REGISTRY,
    READ_SYSTEM_REGISTRY,
    READ_EXTERNAL_REGISTRY,
    WRITE_REGISTRY,
    WRITE_SERVICE_REGISTRY,
    WRITE_SYSTEM_REGISTRY,
    WRITE_EXTERNAL_REGISTRY,
    LOAD_EXT_REGISTRY_DATA,
    UNLOAD_EXT_REGISTRY_DATA,

    MOUNT_FILESYSTEMS,
    UNMOUNT_FILESYSTEMS,
    FORMAT_FILESYSTEMS,

    START_PROCESSES,
    START_PROCESSES_SUPERUSER, // << Allows a program to run processes as system / root
    TRACE_PROCESSES,           // << Allows a process to trace system calls
    MANAGE_PROCESSES,          // << Allows a process to list and kill processes

    ACCESS_PROFILE_DATA,      // << allows read and write access to user info outside the current profile
    ACCESS_PROFILE_FILES,     // << allows read and write access to user home directory outside the current profile
    INTERACT_ACROSS_PROFILES, // << allows access to other profiles outside the current running profile
    ACCESS_CLIPBOARD,         // << allows read and write access to the clipboard
    ACCESS_DESKTOP,           // << allows access to Desktop itself, along with wallpaper access and installing shortcuts

    INTERNET,              // << Allows the process to perform network connections
    MANAGE_NETWORK_STATE,  // << Allows the process to access network interfaces and their states
    BIND_NETWORK_STATE,    // << Allows for services, such as VPNs, to rely on this permission

    BLUETOOTH,
    BLUETOOTH_ADMIN,
    BLUETOOTH_ADVERTISE,
    BLUETOOTH_CONNECT,
    BLUETOOTH_SCAN,

    ACCESS_CAMERA_DEVICES,      // << Read / Write access to camera devices, along with creation of virtual cameras
    ACCESS_AUDIO_DEVICES,       // << Read / Playback access to audio devices
    ACCESS_USB_DEVICES,         // << Read / Write access to external USB devices (if kernel module is not present for a specific device)
    ACCESS_INPUT_DEVICES,       // << Read / Write access to input devices (e.g. keyboard, mouse)
    ACCESS_OUTPUT_DEVICES,      // << Read / Write access to output devices (e.g. GPUs, monitors, screens, cameras)
    ACCESS_SENSORS,             // << Access to regular sensors (e.g. Accelerator, Gyroscope)
    ACCESS_BODY_SENSORS,        // << Separate from regular sensors (Accelerator), this allows to access sensors to a human body (e.g. heart beat)
    ACCESS_NFC_DEVICES,         // << Allows access to Near-Field Communication devices
    ACCESS_BIOMETRIC_DEVICES,   // << Allows access to devices like the IR and fingerprint sensors
    FIND_NEARBY_DEVICES,

    ACCESS_PRECISE_LOCATION,   // << GPS location
    ACCESS_AVERAGE_LOCATION,   // << IP-based location

    KERNEL_ACCESS,           // << allows the retrieval of the kernel status (loaded modules, version)
    KERNEL_LOAD_MODULES,     // << perform load_module call
    KERNEL_UNLOAD_MODULES,   // << perform unload_module call
    KERNEL_ADD_MODULES,      // << adds additional kernel modules
    KERNEL_DELETE_MODULES,   // << removes kernel modules from the system

    ACCESS_IPC,
    ACCESS_IPC_MEMORY,
    ACCESS_IPC_PIPES,
    ACCESS_IPC_SOCKETS,
    ACCESS_IPC_CROSS_USERS_HIGHER, // << Allows processes to communicate with higher users (e.g. system/root)
    ACCESS_IPC_CROSS_USERS_LOWER,  // << Allows processes to communicate with lower users (e.g. normal/admin users from services)

    ACCESS_NOTIFICATIONS,          // << Allows the process to send and list notifications
    ACCESS_NOTIFICATION_CONTENTS,  // << Allows the process to read notification contents

    LIST_PACKAGES,            // << Allows the process to list packages
    REQUEST_INSTALL_PACKAGES, // << Allows the process to request installation procedures
    REQUEST_REMOVE_PACKAGES,  // << Allows the process to request uninstallation procedures

    ACCESS_LOCK_STATE,        // << Allows to check, whether
    ACCESS_POWER_STATE,       // << Allows access to the current power config and state
    ACCESS_POWER_MANAGEMENT,  // << Allows access to manage power configurations between balance and other configs
    START_UPON_BOOT,          // << Allows programs to launch upon successful boot
    START_UPON_BOOT_COMPLETE, // << Allows programs to launch upon a successful boot, once the user is logged in

    PERFORM_SCREEN_CAPTURE,
    PERFORM_SCREENSHOTS,

    TELEPHONY,              // << Access to Modem Interfaces
    MANAGE_APN_SETTINGS,
    PERFORM_CALLS,
    RECEIVE_CALLS,
    USE_SIP,                // << Manage VoIP / SIP calls
    RWS_SMS_COMMUNICATIONS, // << Receive / Read, Write and Send
    RWS_MMS_COMMUNICATIONS, // << Receive / Read, Write and Send
    READ_PHONE_NUMBERS,     // << Allows the process to access the phone numbers

    MANAGE_DEVICE_STATE,        // << Allows the process to read and write the device state
    MANAGE_CONTACTS,
    MANAGE_CALENDAR,
    MANAGE_CALL_LOGS,
    MANAGE_SETTINGS,
    MANAGE_SECURE_SETTINGS,
    MANAGE_SYSTEM_PREFERENCES,  // << Access to system registry and configurations
    MANAGE_ALARMS,              // << Access to wake-up alarms (e.g. schedule to perform actions, like playing an alarm)

    MANAGE_ACCOUNTS,      // << Read / Insert access to Account Storage
    MANAGE_CREDENTIALS,   // << Read / Write access to Credential Storage,
};

struct PermissionRequest {
    Permission permission;
    PermissionStatus status;
};

using PermissionCallback = std::function<void(const std::vector<PermissionRequest>&)>;


PermissionStatus CheckPermissionStatus(const Permission& p);

void RequestPermission(const std::vector<Permission>& pList, const PermissionCallback& callback);

#endif //ALX64_PERMISSIONS_HPP
