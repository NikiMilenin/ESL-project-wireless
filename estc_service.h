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
#define ESTC_GATT_CHAR_2_UUID 0xf349 // 2nd characteristic uuid
#define ESTC_GATT_CHAR_3_UUID 0xf350 // 3rd characteristic uuid

//Workshop 4
#define ESTC_GATT_CHAR_N_UUID 0xf351 // "Notification" characteristics
#define ESTC_GATT_CHAR_I_UUID 0xf352 // "Indication" characteristics

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
    uint8_t* descriptor;
    int desc_size;
    short char_read;  // read characteristicks 
    short char_write;  // write characteristicks
    short desc_write;  // write characteristicks description
} ble_estc_char_cfg;

ret_code_t estc_ble_service_init(ble_estc_service_t *service);

void estc_update_characteristic_indication_value(ble_estc_service_t *service, uint8_t *value);
void estc_update_characteristic_notification_value(ble_estc_service_t *service, uint8_t *value);

#endif /* ESTC_SERVICE_H__ */