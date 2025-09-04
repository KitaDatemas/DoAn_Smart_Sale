#include <Arduino.h>
#include <ble.h>

#define UNKNOWN_UUID        "00001543-0000-3512-2118-0009af100700"

BLE ble_client;

void setup() {
  Serial.begin (9600);

  // put your setup code here, to run once:
  ble_client.init (BLE_CLIENT);
  ble_client.scanDevices ();
  while (!ble_client.getAddress("MI SCALE2")) 
    ble_client.scanDevices ();
  // ble_client.registCharacteristic(SERVICE_UUID, CHARACTERISTIC_UUID);
  ble_client.registCharacteristic(UNKNOWN_SERVICE_UUID, UNKNOWN_UUID);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (ble_client.connected()) {
    BLE_Callback_Data data = ble_client.getData();
    if (data.data_length != 0)
    {
      // if (strcmp(data.UUID, CHARACTERISTIC_UUID) == 0) {
      if (strcmp(data.UUID, UNKNOWN_UUID) == 0) {
        Serial.print("Received data: ");
        for (int i = 0; i < data.data_length; i++) {
          Serial.printf("%02X ", data.data[i]);
        }
        Serial.println();
      }
    }
  }
  else {
    Serial.println("Reconnecting...");
    ble_client.reconnect();
  }
}
