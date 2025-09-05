#include <Arduino.h>
#include "Scale_Service.h"

void setup() {
  Serial.begin (9600);
  Scale_Service_run ();
}

void loop() {
  // put your main code here, to run repeatedly:
  Scale_Value value = Scale_Service_getScaleValue();
  Serial.print("Weight: ");
  Serial.print(value.weight);
  Serial.print(value.is_lbs ? " lbs" : " kg");
  Serial.printf(". Timestamp: %04d-%02d-%02d %02d:%02d:%02d\n",
                value.timestamp.year,
                value.timestamp.month,
                value.timestamp.day,
                value.timestamp.hour,
                value.timestamp.minute,
                value.timestamp.second);
  delay(2000);
}