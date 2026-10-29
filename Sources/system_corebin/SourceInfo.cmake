# sorry guys for even using AI over here
# its 3 AM over here, when I was finishing things up
# ill rewrite them, I promise
# also, despite what Claude wrote, I will still be handing things the other way around.

set(ALXBOX_NAMES
        # pkgmgr — Main package manager frontend; handles .apkg installs, queries, and removals.
        # Routes package operations through the central package manager daemon.
        pkgmgr

        # sysupdate — Handles .apsd system update packages exclusively.
        # Enforces signature verification as a hard requirement; intentionally separate
        # from pkgmgr to make it clear this is not a casual package operation.
        sysupdate

        # svcmgctl — Control interface for svcmg (Service Manager).
        # Manages firmware loader registration, lists active dynamic callbacks,
        # queries registered firmware paths, and triggers re-scans.
        svcmgctl

        # sysmanctl — Query and control interface for the sysmgr daemon.
        # Lists active admin sessions, revokes sessions, and inspects per-user
        # sysmgr instance state. One sysmgr instance runs per user, plus system and uadmin.
        sysmanctl

        # permctl — Dedicated runtime permission management tool.
        # Queries, grants, and revokes RequestRuntimePermission() grants for apps and processes.
        # Write operations require an active admin session via cmdreq.
        permctl

        # drvctl — Hardware and driver package manager.
        # Manages .aphd and .ddc driver packages; handles DDC loading on first boot.
        # Enforces that DDC files originate from system, vendor, or OEM paths only — not userdata.
        # DDC = Device Driver Controller
        drvctl

        # nethostctl — Read/write interface for /Data/config/system/nethosts.cfg.
        # Read access is open (file is world-readable); writes are brokered through sysmgr IPC.
        nethostctl

        # netinfo — Read-only query tool for active network state.
        # Reports interfaces, addresses, and routes. No elevated privileges required for basic queries.
        netinfo

        # bootcontrl — Runtime inspection and control tool for the NLBoot chain.
        # Queries the active boot environment (normal, managed, recovery, live),
        # reads nlboot-boot.conf entries, and signals a reboot into a target environment
        # with appropriate privilege.
        bootcontrl

        # bootenv — Low-level next-boot environment flag setter.
        # Called by bootcontrl internally; also available standalone for scripted maintenance.
        # Sets which NLBoot environment image is selected on next boot.
        bootenv

        # avscan — Manual invocation of the pre-execution AV scanner.
        # Runs outside the automatic run-before-execute path; returns a verdict for a given binary.
        # Useful for admin review and validation pipelines before whitelisting new binaries.
        avscan

        # scmctl — System Component Module lifecycle management tool.
        # Mounts and unmounts .scmi modules, queries active modules,
        # and forces module reloads without rebooting.
        scmctl

        # fmwrmgr — Control interface for the mdld (Module Loader) daemon.
        # Queries the runtime firmware cache at /Cache/runtime/mdld/firmware,
        # triggers cache invalidation, and lists registered firmware blobs with their priority-chain sources.
        fmwrmgr

        # pathq — AnlinxOS-aware path resolution tool.
        # Resolves logical AnlinxOS paths (e.g. user config for a given uid) without requiring
        # callers to know the /Data/userconfig/[uid] layout directly. Intended for scripts and tooling.
        pathq

        # alxinfo — System identity and state reporting tool.
        # Reports AnlinxOS version, build fingerprint, active boot mode, kernel version,
        # and device profile (mobile vs desktop). AnlinxOS-native equivalent of uname.
        alxinfo

        # codecinfo — Codec resolution chain inspection tool.
        # Queries libmediacodecresolv.so for a given file or codec name; shows which path wins
        # across /Data/media/codecs, /System/core/media/codecs, and /vendor/core/media/codecs,
        # and the resolved library path.
        codecinfo

        # usrmgr — User account lifecycle management tool.
        # Creates and removes users, queries /Data/userdata/[uid] layout, and manages per-user config.
        # Wraps the user lifecycle owned by sysmgr. Not a drop-in for useradd — path conventions differ.
        usrmgr

        # cmdreq — Privilege elevation tool; safer alternative to sudo.
        # Does not drop to a raw root shell; IPC-requests sysmgr to open an admin session instead.
        # All cmdreq calls are logged by sysmgr. The sole sanctioned escalation path on AnlinxOS.
        cmdreq
)

# 1 - Build the stuff first
CollectSources("${PROJECT_MODULE_ROOT}" SystemBinSources)
add_executable(alxbox ${SystemBinSources})
target_link_options(alxbox PRIVATE -static)
SetTargetOutputDir(alxbox ${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX})
foreach (ALXBOX_NAME ${ALXBOX_NAMES})
    SymlinkTarget(alxbox "alxbox" ${ALXBOX_NAME} "${OUTPUT_DIRECTORY_SYSTEM}/${BINARY_PREFIX}")
endforeach ()

# 2 - Copy over the core binaries to recovery
CopyTarget(alxbox alxbox ${OUTPUT_DIRECTORY_RECVRY}/${BINARY_PREFIX})
foreach (ALXBOX_NAME ${ALXBOX_NAMES})
    SymlinkTarget(alxbox "alxbox" ${ALXBOX_NAME} "${OUTPUT_DIRECTORY_RECVRY}/${BINARY_PREFIX}")
endforeach ()

# 3 - Copy over the core binaries to initramfs
CopyTarget(alxbox alxbox ${OUTPUT_DIRECTORY_INITBL}/${BINARY_PREFIX})
foreach (ALXBOX_NAME ${ALXBOX_NAMES})
    SymlinkTarget(alxbox "alxbox" ${ALXBOX_NAME} "${OUTPUT_DIRECTORY_INITBL}/${BINARY_PREFIX}")
endforeach ()