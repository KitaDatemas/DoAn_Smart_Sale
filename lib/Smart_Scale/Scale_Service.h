#ifndef SCALE_SERVICE_H
#define SCALE_SERVICE_H
#include <Arduino.h>
#include "freertos/event_groups.h"
#include "flag.h"
#include "ble.h"

#define DEVICE_NAME                 "MI SCALE2"
#define SERVICE_UUID                "0000181d-0000-1000-8000-00805f9b34fb"
#define WEIGHT_CHARACTERISTIC_UUID  "00002a9d-0000-1000-8000-00805f9b34fb"

enum SCALE_ERROR_CODE {
    SCALE_OK = 0,
    SCALE_INIT_FAILED = -1,
    SCALE_ERR_BLE_NOT_CONNECTED = -2,
    SCALE_ERR_BLE_SERVICE_NOT_FOUND = -4,
    SCALE_ERR_BLE_CHAR_NOT_FOUND = -5,
    SCALE_ERR_UNKNOWN = -100
};

struct Scale_Timestamp {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

struct Scale_Value {
    Scale_Timestamp timestamp;
    bool is_lbs;
    float weight;
    float bmi;
    uint8_t body_fat_percentage;
    uint8_t muscle_mass_percentage;
    uint8_t water_percentage;
    uint8_t bone_mass_kg;
    uint8_t visceral_fat_level;
    uint8_t metabolic_age;
};

SCALE_ERROR_CODE Scale_Service_init ();
void Scale_Service_run  ();

static void Scale_Service_keepAlive (void * pvParameters);
static void Scale_Service_updateScaleValue (void * pvParameters);
Scale_Value Scale_Service_getScaleValue ();
void        Scale_Service_parseWeightData (uint8_t * data, size_t length);

#endif