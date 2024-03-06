//Used to handle actions by the esp, such as lighting leds reading remote codes etc

#include "PeripheralHandler.h"

//Used to make an LED blink at a specific frequency to provide some sort of validation
void blink(uint8_t PIN, float frequency){
    const unsigned long interval = 1000 / frequency;
    unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        digitalWrite(PIN, !digitalRead(PIN));
  }
}