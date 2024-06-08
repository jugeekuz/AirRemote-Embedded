#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
namespace Utils {
    void arrayToString(volatile uint16_t* input, size_t len, char* buffer);
    uint8_t calculateChecksum(uint8_t* data, size_t len);
    void stringToArray(const char * input, size_t len, uint16_t* buffer);
    String wpspin2string(uint8_t a[]);
    
}
#endif