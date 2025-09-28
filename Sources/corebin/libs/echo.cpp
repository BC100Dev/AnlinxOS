#include "FuncHeader.hpp"

#include <iostream>
#include <sstream>

bool echo_suddenCutoff = false;

void echo_help() {
}

void echo_version() {
}

std::string echo_parseInput(const std::stringstream& input) {
    std::string str = input.str();
    std::string out;

    for (size_t i = 0; i < str.size(); i++) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            i++;
            switch (str[i]) {
                case 'n':
                    out += '\n';
                    break;
                case 't':
                    out += '\t';
                    break;
                case 'r':
                    out += '\r';
                    break;
                case '\\':
                    out += '\\';
                    break;
                case 'c':
                    echo_suddenCutoff = true;
                    return out;
                case 'v':
                    out += '\v';
                    break;
                case 'f':
                    out += '\f';
                    break;
                case 'e':
                    out += '\x1B';
                    break;
                case 'b':
                    out += '\b';
                    break;
                case 'a':
                    out += '\a';
                    break;
                case '0': {
                    int val = 0, digits = 0;
                    while (digits < 3 && i + 1 < str.size() && str[i + 1] >= '0' && str[i + 1] <= '7') {
                        val = (val << 3) | (str[++i] - '0');
                        digits++;
                    }

                    out += static_cast<char>(val);
                    break;
                }
                case 'x': {
                    int val = 0, digits = 0;
                    while (i + 1 < str.size() && isxdigit(str[i + 1])) {
                        char c = str[++i];
                        val = (val << 4) | (isdigit(c) ? (c - '0') : (tolower(c) - 'a' + 10));
                        digits++;
                    }
                    if (digits > 0)
                        out += static_cast<char>(val);
                    break;
                }
                default:
                    out += '\\';
                    break;
            }
        } else
            out += str[i];
    }

    return out;
}

int echo_func(int argc, char** argv, char** env) {
    bool newline = true;
    bool interpretBackslash = false;

    if (argc == 1) {
        std::cout << std::endl;
        return 0;
    }

    std::stringstream input;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg[0] == '-') {
            if (arg == "--help") {
                echo_help();
                return 0;
            } else if (arg == "--version") {
                echo_version();
                return 0;
            } else if (arg == "-n") {
                newline = false;
                continue;
            } else if (arg == "-e") {
                interpretBackslash = true;
                continue;
            } else if (arg == "-E") {
                interpretBackslash = false;
                continue;
            } else {
                if (input.str().empty())
                    input << arg;
                else
                    input << " " << arg;
            }
        } else {
            if (input.str().empty())
                input << arg;
            else
                input << " " << arg;
        }
    }

    std::string result = interpretBackslash ? echo_parseInput(input) : input.str();
    std::cout << result;
    if (newline) {
        if (!(interpretBackslash && echo_suddenCutoff))
            std::cout << std::endl;
    }

    return 0;
}
