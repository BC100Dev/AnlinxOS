#!/bin/bash

# have you ever heard of copy-pasting every single thing?
# no?
# then let me cook up a quick shell script that auto-generates these little shits for the fucking corebin
# otherwise, if I weren't be doing this, I would have already cried-

SHELL_DIR="$(cd -- "$(dirname -- "$(readlink -f -- "${BASH_SOURCE[0]}")")" && pwd)"

if [ ! -d "$SHELL_DIR/handler" ]; then
    mkdir -p "$SHELL_DIR/handler"
fi

echo -e "#ifndef ANLINXOS_FUNCHEADER_HPP\n#define ANLINXOS_FUNCHEADER_HPP\n\n#include <map>\n#include <string>\n#include <functional>\n" > "${SHELL_DIR}/FuncHeader.hpp"

for i in "$@"; do
    # before I start crying over lost work, I HAD to implement that check
    if [ ! -f "${SHELL_DIR}/$i.cpp" ]; then
	    echo -e "#include \"FuncHeader.hpp\"\n\nint ${i}_func(int argc, char** argv, char** env) {\n    return 0;\n}" > "${SHELL_DIR}/$i.cpp"
	fi

	echo -e "int ${i}_func(int, char**, char**);" >> "${SHELL_DIR}/FuncHeader.hpp"
done

echo -e "\nextern std::map<std::string, std::function<int(int, char**, char**)>> funcMap;\n\n#endif //ANLINXOS_FUNCHEADER_HPP" >> "${SHELL_DIR}/FuncHeader.hpp"

echo -e "#include \"../FuncHeader.hpp\"\n\nstd::map<std::string, std::function<int(int, char**, char**)>> funcMap = {" > "$SHELL_DIR/handler/FuncHeader.cpp"

for i in "$@"; do
    echo -e "    { \"$i\", ${i}_func }," >> "$SHELL_DIR/handler/FuncHeader.cpp"
done

echo -e "};" >> "$SHELL_DIR/handler/FuncHeader.cpp"