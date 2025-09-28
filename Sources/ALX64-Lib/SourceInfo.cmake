CollectSources("${PROJECT_MODULE_ROOT}" ALX64_SRC)

add_library(alx-shared SHARED ${ALX64_SRC})
SetTargetOutputDir(alx-shared "${OUTPUT_DIRECTORY_SYSTEM}/x64")
SetTargetFilename(alx-shared "alx")
target_link_libraries(alx-shared PRIVATE os-shared_src-shared)

add_library(alx-static STATIC ${ALX64_SRC})
SetTargetOutputDir(alx-static "${OUTPUT_DIRECTORY_SYSTEM}/x64")
SetTargetFilename(alx-static "alx")
target_link_libraries(alx-static PRIVATE os-shared_src-static)