#ifndef ASYNC_BUTTON_H
#define ASYNC_BUTTON_H

#define BUTTON_DELAY_MS 500


#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>



class AsyncButton {
public:
    AsyncButton(uint8_t pin);
    void setButtonListener(void (*callback)());
    void setButtonListener(void (*callback)(), UBaseType_t uxPriority);

private:
    uint8_t pin;
    void (*callback)();

    static void buttonTask(void *param);
};

#endif 
