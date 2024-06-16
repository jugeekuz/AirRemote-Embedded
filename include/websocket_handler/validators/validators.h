#ifndef VALIDATORS_H
#define VALIDATORS_H

#include <string>
#include <Arduino.h>
#include <ArduinoJson.h>

/**
 * @brief Namespace containing functions for validating messages.
 */
namespace MessageValidators {
    /**
     * @brief Checks the validity of a message, throws error if its' not valid.
     * 
     * @param request_payload The payload of the message to be checked.
     */
    void checkMessage(uint8_t *request_payload);
}

/**
 * @brief Namespace containing functions for validating commands.
 */
namespace CommandValidators {
    /**
     * @brief Checks the validity of a command.
     * 
     * @param request_payload The payload of the command to be checked.
     * @return True if the payload is indeed a command, false otherwise and error if its' not valid.
     */
    bool checkCommand(uint8_t *request_payload);

    /**
     * @brief Checks the validity of a read command.
     * 
     * @param request_payload The payload of the read command to be checked.
     * @return True if the payload is indeed a `read` command, false otherwise and error if its' not valid.
     */
    bool checkReadCommand(uint8_t *request_payload);

    /**
     * @brief Checks the validity of an execute command.
     * 
     * @param request_payload The payload of the execute command to be checked.
     * @return True if the payload is indeed an `execute` command, false otherwise and error if its' not valid.
     */
    bool checkExecuteCommand(uint8_t *request_payload);
}

/**
 * @brief Namespace containing functions for validating errors.
 */
namespace ErrorValidators {
    /**
     * @brief Checks the validity of an error message.
     * 
     * @param request_payload The payload of the error to be checked.
     * @return True if the payload is indeed an `error` message, false otherwise and error if its' not valid.
     */
    bool checkError(uint8_t *request_payload);
}

#endif
