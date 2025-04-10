# To make sure that we do not clutter everything within the "Builds/(architecture)" directory,
# we want to have all targets be placed within "Builds/(architecture)/BundleBuild"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/BundleBuild")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/BundleBuild")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/BundleBuild")