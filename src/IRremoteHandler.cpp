/*
Class that is used to receive and send IR codes of different protocols.
*/

#include "IRremoteHandler.h"

IRremote::IRremote(uint8_t INPUT_PIN, uint8_t OUTPUT_PIN,  LED** feedbackLED): IRrecv(INPUT_PIN, BIG_IR_BUFFER_SIZE), IRsend(OUTPUT_PIN), feedbackLED(feedbackLED){
    enableIRIn();
    begin();
}
IRremote::IRremote(uint8_t INPUT_PIN,uint8_t OUTPUT_PIN, uint8_t timeout,  LED** feedbackLED): IRrecv(INPUT_PIN, BIG_IR_BUFFER_SIZE, timeout), IRsend(OUTPUT_PIN){
    enableIRIn();
    begin();
}

bool IRremote::getCode(decode_results *results, uint16_t timeout_s){
    unsigned long startTime = millis();

    (*feedbackLED)->blink(10, 10);
    while (millis() - startTime < timeout_s*1000){
        if (decode(results)) {
            resume();
            Serial.println("Received code");
            
            (*feedbackLED)->blink(6, 10);

            return true;  
        }
    }
    Serial.println("Code reading has reached a timeout");
    (*feedbackLED)->blink(1, 5);
    return false;
}
bool IRremote::getCode(decode_results *results){

    (*feedbackLED)->blink(2, 5);

    if (decode(results)) {
        resume();
        return true;
        (*feedbackLED)->blink(5, 6);  
    }
    return false;
}

void IRremote::sendCode(uint16_t * raw_array, size_t raw_len){
    sendRaw(raw_array, raw_len, 38);
    Serial.println("Succesfully sent code.");
    return;
}


char* IRremote::resultsToPayload(decode_results *results, char *buffer){
    return buffer;
}