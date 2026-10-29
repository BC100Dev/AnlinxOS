#ifndef SYS_SYSTEM_PROPERTIES_H
#define SYS_SYSTEM_PROPERTIES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PROP_VALUE_MAX  398
#define PROP_NAME_MAX   254

#define PROP_SERVICE_NAME                 "property_service"
#define PROP_SERVICE_FOR_SYSTEM_NAME      "property_service_for_system"
#define PROP_DIRNAME                      "/dev/__properties__"

#define PROP_MSG_SETPROP                  1
#define PROP_MSG_SETPROP2                 0x00020001

#define PROP_SUCCESS                      0
#define PROP_ERROR_READ_CMD               0x0004
#define PROP_ERROR_READ_DATA              0x0008
#define PROP_ERROR_READ_ONLY_PROPERTY     0x000B
#define PROP_ERROR_INVALID_NAME           0x0010
#define PROP_ERROR_INVALID_VALUE          0x0014
#define PROP_ERROR_PERMISSION_DENIED      0x0018
#define PROP_ERROR_INVALID_CMD            0x001B
#define PROP_ERROR_HANDLE_CONTROL_MESSAGE 0x0020
#define PROP_ERROR_SET_FAILED             0x0024

typedef struct prop_info prop_info;

typedef struct {
    const char* name;
    const char* value;
} system_property;

typedef struct {
    const char* app_namespace;
    const char* name;
    const char* value;
} ns_system_property;

int __system_property_set(const char* name, const char* value);
const prop_info* __system_property_find(const char* name);
void __system_property_read_callback(const prop_info* pi,
                                     void (*callback)(void* cookie, const char* name, const char* value,
                                                      uint32_t serial),
                                     void* cookie);
int __system_property_foreach(void (*callback)(const prop_info* pi, void* cookie), void* cookie);
bool __system_property_wait(const prop_info* pi, uint32_t old_serial, uint32_t* new_serial_ptr,
                            const struct timespec* relative_timeout);
uint32_t __system_property_area_serial(void);
uint32_t __system_property_serial(const prop_info* pi);
int __system_properties_init(void);
int __system_property_area_init(void);
int __system_property_add(const char* name, unsigned int name_length, const char* value, unsigned int value_length);
int __system_property_update(prop_info* pi, const char* value, unsigned int value_length);

int __system_property_get(const char* name, char* value);
int __system_property_read(const prop_info* pi, char* name, char* value);
const prop_info* __system_property_find_nth(unsigned n);
uint32_t __system_property_wait_any(uint32_t old_serial);
int __system_property_set_filename(const char* filename);

int __system_property_set_sp(const system_property* prop);
int __system_property_get_sp(const char* name, system_property* out);
const prop_info* __system_property_find_sp(const system_property* prop);
void __system_property_read_callback_sp(const prop_info* pi,
                                        void (*callback)(void* cookie, const system_property* prop, uint32_t serial),
                                        void* cookie);
int __system_property_foreach_sp(void (*callback)(const prop_info* pi, const system_property* prop, void* cookie),
                                 void* cookie);

int __system_property_set_ns(const ns_system_property* prop);
int __system_property_get_ns(const char* app_namespace, const char* name, ns_system_property* out);
const prop_info* __system_property_find_ns(const ns_system_property* prop);
void __system_property_read_callback_ns(const prop_info* pi,
                                        void (*callback)(void* cookie, const ns_system_property* prop, uint32_t serial),
                                        void* cookie);
int __system_property_foreach_ns(const char* app_namespace,
                                 void (*callback)(const prop_info* pi, const ns_system_property* prop, void* cookie),
                                 void* cookie);

#ifdef __cplusplus
}
#endif

#endif
