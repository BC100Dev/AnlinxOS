#ifndef ANLINXOS_POWER_H
#define ANLINXOS_POWER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    POWER_ALLOWANCE_PERMISSION_GRANTED = 0,
    POWER_ALLOWANCE_USER_ADMINISTRATOR = 1,
    POWER_ALLOWANCE_SYSTEM_USER = 2,
    POWER_ALLOWANCE_NOT_GRANTED = -1
} Power__ActionAllowance;

// This will check, whether the current permission has the "MANAGE_POWER_STATE" permission granted / whitelisted,
// or whether the process is being executed as "uadmin" or other system-level users.
Power__ActionAllowance Power__GetPermissionLevel();

// This call can be used by any processes, in which that process either require:
// - The process/executable having the "MANAGE_POWER_STATE" permission granted or whitelisted
// - The running process being executed under the "uadmin" user
// - The process being executed under system-level users (system, root)
//
// For the first check to be allowed, the current user needs to be a part of the "administrators" group to perform this call.
// If an error is set, errno will be defined accordingly with the given error, as defined per <ALX64/errno.h>
bool Power__PerformReboot(const char* reason);

// This call can be used by any processes, in which that process either require:
// - The process/executable having the "MANAGE_POWER_STATE" permission granted or whitelisted
// - The running process being executed under the "uadmin" user
// - The process being executed under system-level users (system, root)
//
// For the first check to be allowed, the current user needs to be a part of the "administrators" group to perform this call.
// If an error is set, errno will be defined accordingly with the given error, as defined per <ALX64/errno.h>
bool Power__PerformShutdown(const char* reason);

// This call can only be used by system-level processes.
// No userland apps can use this method, as they will always receive a PERM_DENIED error.
// If an error is set, errno will be defined accordingly with the given error, as defined per <ALX64/errno.h>
bool Power__PerformForcedReboot();

// This call can only be used by system-level processes.
// No userland apps can use this method, as they will always receive a PERM_DENIED error.
// If an error is set, errno will be defined accordingly with the given error, as defined per <ALX64/errno.h>
bool Power__PerformForcedShutdown();

#ifdef __cplusplus
}
#endif

#endif //ANLINXOS_POWER_H
