#ifndef IR_REMOTE_H
#define IR_REMOTE_H

#include <IRremoteESP8266.h>
#include <IRutils.h>
#include <IRrecv.h>
#include <IRsend.h>
#include "utils/utils.h"
#include "async_led/async_led.h"

/**
 * @brief The IRremote class provides an interface for receiving and sending infrared (IR) remote control signals.
 * 
 * This class inherits from the IRrecv and IRsend classes of the IRremoteESP8266 library, which provide the functionality
 * for receiving and sending IR signals respectively. It also uses the AsyncLED class from the async_led library for feedback LED control.
 */
class IRremote: public IRrecv, public IRsend{
public:
    /**
     * @brief Constructs an IRremote object with the specified input and output pins, and feedback LED array.
     * 
     * @param INPUT_PIN The input pin number for receiving IR signals.
     * @param OUTPUT_PIN The output pin number for sending IR signals.
     * @param feedbackLED A pointer to a pointer of an AsyncLED object used for feedback LED control.
     */
    IRremote(uint8_t INPUT_PIN, uint8_t OUTPUT_PIN, AsyncLED** feedbackLED);

    /**
     * @brief Constructs an IRremote object with the specified input and output pins, timeout value, and feedback LED array.
     * 
     * @param INPUT_PIN The input pin number for receiving IR signals.
     * @param OUTPUT_PIN The output pin number for sending IR signals.
     * @param timeout The timeout value in milliseconds for receiving IR signals.
     * @param feedbackLED A pointer to a pointer of an AsyncLED object used for feedback LED control.
     */
    IRremote(uint8_t INPUT_PIN, uint8_t OUTPUT_PIN, uint8_t timeout, AsyncLED** feedbackLED);

    /**
     * @brief Receives an IR signal and stores the decoded results in the provided decode_results object.
     * 
     * @param results A pointer to the `decode_results` object where the decoded results will be stored.
     * @param timeout_s The timeout value in seconds for receiving the IR signal.
     * @return true if an IR signal was successfully received within the specified timeout, false otherwise.
     */
    bool getCode(decode_results* results, uint16_t timeout_s);

    /**
     * @brief Receives an IR signal and stores the decoded results in the provided decode_results object.
     * 
     * @param results A pointer to the `decode_results` object where the decoded results will be stored.
     * @return true if an IR signal was successfully received, false otherwise.
     */
    bool getCode(decode_results* results);

    /**
     * @brief Sends an IR signal using the provided raw_array and raw_len.
     * 
     * @param raw_array An array of raw IR signal data.
     * @param raw_len The length of the raw_array.
     */
    void sendCode(uint16_t* raw_array, size_t raw_len);

    /**
     * @brief Converts the decoded results to a payload string format.
     * 
     * @param results A pointer to the decode_results object containing the decoded IR signal.
     * @param buffer A character buffer to store the payload string.
     * @return A pointer to the buffer containing the payload string.
     */
    char* resultsToPayload(decode_results* results, char* buffer);

private:
    AsyncLED** feedbackLED; //< A pointer to a pointer of an AsyncLED object used for feedback LED control. 
};

#endif