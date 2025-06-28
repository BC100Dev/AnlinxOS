CollectSources("${PROJECT_MODULE_ROOT}" SharedSources)

add_library(os-shared_src-static STATIC ${SharedSources})
SetTargetOutputDir(os-shared_src-static "${OUTPUT_DIRECTORY_SYSTEM}/x64")
SetTargetFilename(os-shared_src-static "anlinx-shared")

add_library(os-shared_src-shared SHARED ${SharedSources})
SetTargetOutputDir(os-shared_src-shared "${OUTPUT_DIRECTORY_SYSTEM}/x64")
SetTargetFilename(os-shared_src-shared "anlinx-shared")