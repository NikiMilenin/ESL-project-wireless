#include "led.h"


static uint8_t rgb[] = {200, 122, 125};


nrfx_pwm_t rgb_led = NRFX_PWM_INSTANCE(0);


nrf_pwm_values_individual_t seq_values = {
    .channel_0 = 0,
    .channel_1 = 0,
    .channel_2 = 0,
    .channel_3 = 0
};


static const nrfx_pwm_config_t rgb_pwm_config = {
    .output_pins = {
        NRFX_PWM_PIN_NOT_USED,
        LED2_R_PIN,
        LED2_G_PIN,
        LED2_B_PIN  
    },
    .irq_priority = 6,
    .base_clock = NRF_PWM_CLK_1MHz,
    .count_mode = NRF_PWM_MODE_UP,
    .top_value = PWM_RGB_TOP_VALUE,
    .load_mode = NRF_PWM_LOAD_INDIVIDUAL,
    .step_mode = NRF_PWM_STEP_AUTO
};


ret_code_t leds_init()
{
    ret_code_t err_code = NRF_SUCCESS;
    const nrf_pwm_sequence_t sequence = {
        .values.p_individual = &seq_values,
        .length = NRF_PWM_VALUES_LENGTH(seq_values),
        .repeats = 0,
        .end_delay = 0
    };
    rgb_off();
    fstorage_init();
    get_led(rgb, sizeof(rgb));
    err_code = nrfx_pwm_init(&rgb_led, &rgb_pwm_config, NULL);
    APP_ERROR_CHECK(err_code);
    err_code = nrfx_pwm_simple_playback(&rgb_led, &sequence, 1, NRFX_PWM_FLAG_LOOP);
    return(err_code);
}

void change_led_data(uint8_t const * p_data)
{
    NRF_LOG_INFO("Writing %d, %d, %d", p_data[0], p_data[1], p_data[2]);
    rgb[0] = p_data[0];
    rgb[1] = p_data[1];
    rgb[2] = p_data[2];
    rgb_on();
    save_led(rgb, sizeof(rgb));
}

void rgb_on()
{
    seq_values.channel_1 = rgb[0];
    seq_values.channel_2 = rgb[1];
    seq_values.channel_3 = rgb[2];
}


void rgb_off()
{
    NRF_LOG_INFO("RGB off");
    seq_values.channel_1 = 0;
    seq_values.channel_2 = 0;
    seq_values.channel_3 = 0;
}