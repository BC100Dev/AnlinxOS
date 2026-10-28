CollectSources("${PROJECT_MODULE_ROOT}" ALX64_SRC)

add_library(alx64-shared SHARED ${ALX64_SRC})
SetTargetOutputDir(alx64-shared "${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}")
SetTargetFilename(alx64-shared "alx")
target_compile_options(alx64-shared PRIVATE -static)

add_library(alx64-static STATIC ${ALX64_SRC})
SetTargetOutputDir(alx64-static "${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}")
SetTargetFilename(alx64-static "alx")