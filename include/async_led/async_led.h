#ifndef ASYNC_LED_H
#define ASYNC_LED_H

#include <Arduino.h>
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