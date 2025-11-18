#include <ALX64/Utilities.hpp>

#include <sstream>
#include <stdexcept>

namespace ALX64::Utils {

    std::vector<std::string> TranslateStringToCommandline(const std::string &cmdLine) {
        if (cmdLine.empty() || cmdLine.find_first_not_of(' ') == std::string::npos)
            return {};

        const int normal = 0;
        const int inQuote = 1;
        const int inDoubleQuote = 2;
        int state = normal;

        std::vector<std::string> result;
        std::stringstream current;
        bool lastTokenHasBeenQuoted = false;

        for (char nextTok: cmdLine) {
            switch (state) {
                case inQuote:
                    if (nextTok == '\'') {
                        lastTokenHasBeenQuoted = true;
                        state = normal;
                    } else
                        current << nextTok;

                    break;

                case inDoubleQuote:
                    if (nextTok == '"') {
                        lastTokenHasBeenQuoted = true;
                        state = normal;
                    } else
                        current << nextTok;

                    break;

                default:
                    if (nextTok == '\'')
                        state = inQuote;
                    else if (nextTok == '"')
                        state = inDoubleQuote;
                    else if (nextTok == ' ') {
                        if (lastTokenHasBeenQuoted || current.tellp() > 0) {
                            result.push_back(current.str());
                            current.str("");
                            current.clear();
                        }
                    } else
                        current << nextTok;

                    lastTokenHasBeenQuoted = false;
                    break;
            }
        }

        if (lastTokenHasBeenQuoted || current.tellp() > 0)
            result.push_back(current.str());

        if (state == inQuote || state == inDoubleQuote)
            throw std::runtime_error("unbalanced quotes in " + cmdLine);

        return result;
    }

    std::string TrimString(const std::string &str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        size_t end = str.find_last_not_of(" \t\r\n");
        return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

}