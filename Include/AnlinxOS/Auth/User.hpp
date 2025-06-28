// For anyone thinking that those docs in here are AI-written, that's because I let the CLion's AI
// (or rather, the JetBrains AI) do the job here. The same goes for the "Group.hpp" file.

#ifndef ANLINOS_USER_HPP
#define ANLINOS_USER_HPP

#include <string>
#include <optional>
#include <vector>
#include <map>
#include <filesystem>
#include <any>

#include "Group.hpp"

namespace AnlinOS::Auth {

    /**
     * @enum UserType
     *
     * @brief Represents the group type that a user can be assigned to within the system.
     *
     * This enum defines the different categories of users in the system based on their
     * access level, role, and purpose. Each type serves a specific functionality and level
     * of interaction with the operating system and its resources.
     *
     * @constant SYSTEM_USER
     * A system user reserved for specific components of the system, like core services
     * such as networking, audio subsystems, or other privileged components. Examples include
     * root, networking, and audio.
     *
     * @constant SYSTEM_SERVICE_USER
     * A system service user intended for executing certain actions requiring interaction
     * with system-level users. Used for running essential services that access system
     * resources without requiring root access. Examples include sshd, nginx, and apache.
     *
     * @constant DATA_SERVICE_USER
     * A user-level service user that operates on behalf of regular users. Facilitates
     * communication with system user services through inter-process communication (IPC).
     * These users are accessible to normal users for specific service interactions.
     *
     * @constant DATA_ACCESSIBLE_USER
     * A general-purpose user that can be logged into and accessed by regular users. These
     * accounts are used for managing personal user data and often provide access to the
     * Desktop Environment. User-specific data is stored in directories, typically under
     * "/Data/UserDir/{username}", modified from the standard FHS.
     */
    enum UserType {
        SYSTEM_USER = 0,
        SYSTEM_SERVICE_USER = 1,
        DATA_SERVICE_USER = 2,
        DATA_ACCESSIBLE_USER = 3
    };

    /**
     * @struct User
     *
     * @brief Represents an individual user within the system, encapsulating key attributes
     * derived from the system's user database, such as "/etc/passwd".
     *
     * The User structure stores detailed metadata about a user, including their username, ID,
     * primary group membership, account type, and additional optional information. It is
     * designed for use in system-level and user-level interactions, providing functionality
     * for managing user-specific data and configurations.
     *
     * The structure is subject to updates as development progresses, ensuring compatibility
     * with evolving standards and requirements.
     *
     * @var std::string User::u_name
     * The unique username of the user.
     *
     * @var std::optional<std::string> User::u_pw
     * The password (or password hash) for the user, if applicable.
     *
     * @var int User::u_id
     * The unique numerical identifier for the user (UID).
     *
     * @var Group User::g_primary
     * The primary group to which the user belongs.
     *
     * @var std::optional<UserType> User::u_type
     * The type of user, based on the UserType enumeration. Helps define the user's role
     * and permissions within the system.
     *
     * @var std::optional<std::map<std::string, std::string>> User::u_info
     * Optional additional metadata or information about the user, represented as a key-value map.
     *
     * @var std::filesystem::path User::u_home
     * The absolute path to the user's home directory.
     *
     * @var std::filesystem::path User::u_shell
     * The absolute path to the user's default shell executable.
     */
    struct User {
        std::string u_name;
        std::optional<std::string> u_pw;
        int u_id;
        Group g_primary;
        std::optional<UserType> u_type;
        std::optional<std::map<std::string, std::string>> u_info;
        std::filesystem::path u_home;
        std::filesystem::path u_shell;
    };

    // TODO: declare the methods

    // Assigns a newly created user an UID to the System Runtime
    void AssignUserID(User& user);

    // Updates the user with the given fields from the "user_content" parameter.
    // This may include the UID, pGID, Group Type, and other fields.
    // Any external key will be assigned to "u_info"
    // Key Parameter "u_pw" will be ignored, use "UpdateUserPass" instead
    void UpdateUser(User& user, const std::map<std::string, std::any>& user_content);

    // Updates the user password
    // again; I don't have any way to actually store them securely, so don't attack me
    void UpdateUserPass(User& user, const std::string& pw_hash);

    // Assigns the user to an existing group
    // This will effectively update the Group itself, adding the given user to the Group Members.
    void AssignUserToGroup(User& user, Group& group);

}

#endif //ANLINOS_USER_HPP
