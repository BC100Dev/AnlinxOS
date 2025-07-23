CollectSources(${PROJECT_MODULE_ROOT} TestSources)
add_executable(Test-KernelModuleInsertion ${TestSources})
target_link_options(Test-KernelModuleInsertion PRIVATE -static)
target_link_libraries(Test-KernelModuleInsertion PRIVATE alx-static)

add_test(NAME KernelModuleInsertion COMMAND Test-KernelModuleInsertion)