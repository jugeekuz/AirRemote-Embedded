#ifndef ACTION_HANDLER_H
#define ACTION_HANDLER_H

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRsend.h>
class IRremote: public IRrecv, public IRsend{
public:
    IRremote(uint8_t INPUT_PIN,uint8_t OUTPUT_PIN);
    uint16_t getCode();
    void sendCode(uint16_t code);
};

void blink(uint8_t PIN, float frequency);

#endif 