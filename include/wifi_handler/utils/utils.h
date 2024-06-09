/**
 * @file utils.h
 * @brief Contains utility functions for handling WiFi credentials and EEPROM operations.
 */

#include <EEPROM.h>
#include "utils/utils.h"

/**
 * @struct eepromcredentials_t
 * @brief Structure to store WiFi credentials in EEPROM.
 */
struct eepromcredentials_t {
    char ssid[33]; /**< SSID of the WiFi network. */
    char password[64]; /**< Password of the WiFi network. */
    uint8_t checksum; /**< Checksum for data integrity. */
};

/**
 * @struct WiFiCredentials
 * @brief Structure to store WiFi credentials.
 */
struct WiFiCredentials{
    char * ssid; /**< SSID of the WiFi network. */
    char * password; /**< Password of the WiFi network. */
};

/**
 * @namespace WiFiUtils
 * @brief Namespace containing utility functions for WiFi operations.
 */
namespace WiFiUtils{
    /**
     * @brief Saves WiFi credentials to EEPROM.
     * @param credentials The WiFi credentials to be saved.
     * @return True if the credentials were successfully saved, false otherwise.
     */
    bool saveEEPROMCredentials(WiFiCredentials &credentials);

    /**
     * @brief Loads WiFi credentials from EEPROM.
     * @param wifi_credentials Pointer to the WiFiCredentials structure to store the loaded credentials.
     * @return True if the credentials if valid credentials were loaded, false otherwise.
     */
    bool loadEEPROMCredentials(WiFiCredentials *wifi_credentials);
}