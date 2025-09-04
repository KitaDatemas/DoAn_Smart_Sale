#ifndef COMMON_H
#define COMMON_H
#include <Arduino.h>

#ifdef DEBUG
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
    #define DEBUG_PRINT(x) \
        do                 \
        {                  \
        } while (0)
    #define DEBUG_PRINTLN(x) \
        do                   \
        {                    \
        } while (0)
    #define DEBUG_PRINTF(...) \
        do                    \
        {                     \
        } while (0)
#endif // DEBUG

struct WEIGHT_SCALE_DATA {
    float weight;
    int day, month, year, hour, minute, second;
};
#endif  // COMMON_H