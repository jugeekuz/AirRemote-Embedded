//Used to handle actions by the esp, such as lighting leds reading remote codes etc


#include "ActionHandler.h"

decode_results results;

//Used as an IR remote
IRremote::IRremote(uint8_t INPUT_PIN,uint8_t OUTPUT_PIN): IRrecv(INPUT_PIN), IRsend(OUTPUT_PIN){
    enableIRIn();
    begin();
}

uint16_t IRremote::getCode(){
    if(decode(&results)){ 
        delay(10);
        resume();
        return results.value;
    }
    return getCode();
}

void IRremote::sendCode(uint16_t code){
    sendNEC(results.value, 32);
    return;
}

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