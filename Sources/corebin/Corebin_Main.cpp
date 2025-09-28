#include <iostream>
#include <vector>
#include <cstring>

#include "libs/FuncHeader.hpp"

struct Executor {
    int argc;
    char **argv;
    char **env;
};

Executor makeExecutor(int argc, char **argv, char **env) {
    std::vector<std::string> v;

    for (int i = 0; i < argc; i++)
        v.emplace_back(argv[i]);

    char **c_argv = new char *[v.size() + 1];

    for (size_t i = 0; i < v.size(); ++i) {
        c_argv[i] = new char[v[i].size() + 1];
        std::memcpy(c_argv[i], v[i].c_str(), v[i].size() + 1);
    }
    c_argv[v.size()] = nullptr;

    Executor e{};
    e.argc = v.size();
    e.argv = c_argv;
    e.env = env;

    return e;
}

void freeExec(Executor &e) {
    for (int i = 0; i < e.argc; i++)
        delete[] e.argv[i];

    delete[] e.argv;
}

int main(int argc, char **argv, char **env) {
    auto fnd = funcMap.find(argv[0]);
    if (fnd != funcMap.end())
        return fnd->second(argc, argv, env);

    if (argc == 1) {
        std::cout << "AnlinxOS Core Binaries:" << std::endl;
        std::cout << "-----------------------" << std::endl;

        int c = 0;
        int m = 8;
        for (const auto &it: funcMap) {
            std::cout << it.first << " ";
            c++;

            if (c == m) {
                std::cout << std::endl;
                c = 0;
            }
        }

        if (c != m)
            std::cout << std::endl;

        return 0;
    }

    fnd = funcMap.find(argv[1]);
    if (fnd != funcMap.end()) {
        Executor exec = makeExecutor(argc - 1, argv + 1, env);
        int ret = fnd->second(exec.argc, exec.argv, exec.env);
        freeExec(exec);
        return ret;
    }

    std::cerr << "Unknown command: " << argv[1] << std::endl;
    return 1;
}