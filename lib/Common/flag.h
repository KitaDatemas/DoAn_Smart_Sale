#ifndef CONTROL_FLAG_H
#define CONTROL_FLAG_H
#include <Arduino.h>
#include "freertos/event_groups.h"

extern EventGroupHandle_t ble_event_group; // Used for ble event notification, LSB is used for data ready

#endif  // CONTROL_FLAG_H