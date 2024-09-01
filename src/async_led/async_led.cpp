#include "async_led/async_led.h"

AsyncLED::AsyncLED(const uint8_t pin): pin(pin) {
    pinMode(pin, OUTPUT);
}

AsyncLED::~AsyncLED() {
    deleteTask();
}

// Static task handle for running task
TaskHandle_t AsyncLED::xRunningHandle = NULL;

// Delete the running task
void AsyncLED::deleteTask() {
    if (xRunningHandle != NULL) {
        vTaskDelete(xRunningHandle);
        xRunningHandle = NULL;
    }
}

// Set the state of the LED
void AsyncLED::setState(uint8_t val) {
    deleteTask();
    digitalWrite(this->pin, val);
}

// Blink the LED for a specified frequency and number of times
void AsyncLED::blink(uint8_t frequency, uint8_t times) {
    deleteTask();
    
    this->blink_frequency = frequency;
    this->blink_times = times;

    if( xTaskCreate(
            blinkTask,     // Task function
            "BlinkTask",   // Name of the task (for debugging)
            2048,          // Stack size
            this,        // Parameters to pass to the task
            1,             // Task priority
            &xRunningHandle // Task handle
        ) != pdPASS) {
            Serial.println("[AsyncLED][ERROR] Failed to create the task. Insufficient heap memory.");
    }
}

// Blink the LED forever with a specified frequency
void AsyncLED::blink(uint8_t frequency) {
    deleteTask();
    
    this->blink_frequency = frequency;

    if (xTaskCreate(
            blinkForeverTask,     // Task function
            "BlinkTaskForever",   // Name of the task (for debugging)
            2048,          // Stack size
            this,        // Parameters to pass to the task
            1,             // Task priority
            &xRunningHandle // Task handle
        ) != pdPASS) {
            Serial.println("[AsyncLED][ERROR] Failed to create the task. Insufficient heap memory.");
    }
}

// Task function for blinking the LED a specified number of times
void AsyncLED::blinkTask(void* parameter) {

    AsyncLED* params = static_cast<AsyncLED*>(parameter);

    if (params == NULL) {
        vTaskDelete(NULL);
        return;
    }
    int delayTime = 1000 / params->blink_frequency;

    for(int i = 0; i < params->blink_times; i++) {
        digitalWrite(params->pin, HIGH);
        vTaskDelay(delayTime / portTICK_PERIOD_MS);
        digitalWrite(params->pin, LOW);
        vTaskDelay(delayTime / portTICK_PERIOD_MS);
    }

    xRunningHandle = NULL;
    vTaskDelete(NULL);
}

// Task function for blinking the LED forever
void AsyncLED::blinkForeverTask(void* parameter) {

    AsyncLED* params = static_cast<AsyncLED*>(parameter);

    if (params == NULL) {
        vTaskDelete(NULL);
        return;
    }
    int delayTime = 1000 / params->blink_frequency;

    for(;;) {
        digitalWrite(params->pin, HIGH);
        vTaskDelay(delayTime / portTICK_PERIOD_MS);
        digitalWrite(params->pin, LOW);
        vTaskDelay(delayTime / portTICK_PERIOD_MS);
    }
}