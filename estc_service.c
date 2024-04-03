#include "estc_service.h"

#include "app_error.h"
#include "nrf_log.h"

#include "ble.h"
#include "ble_gatts.h"
#include "ble_srv_common.h"

static ret_code_t estc_ble_add_characteristics(ble_estc_service_t *service, ble_uuid128_t service_uuid);
static ret_code_t estc_ble_create_char(ble_estc_service_t*, ble_uuid128_t, ble_estc_char_cfg, ble_gatts_char_handles_t);
static ret_code_t estc_ble_create_notification_char(ble_estc_service_t*);
static ret_code_t estc_ble_create_identification_char(ble_estc_service_t*);

ret_code_t estc_ble_service_init(ble_estc_service_t *service)
{
    //Workshop2
    ret_code_t error_code = NRF_SUCCESS;

    // Adding service UUIDs to the BLE stack table
    ble_uuid_t service_uuid;
    service_uuid.uuid = ESTC_SERVICE_UUID;
    service_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN;

    ble_uuid128_t base_uuid = ESTC_BASE_UUID;

    error_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(error_code);


    // Adding service to the BLE stack
    error_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &(service->service_handle));
    APP_ERROR_CHECK(error_code);

    NRF_LOG_DEBUG("%s:%d | Service UUID: 0x%04x", __FUNCTION__, __LINE__, service_uuid.uuid);
    NRF_LOG_DEBUG("%s:%d | Service UUID type: 0x%02x", __FUNCTION__, __LINE__, service_uuid.type);
    NRF_LOG_DEBUG("%s:%d | Service handle: 0x%04x", __FUNCTION__, __LINE__, service->service_handle);

    return estc_ble_add_characteristics(service, base_uuid);
}

static ret_code_t estc_ble_add_characteristics(ble_estc_service_t *service, ble_uuid128_t service_uuid)
{
    ret_code_t error_code = NRF_SUCCESS;
    
    uint8_t chr1_user_desc[] = "Char 1 descriptor.";
    ble_estc_char_cfg char_1_cfg = {ESTC_GATT_CHAR_1_UUID, 
                                        chr1_user_desc,
                                        sizeof(chr1_user_desc)/sizeof(chr1_user_desc[0]),
                                        1, 1, 0, };
    
    error_code = estc_ble_create_char(service, service_uuid, char_1_cfg, service->char_1_handle);
    APP_ERROR_CHECK(error_code);

    uint8_t chr2_user_desc[] = "Char 2 descriptor (changable)";
    ble_estc_char_cfg char_2_cfg = {ESTC_GATT_CHAR_2_UUID, 
                                        chr2_user_desc,
                                        sizeof(chr2_user_desc)/sizeof(chr2_user_desc[0]),
                                        1, 1, 1, };
    
    error_code = estc_ble_create_char(service, service_uuid, char_2_cfg, service->char_2_handle);
   
    
    ble_estc_char_cfg char_3_cfg = {ESTC_GATT_CHAR_3_UUID, 
                                        NULL,
                                        0,
                                        1, 0, 0, };
    
    error_code = estc_ble_create_char(service, service_uuid, char_3_cfg, service->char_3_handle);
    error_code = estc_ble_create_notification_char(service);
    estc_ble_create_identification_char(service);
    return error_code;
}


