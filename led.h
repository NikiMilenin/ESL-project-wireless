#ifndef LED_H__
#define LED_H__


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


#include "memory.h"


#define LED2_R_PIN NRF_GPIO_PIN_MAP(0, 8)
#define LED2_G_PIN NRF_GPIO_PIN_MAP(1, 9)
#define LED2_B_PIN NRF_GPIO_PIN_MAP(0, 12)
#define PWM_RGB_TOP_VALUE 255


#define RGB_SIZE 3


ret_code_t leds_init();
void change_led_data(uint8_t const * p_data);
void rgb_on();
void rgb_off();


#endif