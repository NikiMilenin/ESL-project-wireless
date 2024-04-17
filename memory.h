#ifndef MEMORY_H__
#define MEMORY_H__


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


#include "nrf_fstorage.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_fstorage_sd.h"


#define MEM_START 0x3e000
#define MEM_END   0x3ffff
#define WRITE_ADDE 0x3d000


void fstorage_init();
void save_led(uint8_t * arr, int len);
void get_led(uint8_t * arr, int len);


#endif