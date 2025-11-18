CollectSources("${PROJECT_MODULE_ROOT}/initfs_main" InitfsSources)
CollectSources("${PROJECT_MODULE_ROOT}/ServiceManager" SVCMGR_Sources)
CollectSources("${PROJECT_MODULE_ROOT}/root_main" RootInitSources)

add_executable(initramfs-init ${InitfsSources})
target_link_libraries(initramfs-init PRIVATE alx64-static)
target_link_options(initramfs-init PRIVATE -static)
SetTargetFilename(initramfs-init "init")
SetTargetOutputDir(initramfs-init "${OUTPUT_DIRECTORY_I_BOOT}")