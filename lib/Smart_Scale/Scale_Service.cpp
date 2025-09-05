#include "Scale_Service.h"

static BLE ble_client;
static Scale_Value scale_value;

SCALE_ERROR_CODE Scale_Service_init()
{
    if (!ble_client.init(BLE_CLIENT))
    {
        DEBUG_PRINTLN("Failed to initialize BLE client");
        return SCALE_INIT_FAILED;
    }

    DEBUG_PRINTLN("Trying to connnect to scale");
    while (!ble_client.connectDevice("MI SCALE2"))
    {
        DEBUG_PRINT(".");
    }
    DEBUG_PRINTLN("\nConnected to scale");

    if (ble_client.registCharacteristic(SERVICE_UUID, WEIGHT_CHARACTERISTIC_UUID) < 0)
    {
        DEBUG_PRINTLN("Failed to register to weight characteristic");
        return SCALE_ERR_BLE_CHAR_NOT_FOUND;
    }
    return SCALE_OK;
}

void Scale_Service_run ()
{
    while (Scale_Service_init() != SCALE_OK)
    {
        vTaskDelay(pdMS_TO_TICKS(3000));
        DEBUG_PRINTLN("Retrying to initialize scale service in 5 seconds...");
    }
    xTaskCreate(Scale_Service_keepAlive, "Scale Keep Alive", 2048, NULL, 1, NULL);
    xTaskCreate(Scale_Service_updateScaleValue, "Scale Update Value", 4096, NULL, 1, NULL);
}

void Scale_Service_keepAlive(void *pvParameters)
{
    while (1)
    {
        if (!ble_client.connected())
        {
            DEBUG_PRINTLN("Reconnecting...");
            if (ble_client.reconnect())
            {
                ble_client.registCharacteristic(SERVICE_UUID, WEIGHT_CHARACTERISTIC_UUID);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Check every 100 millis seconds
    }
    vTaskDelete(NULL);
}

void Scale_Service_updateScaleValue(void *pvParameters)
{
    while (1)
    {
        // Wait for data ready event from BLE callback
        xEventGroupWaitBits(ble_event_group, 0x01, pdTRUE, pdFALSE, portMAX_DELAY);
        
        BLE_Callback_Data data = ble_client.getData();
        if (data.data_length != 0)
        {
            // Parse weight data on update
            if (strcmp(data.UUID, WEIGHT_CHARACTERISTIC_UUID) == 0)
            {
                Scale_Service_parseWeightData(data.data, data.data_length);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Yield to other tasks
    }
    vTaskDelete(NULL);
}

void Scale_Service_parseWeightData (uint8_t * data, size_t length)
{
    if (data == nullptr || length <= 0)
        return;

    scale_value.is_lbs           = (data[0] & 0x01);
    scale_value.weight           = ((data[2] << 8) | data[1]) / ((scale_value.is_lbs) ? 100.0 : 200.0);
    scale_value.timestamp.year   = (data[4] << 8) | data[3];
    scale_value.timestamp.month  = data[5];
    scale_value.timestamp.day    = data[6];
    scale_value.timestamp.hour   = data[7];
    scale_value.timestamp.minute = data[8];
    scale_value.timestamp.second = data[9];
}

Scale_Value Scale_Service_getScaleValue()
{
    return scale_value;
}