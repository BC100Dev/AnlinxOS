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

    // The group type that the user can be assigned to
    enum UserGroupType {
        SYSTEM_USER = 0,
        SYSTEM_SERVICE_USER = 1,
        DATA_SERVICE_USER = 2,
        LOGIN_USER = 3
    };

    // The user structure itself
    // this is currently taken out of "/etc/passwd" structure; will be updated throughout the development
    struct User {
        std::string u_name;
        std::string u_pw;
        int u_id;
        int u_primary_gid;
        UserGroupType u_group_type;
        std::optional<std::map<std::string, std::string>> u_info;
        std::filesystem::path u_home = "/Data/UserDir/" + u_name;
        std::filesystem::path u_shell = "/System/x64/zsh";
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
