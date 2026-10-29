#ifndef ANLINXOS_FUNCHEADER_HPP
#define ANLINXOS_FUNCHEADER_HPP

#include <map>
#include <string>
#include <functional>

extern std::map<std::string, std::function<int(int, char**, char**)>> funcMap;

#endif //ANLINXOS_FUNCHEADER_HPP
