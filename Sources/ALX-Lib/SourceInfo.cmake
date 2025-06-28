CollectSources("${PROJECT_MODULE_ROOT}/Shared" SHARED_CODE)
CollectSources("${PROJECT_MODULE_ROOT}/Lib32" L32_CODE)
CollectSources("${PROJECT_MODULE_ROOT}/Lib64" L64_CODE)

add_library(alx-shared SHARED ${SHARED_CODE})
SetTargetOutputDir(alx-shared "${OUTPUT_DIRECTORY_SYSTEM}/x64")
SetTargetFilename(alx-shared "alx")
target_link_libraries(alx-shared PRIVATE os-shared_src-shared)

add_library(alx-static STATIC ${SHARED_CODE})
SetTargetOutputDir(alx-static "${OUTPUT_DIRECTORY_SYSTEM}/x64")
SetTargetFilename(alx-static "alx")
target_link_libraries(alx-static PRIVATE os-shared_src-static)