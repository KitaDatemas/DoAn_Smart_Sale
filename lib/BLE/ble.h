#include <Arduino.h>

#include "common.h"
#include "flag.h"

#include <BLEDevice.h>
#include <BLEUtils.h>
// #include <BLEServer.h>
#include <BLEClient.h>
#include <BLE2902.h>

#define MAX_SERVICES            20
#define MAX_CHARACTERISTICS     30

enum BLE_ROLE {BLE_SERVER, BLE_CLIENT};

struct BLE_Callback_Data {
    char    UUID[256];
    uint8_t data_length;
    uint8_t data [128];
};

class BLE{
private:
    struct BLE_Service  {
        BLEAdvertisedDevice device;
        BLEService * service = nullptr;
        BLECharacteristic * characteristic_list [MAX_CHARACTERISTICS] = {nullptr};
        int nb_of_chars = 0;
    };

public:
    BLE ();
    bool init (BLE_ROLE ble_role);
    bool initClient ();
    bool initServer ();
    bool connected () { return ble_client->isConnected(); };
    int  scanDevices ();
    bool addNewCharacteristic (String service_uuid, String CHARACTERISTIC_UUID);
    bool reconnect ();
    bool connectDevice (String device_name);
    int  registCharacteristic (String service_uuid, String char_uuid);
    BLE_Callback_Data getData ();
    static void callback_function(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* data, size_t length, bool isNotify);

private:
    bool is_initialized;
    BLEDevice ble_device;
    BLEClient * ble_client;
    BLEServer * ble_server;
    BLEAdvertisedDevice ble_device_found;
    BLE_ROLE role;
    BLE_Service service_handler [MAX_SERVICES];
    int nb_of_services = 0;
    bool ble_connected = false;
};

extern QueueHandle_t data_queue;