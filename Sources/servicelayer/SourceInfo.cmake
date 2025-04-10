CollectSources("${PROJECT_MODULE_ROOT}/init" InitSources)
CollectSources("${PROJECT_MODULE_ROOT}/ServiceManager" SVCMGR_Sources)

add_executable(init ${InitSources})

# !!! REQUIRED !!!
# since the OS doesn't have a libc currently, we need to statically link this p.o.s.
target_link_options(init PRIVATE -static)