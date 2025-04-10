# we make this one mainly optional.
# it's not really a requirement, but it's good to at least have some userland binaries

set(BUSYBOX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External/busybox")

if (NOT EXISTS "${BUSYBOX_SOURCE_DIR}")
    message(STATUS "BusyBox directory does not exist")
    return()
endif ()

if (NOT EXISTS "${BUSYBOX_SOURCE_DIR}/Makefile")
    message(CHECK_FAIL "BusyBox does not have a Makefile, therefore cannot build")
    return()
endif ()

if (NOT DEFINED BUSYBOX_COMPILE_FLAGS)
    set(BUSYBOX_COMPILE_FLAGS "")
endif ()

if (DEFINED LINUX_CROSS_COMPILE AND LINUX_CROSS_COMPILE EQUAL ON)
    if (DEFINED LINUX_COMPILER_ARCH)
        list(APPEND BUSYBOX_COMPILE_FLAGS "ARCH=${LINUX_COMPILER_ARCH}")
    endif ()
    if (DEFINED LINUX_COMPILER_PREFIX)
        list(APPEND BUSYBOX_COMPILE_FLAGS "CROSS_COMPILE=${LINUX_COMPILER_PREFIX}")
    endif ()
endif ()

if (DEFINED COMPILER_THREADS)
    if (COMPILER_THREADS STREQUAL "all")
        GetCompilerThreadCount(NPROC)
        list(APPEND BUSYBOX_COMPILE_FLAGS "-j${NPROC}")
    else ()
        list(APPEND BUSYBOX_COMPILE_FLAGS "-j${COMPILER_THREADS}")
    endif ()
endif ()

add_custom_target(busybox
        COMMAND make -C "${BUSYBOX_SOURCE_DIR}" ${BUSYBOX_COMPILE_FLAGS}
        WORKING_DIRECTORY "${BUSYBOX_SOURCE_DIR}"
)