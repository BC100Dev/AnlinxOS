#include "FuncHeader.hpp"

#include "cmd/list.hpp"

std::map<std::string, std::function<int(int, char**, char**)>> funcMap = {
    {"cmdreq", cmdreq_func}
};
