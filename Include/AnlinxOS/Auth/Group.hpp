// For anyone thinking that those docs in here are AI-written, that's because I let the CLion's AI
// (or rather, the JetBrains AI) do the job here. The same goes for the "User.hpp" file.

#ifndef ANLINOS_GROUP_HPP
#define ANLINOS_GROUP_HPP

#include <string>
#include <optional>
#include <vector>
#include <map>

#include "User.hpp"

namespace AnlinOS::Auth {

    /**
     * @enum GroupType
     * @brief Represents various types of groups with specific roles and privileges in the system.
     *
     * This enumeration defines the classifications of system and user groups,
     * each with varying levels of control and permission within the application ecosystem.
     *
     * @constant SYSTEM_GROUP
     * Represents system-level groups, typically reserved for core system processes and networking operations.
     *
     * @constant SYSTEM_SERVICE_GROUP
     * Represents groups reserved for user services and other users, typically acting in privileged contexts for such purposes.
     *
     * @constant DATA_PRIVILEGED_GROUP
     * Represents user-space groups that act as service operators for low-privileged users.
     * These groups are allowed to interface with system groups and use privileged commands such as "cmdreq".
     *
     * @constant DATA_SHARED_GROUP
     * Represents user-space groups with no special privileges.
     * These groups are intended for regular user operations and lack the ability to utilize elevated commands like "cmdreq".
     */
    enum GroupType {
        SYSTEM_GROUP = 0,
        SYSTEM_SERVICE_GROUP = 1,
        DATA_PRIVILEGED_GROUP = 2,
        DATA_SHARED_GROUP = 3
    };

    // This is taken out of the "/etc/group" wikis. This will be updated throughout the development.
    struct Group {
        std::string g_name;
        // pls dont attack me, I don't have any other way to store them now
        std::optional<std::string> g_pw;
        int g_id;
        std::optional<GroupType> g_type;
        std::optional<std::map<std::string, std::string>> g_info;
        std::vector<User> g_members;
    };

}

#endif //ANLINOS_GROUP_HPP
