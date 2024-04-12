#ifndef LED_SERVICE_H__
#define LED_SERVICE_H__

#include <stdint.h>

#include "ble.h"
#include "sdk_errors.h"


// Version 4 UUID: 62994c31-2721-4e0a-9b1e-b758ab5a2007
#define LED_SERVICE_BASE_UUID {{0x07, 0x20, 0x5a, 0xab, 0x58, 0xb7, 0x1e, 0x9b, 0x0a, 0x4e, 0x21, 0x27, 0x31, 0x4c, 0x99, 0x62}}
#define LED_SERVICE_UUID 0x4c31
#define LED_CHANGE_LED_CHAR_UUID 0x4c32
#define LED_READ_LED_CHAR_UUID 0x4c32

typedef struct
{
    uint16_t service_handle;
    uint16_t connection_handle;
    ble_gatts_char_handles_t change_led_handle;
    ble_gatts_char_handles_t read_led_handle;
} ble_led_service_t;

ret_code_t led_ble_service_init(ble_led_service_t*);

#endif