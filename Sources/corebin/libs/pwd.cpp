#include "FuncHeader.hpp"

int pwd_func(int argc, char** argv, char** env) {
    /*
     * accepted args:
     * -L / --logical: use PWD from environment, even if it contains symlinks
     * -P / --physical: resolve all symlinks
     * --help: display this help and exit
     * --version: output version information and exit
     *
     * source: "man pwd"
     */
    return 0;
}
