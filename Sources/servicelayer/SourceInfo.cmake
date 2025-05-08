CollectSources("${PROJECT_MODULE_ROOT}/initfs_main" InitfsSources)
CollectSources("${PROJECT_MODULE_ROOT}/ServiceManager" SVCMGR_Sources)
CollectSources("${PROJECT_MODULE_ROOT}/root_main" RootInitSources)

add_executable(init ${InitfsSources})
target_link_libraries(init PRIVATE AnlinOS-SharedSources)

# !!! REQUIRED !!!
# since the OS doesn't have a libc currently, we need to statically link this p.o.s.
target_link_options(init PRIVATE -static)