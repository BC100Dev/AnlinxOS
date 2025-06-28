# A copy of the miniz library
# refer to https://github.com/richgel999/miniz for documentation

CollectSources(${PROJECT_MODULE_ROOT} MINIZ_SRC)
add_library(miniz STATIC ${MINIZ_SRC})