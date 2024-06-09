#include "ir_remote/ir_remote.h"
#define BIG_IR_BUFFER_SIZE 1024

#define RECEIVING_CODE_FREQUENCY 5
#define RECEIVED_CODE_FREQUENCY 10
#define RECEIVED_CODE_TIMES 5

#define SENDING_CODE_FREQUENCY 10
#define SENDING_CODE_TIMES 5

IRremote::IRremote(uint8_t INPUT_PIN, uint8_t OUTPUT_PIN,  AsyncLED** feedbackLED): IRrecv(INPUT_PIN, BIG_IR_BUFFER_SIZE), IRsend(OUTPUT_PIN), feedbackLED(feedbackLED){
    enableIRIn();
    begin();
}
IRremote::IRremote(uint8_t INPUT_PIN,uint8_t OUTPUT_PIN, uint8_t timeout,  AsyncLED** feedbackLED): IRrecv(INPUT_PIN, BIG_IR_BUFFER_SIZE, timeout), IRsend(OUTPUT_PIN){
    enableIRIn();
    begin();
}

// This function accepts IR codes for a `timeout_s` seconds, and saves it in the `results` variable.
bool IRremote::getCode(decode_results *results, uint16_t timeout_s){
    unsigned long startTime = millis();

    (*feedbackLED)->blink(RECEIVING_CODE_FREQUENCY);

    Serial.println("[IRremote][INFO] Waiting for code...");
    while (millis() - startTime < timeout_s*1000){
        if (decode(results)) {
            resume();
            Serial.println("[IRremote][INFO] Received code.");

            (*feedbackLED)->blink(RECEIVED_CODE_FREQUENCY, RECEIVED_CODE_TIMES);
 
            return true;  
        }
    }
    (*feedbackLED)->setState(LOW);

    Serial.println("[IRremote][ERROR] Code reading has reached a timeout.");
    return false;
}

// This function accepts IR codes and saves it in the `results` variable.
bool IRremote::getCode(decode_results *results){
    
    if (decode(results)) {
        resume();
        (*feedbackLED)->blink(RECEIVED_CODE_FREQUENCY, RECEIVED_CODE_TIMES);
        return true;
    }
    return false;
}

// This function sends the IR code that is stored in the `raw_array` variable in raw format.
void IRremote::sendCode(uint16_t * raw_array, size_t raw_len){
    sendRaw(raw_array, raw_len, 38);

    (*feedbackLED)->blink(SENDING_CODE_FREQUENCY, SENDING_CODE_TIMES);

    Serial.println("[IRremote][INFO] Succesfully sent code.");
    return;
}


char* IRremote::resultsToPayload(decode_results *results, char *buffer){
    return buffer;
}