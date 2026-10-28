#ifndef MDLC_ARCHIT_H
#define MDLC_ARCHIT_H

#ifdef __x86_64__
#define SYSTEM_FS_PATH "/System/x64"
#define SYSTEM_FS_NAME "x64"
#endif

#ifdef __aarch64__
#define SYSTEM_FS_PATH "/System/arm64"
#define SYSTEM_FS_NAME "arm64"
#endif

#endif //MDLC_ARCHIT_H
