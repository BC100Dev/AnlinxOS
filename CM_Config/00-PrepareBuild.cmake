# To make sure that we do not clutter everything within the "Builds/(architecture)" directory,
# we want to have all targets be placed within "Builds/(architecture)/BundleBuild"
set(OUTPUT_DIRECTORY_ROOT "${CMAKE_BINARY_DIR}/BundleBuild")
set(OUTPUT_DIRECTORY_INITFS "${OUTPUT_DIRECTORY_ROOT}/initramfs")
set(OUTPUT_DIRECTORY_SYSTEM "${OUTPUT_DIRECTORY_ROOT}/root")
set(OUTPUT_DIRECTORY_RECVRY "${OUTPUT_DIRECTORY_ROOT}/recovery")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY_ROOT}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY_ROOT}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY_ROOT}")

include(ProcessorCount)