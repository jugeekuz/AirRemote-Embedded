#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
namespace Utils {
    void arrayToString(volatile uint16_t* input, size_t len, char* buffer);
    uint8_t calculateChecksum(uint8_t* data, size_t len);
    void stringToArray(const char * input, size_t len, uint16_t* buffer);
    String wpspin2string(uint8_t a[]);
    
}
struct BlinkParameters {
    uint8_t pin;
    uint8_t frequency;
    uint8_t times;
};
struct SetStateParameters {
    uint8_t pin;
    uint8_t val;
};
class LED {
public:
    LED(const uint8_t pin);
    void blink(uint8_t frequency, uint8_t times);
    void setState(uint8_t val);

private:
    uint8_t pin;
    static void blinkTask(void* parameter);
    static void setStateTask(void* parameter);
    static TaskHandle_t blinkTaskHandle;  // Handle for the blink task
};

#endif