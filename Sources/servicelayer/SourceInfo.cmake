CollectSources("${PROJECT_MODULE_ROOT}/init" InitSources)
CollectSources("${PROJECT_MODULE_ROOT}/ServiceManager" SVCMGR_Sources)

add_executable(init ${InitSources})