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

function(SymlinkTarget TARGET_NAME SOURCE_FILE SYMLINK_NAME SYMLINK_DIR)
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

function(Symlink TARGET SRC_BASENAME NEW_TARGET LINK_PATH)
    if (NOT EXISTS "${LINK_PATH}")
        file(MAKE_DIRECTORY "${LINK_PATH}")
    endif ()

    add_custom_command(
            TARGET ${TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${SRC_BASENAME}"
            "${LINK_PATH}/${NEW_TARGET}"
            VERBATIM
            COMMENT "Symlinking ${NEW_TARGET} -> ${SRC_BASENAME}"
    )
endfunction()

function(CopyTarget TARGET COPY_NAME COPY_DIR)
    file(MAKE_DIRECTORY "${COPY_DIR}")

    set(COPYING_TARGET_SRC "$<TARGET_FILE:${TARGET}>")

    add_custom_command(TARGET ${TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${COPYING_TARGET_SRC} "${COPY_DIR}/${COPY_NAME}"
            COMMENT "Copying ${COPYING_TARGET_SRC} -> ${COPY_DIR}/${COPY_NAME}")
endfunction()

function(CopyFile FILE_PATH COPY_NAME COPY_DIR DEPENDS_ON)
    file(MAKE_DIRECTORY "${COPY_DIR}")
    add_custom_command(OUTPUT "${COPY_DIR}/${COPY_NAME}"
            COMMAND ${CMAKE_COMMAND} -E copy "${FILE_PATH}" "${COPY_DIR}/${COPY_NAME}"
            DEPENDS "${FILE_PATH}"
            COMMENT "Copying ${FILE_PATH} -> ${COPY_DIR}/${COPY_NAME}"
    )

    string(MD5 _hash "${COPY_DIR}/${COPY_NAME}")
    string(MAKE_C_IDENTIFIER "${COPY_NAME}" _name)
    add_custom_target(copy_${_name}_${_hash} ALL
            DEPENDS "${COPY_DIR}/${COPY_NAME}")

    if(DEPENDS_ON)
        add_dependencies(copy_${_name}_${_hash} ${DEPENDS_ON})
    endif()
endfunction()

function(CopyExternalTarget TARGET COPY_NAME COPY_DIR)
    file(MAKE_DIRECTORY "${COPY_DIR}")

    # Create a reproducible, short hash based on the copy directory
    string(SHA1 HASH "${COPY_DIR}")
    string(SUBSTRING "${HASH}" 0 8 HASH8)

    set(UNIQUE_NAME copy_external_${TARGET}_${COPY_NAME}_${HASH8})

    add_custom_target(
            ${UNIQUE_NAME} ALL
            COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${TARGET}>
            "${COPY_DIR}/${COPY_NAME}"
            DEPENDS ${TARGET}
            VERBATIM
    )
endfunction()