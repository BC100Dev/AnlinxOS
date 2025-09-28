#include "FuncHeader.hpp"

#include <iostream>
#include <sstream>

void yes_spam(const std::string &input) {
    while (true) {
        // who even sees limits, it spams either way, until stopped
        std::cout << input << std::endl;
    }
}

void yes_usage(const std::string &a0) {
    std::cout << "Usage: " << a0 << " [STRING]..." << std::endl;
    std::cout << "  or:  " << a0 << " OPTION" << std::endl;
    std::cout << "Repeatedly output a line with all specified STRING(s), or 'y'." << std::endl << std::endl;
    std::cout << "      --help        display this help and exit" << std::endl;
    std::cout << "      --version     output version information and exit" << std::endl << std::endl;
    std::cout << "AnlinxOS coreutils online help: https://github.com/BC100Dev/AnlinxOS" << std::endl;
    std::cout << "see the README documentation for coreutils for further help" << std::endl;
}

void yes_version() {
    std::cout << "yes (ALX coreutils) 1.0" << std::endl;
    std::cout << "Copyright (C) 2025 BC100Dev" << std::endl;
    std::cout << "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n"
                 "This is free software: you are free to change and redistribute it.\n"
                 "There is NO WARRANTY, to the extent permitted by law." << std::endl << std::endl;
    std::cout << "Written and developed by BC100Dev" << std::endl;
}

int yes_func(int argc, char **argv, char **env) {
    if (argc == 1) {
        yes_spam("y");
        return 0;
    }

    std::stringstream input;
    for (int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if (arg[0] == '-') {
            if (arg == "--help" || arg == "-h") {
                yes_usage(argv[0]);
                return 0;
            }
            if (arg == "--version" || arg == "-v") {
                yes_version();
                return 0;
            }
        } else {
            if (input.str().empty())
                input << arg;
            else
                input << " " << arg;
        }
    }

    yes_spam(input.str());
    return 0;
}
