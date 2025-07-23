# Test: check and see, if miniz is working properly,
# along with whether we can properly read the configuration file

CollectSources(${PROJECT_MODULE_ROOT} TestSources)
add_executable(Test-ConfigGZ ${TestSources})
target_link_libraries(Test-ConfigGZ PRIVATE miniz)

add_test(NAME ConfigGZ COMMAND Test-ConfigGZ)