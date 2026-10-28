#include <ALX64/errno.h>

#include <cstring>
#include <map>
#include <string>

#include <unistd.h>

static std::map<int, std::string> err_map = {
    {ERR_PERMISSION_DENIED, "Permission for given process denied"},
    {ERR_VALUE_NOT_SUPPORTED, "Value has not been set"},
    {ERR_FILESYSTEM_NOT_MOUNTED, "Access for filesystem is not mounted"}
};

EXPORT_METHOD char* alx_strerror(int errnum) {
    if (!err_map.contains(errnum))
        return std::strerror(errnum);

    return err_map.at(errnum).data();
}

EXPORT_METHOD char* alx_localized_strerror(int errnum) {
    // we stubbing this shit out for now
    if (!err_map.contains(errnum))
        return std::strerror(errnum);

    return err_map.at(errnum).data();
}

namespace ALX64 {
    std::string strerror(int errnum) {
        return alx_strerror(errnum);
    }

    std::string strerror(int errnum, bool localized) {
        return localized ? alx_localized_strerror(errnum) : alx_strerror(errnum);
    }
}
