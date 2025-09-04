#ifndef SCALE_SERVICE_H
#define SCALE_SERVICE_H
#include <Arduino.h>
#include "ble.h"

#define DEVICE_NAME        "MI SCALE2"
#define SERVICE_UUID        "0000181d-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00002a9d-0000-1000-8000-00805f9b34fb"

enum SCALE_ERROR_CODE {
    SCALE_OK = 0,
    SCALE_ERR_BLE_SERVICE_NOT_FOUND = -4,
    SCALE_ERR_BLE_CHAR_NOT_FOUND = -5,
    SCALE_ERR_UNKNOWN = -100
};

SCALE_ERROR_CODE Scale_Service_init ();
static void Scale_Service_run ();

#endif