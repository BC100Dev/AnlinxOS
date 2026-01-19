# Test: check and see, if miniz is working properly,
# along with whether we can properly read the configuration file

CollectSources(${PROJECT_MODULE_ROOT} TestSources)
add_executable(Test-ConfigGZ ${TestSources})
SetTargetOutputDir(Test-ConfigGZ "${OUTPUT_DIRECTORY_TESTS}")
SetTargetFilename(Test-ConfigGZ "kernel-config-extract")
target_link_libraries(Test-ConfigGZ PRIVATE alx64-static)
target_link_options(Test-ConfigGZ PRIVATE -static)

add_test(NAME ConfigGZ COMMAND Test-ConfigGZ)