set(KERNEL_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/linux-kernel")

if (NOT EXISTS "${KERNEL_SOURCE_DIR}")
    # then ignore it, we don't want to make a target that doesn't exist
    return()
endif ()

if (NOT EXISTS "${KERNEL_SOURCE_DIR}/Makefile")
    message(CHECK_FAIL "Makefile for the Linux kernel is missing")
    return()
endif ()

if (NOT DEFINED KERNEL_COMPILE_FLAGS)
    set(KERNEL_COMPILE_FLAGS "")
endif ()

if (DEFINED LINUX_CROSS_COMPILE AND LINUX_CROSS_COMPILE EQUAL ON)
    if (DEFINED LINUX_COMPILER_ARCH)
        list(APPEND KERNEL_COMPILE_FLAGS "ARCH=${LINUX_COMPILER_ARCH}")
    endif ()

    if (DEFINED LINUX_COMPILER_PREFIX)
        list(APPEND KERNEL_COMPILE_FLAGS "CROSS_COMPILE=${LINUX_COMPILER_PREFIX}")
    endif ()
endif ()

if (DEFINED COMPILER_THREADS)
    if (COMPILER_THREADS STREQUAL "all")
        GetCompilerThreadCount(NPROC)
        list(APPEND KERNEL_COMPILE_FLAGS "-j${NPROC}")
    else ()
        list(APPEND KERNEL_COMPILE_FLAGS "-j${COMPILER_THREADS}")
    endif ()
endif ()

message(STATUS "Sourcing up the Linux kernel")
add_custom_target(linux-defconfig
        COMMAND make -C "${KERNEL_SOURCE_DIR}" ${KERNEL_COMPILE_FLAGS} defconfig
        WORKING_DIRECTORY "${KERNEL_SOURCE_DIR}")

add_custom_target(linux
        COMMAND make -C "${KERNEL_SOURCE_DIR}" ${KERNEL_COMPILE_FLAGS}
        WORKING_DIRECTORY "${KERNEL_SOURCE_DIR}")