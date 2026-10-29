CollectSources("${PROJECT_MODULE_ROOT}/src/libm" MathSources)
CollectSources("${PROJECT_MODULE_ROOT}/src/libc" ClibSources)
CollectSources("${PROJECT_MODULE_ROOT}/include" ClibIncludes)

add_custom_target(libm ALL
        COMMAND $(MAKE) -f ${PROJECT_MODULE_ROOT}/src/libm/Makefile
        SRC=${PROJECT_MODULE_ROOT}/src/libm
        INCLUDE=${PROJECT_MODULE_ROOT}/include
        OUT=${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}
        CC=${CMAKE_C_COMPILER}

        COMMENT "Building libm"
        SOURCES ${MathSources} ${PROJECT_MODULE_ROOT}/include/math.h)

add_library(libm_includes INTERFACE)
target_include_directories(libm_includes INTERFACE ${PROJECT_MODULE_ROOT}/include)

add_custom_target(libc ALL
        COMMAND $(MAKE) -f ${PROJECT_MODULE_ROOT}/src/libc/Makefile
        SRC=${PROJECT_MODULE_ROOT}/src/libc
        INCLUDE=${PROJECT_MODULE_ROOT}/include
        OUT=${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}
        CC=${CMAKE_C_COMPILER}

        COMMENT "Building libc"
        SOURCES ${MathSources} ${ClibIncludes})

add_library(libc_includes INTERFACE)
target_include_directories(libc_includes INTERFACE ${PROJECT_MODULE_ROOT}/include)

if (CMAKE_EXPORT_COMPILE_COMMANDS)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -print-file-name=include
            OUTPUT_VARIABLE COMPILER_INCLUDE_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE)

    add_library(libm_ide STATIC EXCLUDE_FROM_ALL ${MathSources})
    target_include_directories(libm_ide PRIVATE ${PROJECT_MODULE_ROOT}/include)
    target_compile_options(libm_ide PRIVATE -nostdlib -nostdinc -isystem ${COMPILER_INCLUDE_DIR})

    add_library(libc_ide STATIC EXCLUDE_FROM_ALL ${ClibSources})
    target_include_directories(libc_ide PRIVATE ${PROJECT_MODULE_ROOT}/include)
    target_compile_options(libc_ide PRIVATE -nostdlib -nostdinc -isystem ${COMPILER_INCLUDE_DIR})
endif ()

# copy C lib and math lib outside of system to recovery and initramfs
CopyFile("${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}/libm.so"
        "libm.so"
        "${OUTPUT_DIRECTORY_RECVRY}/${BINARY_PREFIX}"
        libm)
CopyFile("${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}/libm.so"
        "libm.so"
        "${OUTPUT_DIRECTORY_INITBL}/${BINARY_PREFIX}"
        libm)

CopyFile("${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}/libc.so"
        "libc.so"
        "${OUTPUT_DIRECTORY_RECVRY}/${BINARY_PREFIX}"
        libc)
CopyFile("${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}/libc.so"
        "libc.so"
        "${OUTPUT_DIRECTORY_INITBL}/${BINARY_PREFIX}"
        libc)

# perform symlinks for libdl compatibility
SymlinkTarget(libc "libc.so" "libc.so.6" "${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}")
SymlinkTarget(libc "libc.so" "libc.so.6" "${OUTPUT_DIRECTORY_RECVRY}/${BINARY_PREFIX}")
SymlinkTarget(libc "libc.so" "libc.so.6" "${OUTPUT_DIRECTORY_INITBL}/${BINARY_PREFIX}")

SymlinkTarget(libc "libc.so" "libdl.so" "${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}")
SymlinkTarget(libc "libc.so" "libdl.so" "${OUTPUT_DIRECTORY_RECVRY}/${BINARY_PREFIX}")
SymlinkTarget(libc "libc.so" "libdl.so" "${OUTPUT_DIRECTORY_INITBL}/${BINARY_PREFIX}")

SymlinkTarget(libc "libc.so" "libdl.so.2" "${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}")
SymlinkTarget(libc "libc.so" "libdl.so.2" "${OUTPUT_DIRECTORY_RECVRY}/${BINARY_PREFIX}")
SymlinkTarget(libc "libc.so" "libdl.so.2" "${OUTPUT_DIRECTORY_INITBL}/${BINARY_PREFIX}")

SymlinkTarget(libc "libc.so" "libpthread.so" "${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}")
SymlinkTarget(libc "libc.so" "libpthread.so" "${OUTPUT_DIRECTORY_RECVRY}/${BINARY_PREFIX}")
SymlinkTarget(libc "libc.so" "libpthread.so" "${OUTPUT_DIRECTORY_INITBL}/${BINARY_PREFIX}")

SymlinkTarget(libc "libc.so" "libpthread.so.0" "${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}")
SymlinkTarget(libc "libc.so" "libpthread.so.0" "${OUTPUT_DIRECTORY_RECVRY}/${BINARY_PREFIX}")
SymlinkTarget(libc "libc.so" "libpthread.so.0" "${OUTPUT_DIRECTORY_INITBL}/${BINARY_PREFIX}")