// This function needs to create few characteristics to service
static ret_code_t estc_ble_create_char(ble_estc_service_t* service, ble_uuid128_t service_uuid, ble_estc_char_cfg char_cfg, ble_gatts_char_handles_t char_handle)
{
    //Workshop 3
    ret_code_t error_code = NRF_SUCCESS;

    // Add custom characteristic UUID 
    ble_uuid_t characteristic_uuid;
    characteristic_uuid.uuid = char_cfg.char_uuid;
    characteristic_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN;

    ble_uuid128_t base_uuid = service_uuid;

    error_code = sd_ble_uuid_vs_add(&base_uuid, &characteristic_uuid.type);
    APP_ERROR_CHECK(error_code);

    // Configure Characteristic metadata (enable read and write)
    ble_gatts_char_md_t char_md = { 0 };
    char_md.char_props.read = char_cfg.char_read;
    char_md.char_props.write = char_cfg.char_write;

    
    //Additional 3.1
    if (char_cfg.desc_size > 0) {
        char_md.char_ext_props.wr_aux = char_cfg.desc_write;
            if (char_cfg.desc_write) {
            char_md.char_user_desc_max_size = ESTC_GATT_CHAR_DESC_MAX_SIZE;
            char_md.char_user_desc_size = ESTC_GATT_CHAR_DESC_MAX_SIZE < char_cfg.desc_size ? ESTC_GATT_CHAR_DESC_MAX_SIZE: char_cfg.desc_size;
        } else {
            char_md.char_user_desc_max_size = char_cfg.desc_size;
            char_md.char_user_desc_size = char_cfg.desc_size;
        }
        char_md.p_char_user_desc = char_cfg.descriptor;
    }
    //End of additional 3.1
    

    // Configures attribute metadata. For now we only specify that the attribute will be stored in the softdevice
    ble_gatts_attr_md_t attr_md = { 0 };
    attr_md.vloc = BLE_GATTS_VLOC_STACK;
    
    // Set read/write security levels to attribute metadata 
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&(attr_md.read_perm));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&(attr_md.write_perm));
    
    // Configure the characteristic value attribute
    ble_gatts_attr_t attr_char_value = { 0 };
    attr_char_value.p_uuid = &characteristic_uuid;
    attr_char_value.p_attr_md = &attr_md;

    // Set characteristic length in number of bytes 
    attr_char_value.max_len = 4;
    attr_char_value.init_len = 4;

    // Adding new characteristic to the service 
    error_code = sd_ble_gatts_characteristic_add(service->service_handle, &char_md, 
                                                &attr_char_value, &(char_handle));
    APP_ERROR_CHECK(error_code);
    
    return error_code;
}



static ret_code_t estc_ble_create_notification_char(ble_estc_service_t* service)
{
    ret_code_t err_code;
    ble_uuid_t  char_uuid;
    ble_uuid128_t base_uuid = ESTC_BASE_UUID;
    char_uuid.uuid = ESTC_GATT_CHAR_N_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code);

    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));    
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md; 

    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 0;
    char_md.char_props.write = 0;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_char_value.max_len     = 4;
    attr_char_value.init_len    = 4;
    uint8_t value[4]            = {0x12,0x34,0x56,0x78};
    attr_char_value.p_value     = value;

    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;

    err_code = sd_ble_gatts_characteristic_add(service->service_handle,
                                   &char_md,
                                   &attr_char_value,
                                   &service->char_notif_handle);
    APP_ERROR_CHECK(err_code);


    return err_code;
}


static ret_code_t estc_ble_create_identification_char(ble_estc_service_t* service)
{
    ret_code_t err_code;
    ble_uuid_t  char_uuid;
    ble_uuid128_t base_uuid = ESTC_BASE_UUID;
    char_uuid.uuid = ESTC_GATT_CHAR_I_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code);

    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));    
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md; 

    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 0;
    char_md.char_props.write = 0;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    attr_char_value.max_len     = 4;
    attr_char_value.init_len    = 4;
    uint8_t value[4]            = {0x12,0x34,0x56,0x78};
    attr_char_value.p_value     = value;

    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.indicate  = 1;

    err_code = sd_ble_gatts_characteristic_add(service->service_handle,
                                   &char_md,
                                   &attr_char_value,
                                   &service->char_ind_handle);
    APP_ERROR_CHECK(err_code);
    return err_code;
}


void estc_update_characteristic_indication_value(ble_estc_service_t *service, uint8_t *value)
{
    uint16_t               len = 4;
    ble_gatts_hvx_params_t hvx_params;
    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = service->char_ind_handle.value_handle;
    hvx_params.type   = BLE_GATT_HVX_INDICATION;
    hvx_params.offset = 0;
    hvx_params.p_len  = &len;
    hvx_params.p_data = value;  

    sd_ble_gatts_hvx(service->connection_handle, &hvx_params);
}

void estc_update_characteristic_notification_value(ble_estc_service_t *service, uint8_t *value)
{
    uint16_t               len = 4;
    ble_gatts_hvx_params_t hvx_params;
    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_params.handle = service->char_notif_handle.value_handle;
    hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
    hvx_params.offset = 0;
    hvx_params.p_len  = &len;
    hvx_params.p_data = value;  

    sd_ble_gatts_hvx(service->connection_handle, &hvx_params);
}