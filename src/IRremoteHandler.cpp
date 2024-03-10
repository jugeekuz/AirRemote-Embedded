/*
Class that is used to receive and send IR codes of different protocols.
*/

#include "IRremoteHandler.h"



IRremote::IRremote(uint8_t INPUT_PIN,uint8_t OUTPUT_PIN): IRrecv(INPUT_PIN), IRsend(OUTPUT_PIN){
    enableIRIn();
    begin();
}

//Function that is used to receive 16-bit code.
decode_results IRremote::getCode(){
    decode_results results;
    uint32_t total_delay = 0;
    while(total_delay<500){
        if(decode(&results)){ 
            delay(10);
            resume();
            return results;
        }
        delay(50);
        total_delay += 50;
    }
    return results;
}


void IRremote::sendCode(uint64_t code){
    sendNEC(code, 32);
    Serial.println("Succesfully sent code.");
    return;
}
