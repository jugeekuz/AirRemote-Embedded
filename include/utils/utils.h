#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>

/**
 * @namespace Utils
 * @brief Namespace containing utility functions for the project.
 */
namespace Utils {
    /**
     * @brief Converts an array of uint16_t to a string representation of the array.
     * 
     * @param input The input array of uint16_t.
     * @param len The length of the input array.
     * @param buffer The buffer to store the resulting string.
     */
    void arrayToString(volatile uint16_t* input, size_t len, char* buffer);

    /**
     * @brief Calculates the checksum of an array of uint16_t.
     * 
     * @param data The input array of uint16_t.
     * @param len The length of the input array.
     * @return The calculated checksum.
     */
    uint8_t calculateChecksum(uint8_t* data, size_t len);

    /**
     * @brief Converts a string representation of an array to an actual array of uint16_t.
     * 
     * @param input The input string.
     * @param len The length of the input string.
     * @param buffer The buffer to store the resulting array.
     */
    void stringToArray(const char* input, size_t len, uint16_t* buffer);

    /**
     * @brief Converts a WPS PIN to a string representation.
     * 
     * @param a The input array of uint8_t representing the WPS PIN.
     * @return The resulting string representation of the WPS PIN.
     */
    String wpspin2string(uint8_t a[]);
}

#endif