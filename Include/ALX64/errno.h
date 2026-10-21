#ifndef ANLINXOS_ERRNO_H
#define ANLINXOS_ERRNO_H

#include "defines.h"

#define ERR_PERMISSION_DENIED 1005
#define ERR_VALUE_NOT_SUPPORTED 1008
#define ERR_FILESYSTEM_NOT_MOUNTED 1009


EXPORT_METHOD char* alx_strerror(int errnum);

EXPORT_METHOD char* alx_localized_strerror(int errnum);

#ifdef __cplusplus
#include <string>

namespace ALX64 {
    std::string strerror(int errnum);
    std::string strerror(int errnum, bool localized);
}
#endif

#endif //ANLINXOS_ERRNO_H
