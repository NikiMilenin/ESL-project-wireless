#ifndef LED_SERVICE_H__
#define LED_SERVICE_H__


#include <stdint.h>
#include <math.h>
#include "sdk_errors.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log_backend_usb.h"
#include "app_usbd.h"
#include "app_usbd_serial_num.h"


#include <stdint.h>
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "nrfx_systick.h"
#include "nrfx_pwm.h"


#include "app_error.h"
#include "nrf_log.h"


#include "ble.h"
#include "ble_gatts.h"
#include "ble_srv_common.h"

#include "led.h"


// Version 4 UUID: 62994c31-2721-4e0a-9b1e-b758ab5a2007
#define LED_SERVICE_BASE_UUID {{0x07, 0x20, 0x5a, 0xab, 0x58, 0xb7, 0x1e, 0x9b, 0x0a, 0x4e, 0x21, 0x27, 0x31, 0x4c, 0x99, 0x62}}
#define LED_SERVICE_UUID 0x4c31
#define LED_CHANGE_COLOR_CHAR_UUID 0x4c32
#define LED_READ_READ_CHAR_UUID 0x4c32


typedef struct
{
    uint16_t service_handle;
    uint16_t connection_handle;
    ble_gatts_char_handles_t change_led_handle;
    ble_gatts_char_handles_t read_led_handle;
} ble_led_service_t;


ret_code_t led_ble_service_init(ble_led_service_t *service);
void led_on_write(ble_evt_t const * p_ble_evt, ble_led_service_t * service);


#endif