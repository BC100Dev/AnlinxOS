#ifndef ALX64_UTILITIES_HPP
#define ALX64_UTILITIES_HPP

#include <string>
#include <vector>

namespace ALX64::Utils {

    std::vector<std::string> TranslateStringToCommandline(const std::string &cmdLine);

    std::string TrimString(const std::string& str);

}

#endif //ALX64_UTILITIES_HPP
