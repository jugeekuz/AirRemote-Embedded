#include "IRremote.h"

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
