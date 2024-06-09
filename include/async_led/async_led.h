#ifndef ASYNC_LED_H
#define ASYNC_LED_H

#include <Arduino.h>

/**
 * @brief The AsyncLED class provides a way to control an LED asynchronously.
 */
class AsyncLED {
public:
    /**
     * @brief Constructs an AsyncLED object with the specified pin.
     * 
     * @param pin The pin number to which the LED is connected.
     */
    AsyncLED(const uint8_t pin);

    /**
     * @brief Destroys the AsyncLED object.
     */
    ~AsyncLED();

    /**
     * @brief Blinks the LED with the specified frequency and number of times.
     * 
     * @param frequency The frequency of the LED blinking in Hz.
     * @param times The number of times the LED should blink.
     */
    void blink(uint8_t frequency, uint8_t times);

    /**
     * @brief Blinks the LED with the specified frequency indefinitely.
     * 
     * @param frequency The frequency of the LED blinking in Hz.
     */
    void blink(uint8_t frequency);

    /**
     * @brief Sets the state of the LED.
     * 
     * @param val The value to set the LED state to (`LOW` for off, `HIGH` for on).
     */
    void setState(uint8_t val);

private:
    uint8_t pin;  // The pin number to which the LED is connected
    uint8_t blink_frequency;  // The frequency of the LED blinking in Hz
    uint8_t blink_times;  // The number of times the LED should blink
    static void blinkTask(void* parameter);  // Task function for blinking the LED
    static void blinkForeverTask(void* parameter);  // Task function for blinking the LED indefinitely
    static TaskHandle_t xRunningHandle;  // Handle for the blink task
    void deleteTask();  // Deletes the blink task
};

#endif