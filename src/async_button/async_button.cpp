#include "async_button/async_button.h"

AsyncButton::AsyncButton(uint8_t pin) : pin(pin), callback(nullptr) {
    pinMode(pin, INPUT_PULLUP);
}

void AsyncButton::setButtonListener(void (*callback)()) {
    this->callback = callback;
    xTaskCreatePinnedToCore(
        AsyncButton::buttonTask,    // Task function
        "ButtonTask",               // Name of task
        2048,                       // Stack size of task
        this,                       // Parameter of the task
        1,                          // Priority of the task
        NULL,                       // Task handle
        1                           // Core where the task should run
    );
}

void AsyncButton::buttonTask(void *param) {
    AsyncButton *params = static_cast<AsyncButton*>(param);

    if (params == NULL) {
        vTaskDelete(NULL);
        return;
    }

    int lastState = LOW;

    while (true) {
        int currentState = digitalRead(params->pin);
        if (currentState == LOW && lastState == HIGH) {
            if (params->callback != nullptr) {
                params->callback();
            }
        }
        lastState = currentState;
        vTaskDelay(BUTTON_DELAY_MS/portTICK_PERIOD_MS);
    }
}
