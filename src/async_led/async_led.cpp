#include "async_led/async_led.h"

TaskHandle_t LED::blinkTaskHandle = NULL;  // Initialize the static task handle

LED::LED(const uint8_t pin): pin(pin) {
    pinMode(pin, OUTPUT);
}

void LED::blink(uint8_t frequency, uint8_t times) {

    BlinkParameters* params = new BlinkParameters{this->pin, frequency, times};

    xTaskCreate(
        blinkTask,     // Task function
        "BlinkTask",   // Name of the task (for debugging)
        1024,          // Stack size
        params,        // Parameters to pass to the task
        1,             // Task priority
        &blinkTaskHandle // Task handle
    );
}

void LED::setState(uint8_t val) {

    SetStateParameters* params = new SetStateParameters{this->pin, val};

    xTaskCreate(
        setStateTask, // Task function
        "SetStateTask", // Name of the task (for debugging)
        1024,           // Stack size
        params,         // Parameters to pass to the task
        1,              // Task priority
        NULL            // Task handle
    );
}

void LED::blinkTask(void* parameter) {

    BlinkParameters* params = static_cast<BlinkParameters*>(parameter);
    int delayTime = 1000 / params->frequency;

    for(int i = 0; i < params->times; i++) {
        digitalWrite(params->pin, HIGH);
        vTaskDelay(delayTime / portTICK_PERIOD_MS);
        digitalWrite(params->pin, LOW);
        vTaskDelay(delayTime / portTICK_PERIOD_MS);
    }

    delete params;
    xTaskNotifyGive(blinkTaskHandle);  // Notify that the blink task is complete
    vTaskDelete(NULL);
}

void LED::setStateTask(void* parameter) {

    SetStateParameters* params = static_cast<SetStateParameters*>(parameter);
    
    // Wait for the blink task to complete if it is running
    if (blinkTaskHandle != NULL) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }

    digitalWrite(params->pin, params->val);

    delete params;
    vTaskDelete(NULL);
}
