#ifndef ALX64_Device_H
#define ALX64_Device_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DEVICE_TYPE_MOBILE = 0xff9a,
    DEVICE_TYPE_DESKTOP = 0xfc5e,
    DEVICE_TYPE_TABLET = 0xfacc,
    DEVICE_TYPE_LAPTOP = 0xfeb1,
    DEVICE_TYPE_USB = 0xacca,
    DEVICE_TYPE_DISPLAY = 0xacc1,
    DEVICE_TYPE_HID_INPUT = 0xacc2,
    DEVICE_TYPE_HID_OUTPUT = 0xacc3,
    DEVICE_TYPE_AUDIO_INPUT = 0xacfe,
    DEVICE_TYPE_AUDIO_OUTPUT = 0xacff,
    DEVICE_TYPE_CAMERA = 0xaf13,
    DEVICE_TYPE_BLUETOOTH = 0xaf14,
    DEVICE_TYPE_WIFI = 0xaf15,
    DEVICE_TYPE_MISC = 0xaaaa
} DeviceType;

DeviceType Device__GetCurrentDeviceType();

DeviceType Device__GetConnectedDeviceType(int bus, int dev);

#ifdef __cplusplus
}
#endif

#endif //ALX64_Device_H
