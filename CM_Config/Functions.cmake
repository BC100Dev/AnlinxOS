function(CollectSources RootDir OutVar)
    set(SEARCH_PATTERNS
            "${RootDir}/*.cpp"
            "${RootDir}/*.hpp"
            "${RootDir}/*.c"
            "${RootDir}/*.h"
            "${RootDir}/*.cc"
            "${RootDir}/*.cuh"
            "${RootDir}/**/*.cpp"
            "${RootDir}/**/*.hpp"
            "${RootDir}/**/*.c"
            "${RootDir}/**/*.h"
            "${RootDir}/**/*.cc"
            "${RootDir}/**/*.cuh")

    file(GLOB_RECURSE SOURCE_FILES ${SEARCH_PATTERNS})
    set(${OutVar} ${SOURCE_FILES} PARENT_SCOPE)
endfunction()

function(GetCompilerThreadCount VAR)
    ProcessorCount(NPROC)
    set(${VAR} ${NPROC} PARENT_SCOPE)
endfunction()

function(SetTargetOutputDir TARGET DIR)
    set_target_properties(${TARGET} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY "${DIR}"
            LIBRARY_OUTPUT_DIRECTORY "${DIR}"
            ARCHIVE_OUTPUT_DIRECTORY "${DIR}"
    )
endfunction()

function(SetTargetFilename TARGET NAME)
    set_target_properties(${TARGET} PROPERTIES
            OUTPUT_NAME "${NAME}"
    )
endfunction()

function(Symlink TARGET_NAME SOURCE_FILE SYMLINK_NAME SYMLINK_DIR)
    if (NOT EXISTS "${SYMLINK_DIR}")
        file(MAKE_DIRECTORY "${SYMLINK_DIR}")
    endif ()

    set(_symlink_path "${SYMLINK_DIR}/${SYMLINK_NAME}")

    add_custom_command(
            TARGET ${TARGET_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${SOURCE_FILE}"
            "${_symlink_path}"
            COMMENT "Symlink ${SYMLINK_NAME} -> ${SOURCE_FILE}"
            VERBATIM
    )
endfunction()

function(SymlinkTarget TARGET NEW_TARGET LINK_PATH)
    if (NOT EXISTS "${LINK_PATH}")
        file(MAKE_DIRECTORY "${LINK_PATH}")
    endif ()

    add_custom_command(TARGET ${TARGET}
            POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink $<TARGET_FILE:${TARGET}> "${LINK_PATH}/${NEW_TARGET}"
            COMMENT "Symlinking ${NEW_TARGET} -> $<TARGET_FILE:${TARGET}>")
endfunction()

function(CopyTarget TARGET COPY_NAME COPY_DIR)
    file(MAKE_DIRECTORY "${COPY_DIR}")

    add_custom_command(
            TARGET ${TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${TARGET}>
            "${COPY_DIR}/${COPY_NAME}"
            COMMENT "Copying $<TARGET_FILE:${TARGET}> -> ${COPY_DIR}/${COPY_NAME}"
    )
endfunction()