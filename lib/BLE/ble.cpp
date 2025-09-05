#include "ble.h"

int data_length = 0;
uint8_t ble_data[256] = {0};
QueueHandle_t data_queue  = nullptr;

class ClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) override {
        DEBUG_PRINTLN("Client connected");
    }

    void onDisconnect(BLEClient* pclient) override {
        DEBUG_PRINTLN("Client disconnected");
    }
};

BLE::BLE() : role(BLE_CLIENT), is_initialized(false), ble_client (nullptr), ble_server (nullptr) {}

void BLE::callback_function(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* data, size_t length, bool isNotify) {
  BLE_Callback_Data callback_data;
  callback_data.UUID[0] = 0;
//   DEBUG_PRINTF("Callback for characteristic %s of length %d\n", pBLERemoteCharacteristic->getUUID().toString().c_str(), length);
  strcpy(callback_data.UUID, pBLERemoteCharacteristic->getUUID().toString().c_str());
  callback_data.data_length = length;
  memcpy(callback_data.data, data, length);
  if (data_queue != nullptr)
    xQueueSend(data_queue, &callback_data, pdMS_TO_TICKS(50));
  else
    DEBUG_PRINTLN("Data queue is null");
  // Wake up task updating scale update data in Scale Service
  xEventGroupSetBitsFromISR(ble_event_group, 0x01, NULL);
}

bool BLE::init(BLE_ROLE ble_role)
{
    this->role = ble_role;
    data_queue = xQueueCreate(256, sizeof(BLE_Callback_Data));
    switch (role)
    {
    case BLE_CLIENT:
        return initClient();

    case BLE_SERVER:
        return initServer();
    }
    return false;
}

bool BLE::initClient()
{
    if (is_initialized)
        return false;
    
    ble_device.init("Smart Scale");
    is_initialized = true;
    return true;
}

bool BLE::initServer()
{
    return true;
}

int BLE::scanDevices()
{
    int nb_of_scan_devices = 0;

    // Retrieve scan object
    BLEScan * scan_object = ble_device.getScan ();
    scan_object->setActiveScan(true);
    // Start scanning for 3 seconds
    BLEScanResults scan_result = scan_object->start (3);
    
    DEBUG_PRINTLN("Bluetooth device list:");
    for (int device_idx = 0; device_idx < scan_result.getCount(); device_idx++) {
        BLEAdvertisedDevice scan_device = scan_result.getDevice(device_idx);
        if (scan_device.haveName()) {
            nb_of_scan_devices++;
            DEBUG_PRINTF("Device %d: %s, RSSI: %d\n", nb_of_scan_devices, scan_device.getName().c_str(), scan_device.getRSSI());
        }
    }

    return nb_of_scan_devices;
}

bool BLE::addNewCharacteristic (String service_uuid, String CHARACTERISTIC_UUID) {
    for (int service_idx = 0; service_idx < nb_of_services; service_idx++) {
        if (service_handler[service_idx].service->getUUID().toString() == service_uuid.c_str()) {
            if (service_handler[service_idx].nb_of_chars + 1 >= MAX_CHARACTERISTICS)
                return false;

            if (role == BLE_CLIENT) {
                // service_handler[service_idx].characteristic_list[service_handler[service_idx].nb_of_chars++] = service_handler[service_idx].device.getS
            }
        } 
    } 
    return true;
}

bool BLE::connectDevice (String device_name) {
    // Retrieve scan object
    BLEScan * scan_object = ble_device.getScan ();
    scan_object->setActiveScan(true);
    // Start scanning for 3 seconds
    BLEScanResults scan_result = scan_object->start (3);
    
    DEBUG_PRINTLN("Bluetooth device list:");
    for (int device_idx = 0; device_idx < scan_result.getCount(); device_idx++) {
        BLEAdvertisedDevice scan_device = scan_result.getDevice(device_idx);
        if (!scan_device.haveName())
            continue;
        DEBUG_PRINTF("Device %d: %s, RSSI: %d\n", device_idx + 1, scan_device.getName().c_str(), scan_device.getRSSI());
        if (strcmp(scan_device.getName().c_str(), device_name.c_str()) == 0) {
            ble_client = BLEDevice::createClient();
            ble_client->setClientCallbacks(new ClientCallback());
            ble_device_found = scan_device;
            ble_client->connect(&scan_device);
            return true;
        }
    }
    return false;
}

bool BLE::reconnect () {
    if (ble_client != nullptr && !ble_client->isConnected()) {
        return ble_client->connect(&ble_device_found);
    }
    return false;
}

int BLE::registCharacteristic (String service_uuid, String char_uuid) {
    if (ble_client != nullptr) {
        BLEUUID ble_service_uuid (service_uuid.c_str()), ble_char_uuid (char_uuid.c_str());

        BLERemoteService * ser_object = ble_client->getService(ble_service_uuid);
        if (ser_object == nullptr) {
            DEBUG_PRINTLN("NOT HAVE GIVEN SERVICE UUID");
            return -1;
        }
        BLERemoteCharacteristic* char_object = ser_object->getCharacteristic(ble_char_uuid);
        if (char_object == nullptr) {
            DEBUG_PRINTLN("NOT HAVE GIVEN CHAR UUID");
            return -1;
        }
        char_object->registerForNotify(callback_function, true);
    }
    return 0;
}

BLE_Callback_Data BLE::getData () {
    BLE_Callback_Data data;
    if (uxQueueMessagesWaiting(data_queue)) {
        xQueueReceive(data_queue, &data, (TickType_t)0);
    }
    return data;
}

