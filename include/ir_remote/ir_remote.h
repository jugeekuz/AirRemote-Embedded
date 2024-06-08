#ifndef IR_REMOTE_H
#define IR_REMOTE_H

#define BIG_IR_BUFFER_SIZE 1024

#include <IRremoteESP8266.h>
#include <IRutils.h>
#include <IRrecv.h>
#include <IRsend.h>
#include "utils/utils.h"
#include "async_led/async_led.h"
/**
 * @brief A class to read and send IR remote control commands.
 * 
 * This class provides methods to send or read IR commands
 * and format the appropriately to send as message.
 */
class IRremote: public IRrecv, public IRsend{
public:
    IRremote(uint8_t INPUT_PIN,uint8_t OUTPUT_PIN, LED** feedbackLED);
    IRremote(uint8_t INPUT_PIN,uint8_t OUTPUT_PIN, uint8_t timeout, LED** feedbackLED);
    bool getCode(decode_results * results, uint16_t timeout_s);
    bool getCode(decode_results *results);
    void sendCode(uint16_t * raw_array, size_t raw_len);
    char* resultsToPayload(decode_results *results, char *buffer);
private:
    LED** feedbackLED;
};

#endif 