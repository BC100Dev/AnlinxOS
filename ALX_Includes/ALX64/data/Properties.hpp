#ifndef ANLINXOS_PROPERTIES_HPP
#define ANLINXOS_PROPERTIES_HPP

#include <any>
#include <string>
#include <optional>
#include <stdexcept>

#include "Registry.hpp"

#define WRITE_PERMISSION_DENIED 0x9c2a5950
#define WRITE_SERVICE_ERROR 0x9c2a5951
#define WRITE_SERVICE_DENIED 0x9c2a5952
#define WRITE_IO_ERROR 0x9c2a5953
#define WRITE_REGISTRY_INVALID 0x9c2a5954
#define WRITE_REGISTRY_HANDLE_DENIED 0x9c2a5955
#define WRITE_INVALID_SIGNATURE 0x9c2a5956
#define WRITE_INVALID_PROCESS 0x9c2a5957
#define WRITE_INVALID_HANDLER 0x9c2a5958

#define READ_PERMISSION_DENIED 0x9c2f5950
#define READ_SERVICE_ERROR 0x9c2f5951
#define READ_SERVICE_DENIED 0x9c2f5952
#define READ_IO_ERROR 0x9c2f5953
#define READ_REGISTRY_INVALID 0x9c2f5954
#define READ_REGISTRY_HANDLE_DENIED 0x9c2f5955
#define READ_INVALID_SIGNATURE 0x9c2f5956
#define READ_INVALID_PROCESS 0x9c2f5957
#define READ_INVALID_HANDLER 0x9c2f5958

namespace ALX64 {

    class PropertyHandlerError : public std::runtime_error {
    public:
        PropertyHandlerError(const std::string &what) : std::runtime_error(what) {}
    };

    class PropertyReadError : public PropertyHandlerError {
    public:
        PropertyReadError(const std::string &what) : PropertyHandlerError(what) {}
    };

    class PropertyWriteError : public PropertyHandlerError {
    public:
        PropertyWriteError(const std::string &what) : PropertyHandlerError(what) {}
    };

    /**
     * Retrieves the System Property associated with a specific system class.
     * @param sr The System Registry class
     * @param sp The System Property Key Name
     * @return a specific value type, depending on type the output can be served with
     */
    std::any GetSystemProperty(SystemRegistry &sr, const std::string &sp);

    /**
     * Retrieves a property from a service by calling it on the service registry, which fetches the corresponding properties from a
     * Service Registry. This will further involve by forwarding the reading onto either one of those possible ways:
     * <ul>
     * <li>The service responds with a valid given response data back to the process</li>
     * <li>The service manager forwards the reading call onto the User Registry and reads the Property File itself</li>
     * <li>The data fails to respond, throwing <code>PropertyReadError</code> in response</li>
     * </ul>
     *
     * @param sn service name itself
     * @param sr the service registry
     * @param pf property file name
     * @param pk property key name
     * @return a valid response data
     * @throws PropertyReadError if the property retrieval fails
     * @throws std::filesystem::filesystem_error if the Service Manager failed to read the service
     */
    std::any GetServiceProperty(const std::string &sn, ServiceRegistry &sr,
                                const std::string &pf, const std::string &pk);

    /**
     * Retrieves a property from a specific user by calling onto the User Registry
     * @param ur the User Registry itself
     * @param pk property key name
     * @return a value containing the property data
     */
    std::any GetUserProperty(UserRegistry &ur, const std::string &pk);

    /**
     * Sets a specific system property.
     * <br><br>
     * Please do note that not every system property can be written by every user. Be mindful, as to what process this
     * gets called upon, since certain ones are only accessible by the Service Manager, the System Initialization Process
     * or certain system-signed executables.
     * @param sr The System Registry class
     * @param sp The System Property Key Name
     * @param d the property data itself
     */
    void SetSystemProperty(SystemRegistry &sr, const std::string &sp, const std::any &d);

    /**
     * Attempts to write a property to a service, effectively calling the service handler via the Service Manager to its corresponding
     * Service Registry. The service will either respond with a OK or a write error sign, depending on what the service expects
     * on the write calls. If the service fails to respond, the Service Manager attempts to write the data to the service manually,
     * which might require the service to be restarted, if the change does not happen immediately.
     *
     * @param sn service name itself
     * @param sr the service registry
     * @param pf property file name
     * @param pk property key name
     * @param d property data
     * @throws PropertyReadError if the property retrieval fails
     * @throws std::filesystem::filesystem_error if the Service Manager failed to read the service
     */
    void SetServiceProperty(const std::string &sn, ServiceRegistry &sr, const std::string &pf,
                            const std::string &pk, const std::any &d);

    /**
     * Writes a property for a specific user via their perspective User Registry
     * @param ur the User Registry itself
     * @param pk property key name
     * @param d property data
     */
    void SetUserProperty(UserRegistry &ur, const std::string &pk, const std::any& d);

}

#endif //ANLINXOS_PROPERTIES_HPP
