#ifndef ANLINOS_GROUP_HPP
#define ANLINOS_GROUP_HPP

#include <string>
#include <optional>
#include <vector>

#include "User.hpp"

namespace AnlinOS::Auth {

    // This is taken out of the "/etc/group" wikis. This will be updated throughout the development.
    struct Group {
        std::string name;
        // pls dont attack me, I don't have any other way to store them now
        std::optional<std::string> pwHash;
        int gid;
        std::vector<User> members;
    };

}

#endif //ANLINOS_GROUP_HPP
