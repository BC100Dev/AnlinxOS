## The core binaries (cp, ls ...)
## this will also deliver custom ones, like "cmdreq" replacing "su", "sudo" and "doas"

# cmdreq: alternative to sudo and su (comms via IPC)
set(COREBIN_NAMES
        arch base32 base64 basename basenc cat chcon chgrp chmod chown
        chroot chvt cksum comm cp csplit cut date dd df dir dircolors
        dmesg dorname du echo env expand expr factor false fmt fold free
        groupadd groupdel groupmod groups head help hostid htop id insmod
        install join last lastlog link ln logname less ls kill md5sum mkdir
        mkfifo mknod mktemp mv modprobe more mount nice nl nohup nproc numfmt od
        paste pathchk pinky pr printenv printf ps ptx pwd readlink realpath reset
        rm rmdir rmmod runcon seq sha1sum sha224sum sha256sum sha384sum sha512sum
        shred sleep sort split stat stdbuf stty sum swapon swapoff sync tac tail
        tee test timeout touch tr true truncate tsort tty umount uname useradd
        userdel usermod users unexpand uniq unlink uptime users vdir w watch wc
        who whoami yes
)

# 1 - Build the stuff first
CollectSources("${PROJECT_MODULE_ROOT}" Corebin_Sources)
add_executable(corebin ${Corebin_Sources})
target_link_options(corebin PRIVATE -static)
SetTargetOutputDir(corebin ${OUTPUT_DIRECTORY_SYSTEM}/x64)
foreach (COREBIN_NAME ${COREBIN_NAMES})
    SymlinkTarget(corebin ${COREBIN_NAME} "${OUTPUT_DIRECTORY_SYSTEM}/x64")
endforeach ()

# 2 - Copy over the core binaries to recovery
CopyTarget(corebin corebin ${OUTPUT_DIRECTORY_RECVRY}/bin)
foreach (COREBIN_NAME ${COREBIN_NAMES})
    Symlink(corebin "${OUTPUT_DIRECTORY_RECVRY}/bin/corebin" ${COREBIN_NAME} "${OUTPUT_DIRECTORY_RECVRY}/bin")
endforeach ()

# 3 - Copy over the core binaries to initramfs
CopyTarget(corebin corebin ${OUTPUT_DIRECTORY_I_BOOT}/bin)
foreach (COREBIN_NAME ${COREBIN_NAMES})
    Symlink(corebin "${OUTPUT_DIRECTORY_I_BOOT}/bin/corebin" ${COREBIN_NAME} "${OUTPUT_DIRECTORY_I_BOOT}/bin")
endforeach ()