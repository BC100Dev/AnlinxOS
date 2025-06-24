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