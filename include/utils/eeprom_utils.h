#ifndef EEPROM_UTILS_H
#define EEPROM_UTILS_H
/**
 * @file utils.h
 * @brief Contains utility functions for handling WiFi credentials and EEPROM operations.
 */

#include <EEPROM.h>
#include "utils/utils.h"

/**
 * @struct eepromconfig_t
 * @brief Structure to store configuration parameters in EEPROM.
 */
struct eepromconfig_t {
    char ssid[33]; /**< SSID of the WiFi network. */
    char password[64]; /**< Password of the WiFi network. */
    char ws_host[64]; /**< WebSocket Host. */
    char ws_url[8]; /**< WebSocket URL. */
    uint16_t ws_port; /**< WebSocket Port. */
    uint8_t checksum; /**< Checksum for data integrity. */
    bool run_server; /** < Run WebServer Setup if true >*/
};
extern eepromconfig_t eeprom_config;

/**
 * @struct WiFiCredentials
 * @brief Structure to store WiFi credentials.
 */
struct wificredentials_t{
    char ssid[33]; /**< SSID of the WiFi network. */
    char password[64]; /**< Password of the WiFi network. */
};
extern wificredentials_t wifi_credentials;

/**
 * @struct WebSocketCredentials
 * @brief Structure to store WebSocket credentials.
 */
struct websocketcredentials_t{
    char ws_host[128]; /**< WebSocket Host. */
    char ws_url[16]; /**< WebSocket URL. */
    uint16_t ws_port; /**< WebSocket Port. */
};
extern websocketcredentials_t websocket_credentials;

/**
 * @namespace EEPROMUtils
 * @brief Namespace containing utility functions for handling EEPROM operations, such as saving and loading configurations.
 */
namespace EEPROMUtils{
    /**
     * @brief Saves WiFi credentials to EEPROM.
     * 
     * This function saves the WiFi credentials, including the SSID, password, WebSocket host, WebSocket URL, and WebSocket port, to the EEPROM.
     * 
     * @param ssid The SSID of the WiFi network.
     * @param password The password of the WiFi network.
     * @param ws_host The host address of the WebSocket server.
     * @param ws_url The URL of the WebSocket server.
     * @param ws_port The port number of the WebSocket server.
     * 
     * @return True if the credentials were successfully saved, false otherwise.
     */
    bool saveConfig(const char * ssid, const char * password, const char * ws_host, const char * ws_url, uint16_t ws_port);

    /**
     * @brief Loads WiFi credentials from EEPROM.
     * 
     * This function loads the WiFi credentials from the EEPROM and stores them in the provided wificredentials_t structure.
     * 
     * @param eepromconfig Pointer to the eepromconfig_t structure to store the loaded credentials.
     * 
     * @return True if valid credentials were loaded, false otherwise.
     */
    bool loadConfig();

    /**
     * @brief Loads WiFi credentials from EEPROM.
     * 
     * This function loads the WiFi credentials from the EEPROM and stores them in the provided wificredentials_t structure.
     * 
     * @param wifi_credentials wificredentials_t structure to store the loaded credentials.
     * 
     * @return True if valid credentials were loaded, false otherwise.
     */
    bool loadWiFiCredentials();

    /**
     * @brief Updates WiFi credentials in EEPROM.
     * 
     * This function updates the WiFi credentials in the EEPROM with the provided wificredentials_t structure.
     * 
     * @param wifi_credentials wificredentials_t structure containing the updated credentials.
     * 
     * @return True if the credentials were successfully updated, false otherwise.
     */
    bool updateWiFiCredentials();

    /**
     * @brief  Updates WebSocket credentials in EEPROM.
     * 
     * This function updates the WebSocket credentials in the EEPROM with the provided websocketcredentials_t structure.
     * 
     * @return True if the credentials were successfully updated, false otherwise. 
     */
    bool updateWebSocketConfig();

    /**
     * @brief Loads WebSocket credentials from EEPROM.
     * 
     * This function loads the WebSocket credentials from the EEPROM and stores them in the provided websocketcredentials_t structure.
     * 
     * @return true 
     * @return false 
     */
    bool loadWebSocketConfig();

    bool resetConfig();

    bool bootWebserver();
}
#endif