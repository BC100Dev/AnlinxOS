#ifndef ALX64_Device_H
#define ALX64_Device_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DEVICE_TYPE_MOBILE  = 0xff9a,
    DEVICE_TYPE_DESKTOP = 0xfc5e,
    DEVICE_TYPE_TABLET  = 0xfacc,
    DEVICE_TYPE_LAPTOP  = 0xfeb1
} DeviceType;

#ifdef __cplusplus
}
#endif

#endif //ALX64_Device_H
