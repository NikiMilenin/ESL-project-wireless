#ifndef ESTC_SERVICE_H__
#define ESTC_SERVICE_H__

#include <stdint.h>

#include "ble.h"
#include "sdk_errors.h"

// Workshop 2

//  Version 4 UUID : 3230f347-0bf1-4276-b477-7629dbd22554:
#define ESTC_BASE_UUID {{0x54, 0x25, 0xd2, 0xdb, 0x29, 0x76, 0x77, 0xb4, 0x76, 0x42, 0xf1, 0x0b, 0x47, 0xf3, 0x30, 0x32}}

#define ESTC_SERVICE_UUID 0xf347 //Service UUID

// Workshop 3
#define ESTC_GATT_CHAR_1_UUID 0xf348 // 1st characteristic uuid
#define ESTC_GATT_CHAR_1_VALUE_SIZE 4

#define ESTC_GATT_CHAR_2_UUID 0xf349 // 2nd characteristic uuid
#define ESTC_GATT_CHAR_2_VALUE_SIZE 4

#define ESTC_GATT_CHAR_3_UUID 0xf350 // 3rd characteristic uuid
#define ESTC_GATT_CHAR_3_VALUE_SIZE 4

//Workshop 4
#define ESTC_GATT_CHAR_N_UUID 0xf351 // "Notification" characteristics
#define ESTC_GATT_CHAR_N_VALUE_SIZE 4

#define ESTC_GATT_CHAR_I_UUID 0xf352 // "Indication" characteristics
#define ESTC_GATT_CHAR_I_VALUE_SIZE 4

#define ESTC_GATT_CHAR_DESC_MAX_SIZE 40
typedef struct
{
    uint16_t service_handle;
    uint16_t connection_handle;
    ble_gatts_char_handles_t char_1_handle;
    ble_gatts_char_handles_t char_2_handle;
    ble_gatts_char_handles_t char_3_handle;
    ble_gatts_char_handles_t char_ind_handle;
    ble_gatts_char_handles_t char_notif_handle;
} ble_estc_service_t;


typedef struct
{
    uint16_t char_uuid;
    ble_uuid128_t service_uuid;
    ble_gatts_char_handles_t *char_handle;
    uint16_t value_size;

    short char_read;
    short char_write;
    short char_write_wo_resp;
    short notif;
    short ind;

    uint8_t* descriptor;
    int desc_size;
    short desc_write; 
} ble_estc_char_cfg;

ret_code_t estc_ble_service_init(ble_estc_service_t *service);

void estc_characteristic_indication(ble_estc_service_t *service, uint8_t *value, ble_gatts_char_handles_t* char_handle,  uint16_t value_size);
void estc_characteristic_notification(ble_estc_service_t *service, uint8_t *value, ble_gatts_char_handles_t* char_handle,  uint16_t value_size);

#endif /* ESTC_SERVICE_H__ */