#include "led_service.h"


static ret_code_t add_change_color_char(ble_led_service_t* service)
{
    ret_code_t error_code = NRF_SUCCESS;

    ble_uuid_t characteristic_uuid;
    characteristic_uuid.uuid = LED_CHANGE_COLOR_CHAR_UUID;
    characteristic_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN;

    ble_uuid128_t base_uuid = LED_SERVICE_BASE_UUID;

    error_code = sd_ble_uuid_vs_add(&base_uuid, &characteristic_uuid.type);
    APP_ERROR_CHECK(error_code);

    ble_gatts_char_md_t char_md = { 0 };
    char_md.char_props.write = 1;

    ble_gatts_attr_md_t attr_md = { 0 };
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&(attr_md.read_perm));
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&(attr_md.write_perm));


    ble_gatts_attr_t attr_char_value = { 0 };
    attr_char_value.p_uuid = &characteristic_uuid;
    attr_char_value.p_attr_md = &attr_md;

    attr_char_value.max_len = sizeof(uint8_t) * RGB_SIZE;
    attr_char_value.init_len = sizeof(uint8_t) * RGB_SIZE;
    uint8_t data[] = {0, 0, 0};
    attr_char_value.p_value = data;
    
    
    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    

    error_code = sd_ble_gatts_characteristic_add(service->service_handle, &char_md, 
                                                &attr_char_value, &(service->change_led_handle));
    APP_ERROR_CHECK(error_code);

    return error_code;
}


static ret_code_t add_read_color_char(ble_led_service_t* service)
{
    ret_code_t error_code = NRF_SUCCESS;

    ble_uuid_t characteristic_uuid;
    characteristic_uuid.uuid = LED_CHANGE_COLOR_CHAR_UUID;
    characteristic_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN;

    ble_uuid128_t base_uuid = LED_SERVICE_BASE_UUID;

    error_code = sd_ble_uuid_vs_add(&base_uuid, &characteristic_uuid.type);
    APP_ERROR_CHECK(error_code);

    ble_gatts_char_md_t char_md = { 0 };
    char_md.char_props.read = 1;

    ble_gatts_attr_md_t attr_md = { 0 };
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&(attr_md.read_perm));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&(attr_md.write_perm));

    ble_gatts_attr_t attr_char_value = { 0 };
    attr_char_value.p_uuid = &characteristic_uuid;
    attr_char_value.p_attr_md = &attr_md;

    attr_char_value.max_len = sizeof(uint8_t) * RGB_SIZE;
    attr_char_value.init_len = sizeof(uint8_t) * RGB_SIZE;
    uint8_t data[] = {0, 0, 0};
    attr_char_value.p_value = data;

    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;

    error_code = sd_ble_gatts_characteristic_add(service->service_handle, &char_md, 
                                                &attr_char_value, &(service->read_led_handle));
    APP_ERROR_CHECK(error_code);

    return error_code;
}


void led_change_notification(ble_led_service_t *service, const uint8_t *value, ble_gatts_char_handles_t* char_handle)
{
    uint16_t               len = RGB_SIZE;
    ble_gatts_hvx_params_t hvx_params;
    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = char_handle->value_handle;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset = 0;
    hvx_params.p_len  = &len;
    hvx_params.p_data = value;  

    sd_ble_gatts_hvx(service->connection_handle, &hvx_params);
}


ret_code_t led_ble_service_init(ble_led_service_t *service)
{
    ret_code_t error_code = NRF_SUCCESS;

    ble_uuid_t service_uuid;
    service_uuid.uuid = LED_SERVICE_UUID;
    service_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN;

    ble_uuid128_t base_uuid = LED_SERVICE_BASE_UUID;

    error_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(error_code);

    error_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &(service->service_handle));
    APP_ERROR_CHECK(error_code);

    NRF_LOG_DEBUG("%s:%d | Service UUID: 0x%04x", __FUNCTION__, __LINE__, service_uuid.uuid);
    NRF_LOG_DEBUG("%s:%d | Service UUID type: 0x%02x", __FUNCTION__, __LINE__, service_uuid.type);
    NRF_LOG_DEBUG("%s:%d | Service handle: 0x%04x", __FUNCTION__, __LINE__, service->service_handle);
    error_code = add_change_color_char(service);
    APP_ERROR_CHECK(error_code);
    return add_read_color_char(service);
}


void led_on_write(ble_evt_t const * p_ble_evt, ble_led_service_t * service)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    if ((p_evt_write->handle == service->change_led_handle.value_handle) && (p_evt_write->len == RGB_SIZE))
    {
        change_led_data(p_evt_write->data);
        led_change_notification(service, p_evt_write->data, &(service->read_led_handle));
    }
}