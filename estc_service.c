#include "estc_service.h"

#include "app_error.h"
#include "nrf_log.h"

#include "ble.h"
#include "ble_gatts.h"
#include "ble_srv_common.h"


// Structure to pass r/w properties for characteristick in to function
typedef struct
{
    short char_read;  // read characteristicks 
    short char_write;  // write characteristicks
    short desc_write;  // write characteristicks description
} ble_estc_char_rw_props;

static ret_code_t estc_ble_add_characteristics(ble_estc_service_t*, ble_uuid128_t);
static ret_code_t estc_ble_create_char(ble_estc_service_t*, ble_uuid128_t, uint16_t, ble_estc_char_rw_props, uint8_t* , int);

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

// Я немного отошел от примера, для того чтобы реализовать сразу несколько характеристик в сервисе и не дублировать код, вынес
// создание сервиса в отдельную функцию, куда передаю необходимые мне параметры, 
// при необходимости можно будет расширить структуру ble_estc_char_rw_props, чтобы она хранила, другие флаги (broadcast, notify, write withou response и т.д.)
// и передавать всё это в функцию estc_ble_create_char.

static ret_code_t estc_ble_add_characteristics(ble_estc_service_t *service, ble_uuid128_t service_uuid)
{
    ret_code_t error_code = NRF_SUCCESS;
    
    ble_estc_char_rw_props char_1_props = {1, 1, 0};
    uint8_t chr1_user_desc[] = "Char 1 descriptor.";
    error_code = estc_ble_create_char(service, service_uuid, ESTC_GATT_CHAR_1_UUID, char_1_props, chr1_user_desc, sizeof(chr1_user_desc)/sizeof(chr1_user_desc[0]));
    APP_ERROR_CHECK(error_code);
    
    ble_estc_char_rw_props char_2_props = {1, 0, 0};
    uint8_t chr2_user_desc[] = "Char 2 descriptor";
    error_code = estc_ble_create_char(service, service_uuid, ESTC_GATT_CHAR_2_UUID, char_2_props,  chr2_user_desc, sizeof(chr2_user_desc)/sizeof(chr2_user_desc[0]));
    APP_ERROR_CHECK(error_code);

    ble_estc_char_rw_props char_3_props = {1, 1, 1};
    uint8_t chr3_user_desc[] = "Char 3 descriptor";
    error_code = estc_ble_create_char(service, service_uuid, ESTC_GATT_CHAR_3_UUID, char_3_props,  chr3_user_desc, sizeof(chr3_user_desc)/sizeof(chr3_user_desc[0]));
    APP_ERROR_CHECK(error_code);

    ble_estc_char_rw_props char_4_props = {1, 1, 0};
    error_code = estc_ble_create_char(service, service_uuid, ESTC_GATT_CHAR_4_UUID, char_4_props, NULL, 0);
    APP_ERROR_CHECK(error_code);
    return error_code;
}


// This function needs to create few characteristics to service
static ret_code_t estc_ble_create_char(ble_estc_service_t* service, ble_uuid128_t service_uuid, uint16_t char_uuid, 
                                                    ble_estc_char_rw_props props, uint8_t* descriptor, int desc_size)
{
    //Workshop 3
    ret_code_t error_code = NRF_SUCCESS;

    // Add custom characteristic UUID 
    ble_uuid_t characteristic_uuid;
    characteristic_uuid.uuid = char_uuid;
    characteristic_uuid.type = BLE_UUID_TYPE_VENDOR_BEGIN;

    ble_uuid128_t base_uuid = service_uuid;

    error_code = sd_ble_uuid_vs_add(&base_uuid, &characteristic_uuid.type);
    APP_ERROR_CHECK(error_code);

    // Configure Characteristic metadata (enable read and write)
    ble_gatts_char_md_t char_md = { 0 };
    char_md.char_props.read = props.char_read;
    char_md.char_props.write = props.char_write;

    
    //Additional 3.1
    if (desc_size > 0) {
        char_md.char_ext_props.wr_aux = props.desc_write;
            if (props.desc_write) {
            char_md.char_user_desc_max_size = ESTC_GATT_CHAR_DESC_MAX_SIZE;
            char_md.char_user_desc_size = ESTC_GATT_CHAR_DESC_MAX_SIZE < desc_size ? ESTC_GATT_CHAR_DESC_MAX_SIZE: desc_size;
        } else {
            char_md.char_user_desc_max_size = desc_size;
            char_md.char_user_desc_size = desc_size;
        }
        char_md.p_char_user_desc = descriptor;
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
                                                &attr_char_value, &(service->characteristic_handle));
    APP_ERROR_CHECK(error_code);
    
    return error_code;
}