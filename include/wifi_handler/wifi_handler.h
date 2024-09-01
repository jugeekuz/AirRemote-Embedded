#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#define ESP_WPS_MODE      WPS_TYPE_PBC
#define ESP_MANUFACTURER  "ESPRESSIF"
#define ESP_MODEL_NUMBER  "ESP32"
#define ESP_MODEL_NAME    "ESPRESSIF IOT"
#define ESP_DEVICE_NAME   "ESP STATION"

#include "WiFi.h"
#include "esp_wps.h"
#include "./utils/utils.h"
#include "./utils/eeprom_utils.h"
#include "async_led/async_led.h"

static esp_wps_config_t wps_config;

/**
 * @enum State
 * @brief Represents the state of the WiFi connection.
 */
typedef enum{
    CONNECTED, /**< Connected to WiFi. */
    DISCONNECTED, /**< Disconnected from WiFi. */
    WPS_CONNECTION, /**< Connecting using WPS. */
    WPS_ERR /**< Error occurred during WPS connection. */
} State;

/**
 * @class WiFiHandler
 * @brief Handles WiFi connection and credentials.
 */
class WiFiHandler : public WiFiClass {
public:
    /**
     * @brief Default constructor for WiFiHandler.
     */
    WiFiHandler(AsyncLED *led); 

    /**
     * @brief Destructor for WiFiHandler.
     */
    ~WiFiHandler();

    /**
     * @brief Sets the WiFi credentials.
     * @param WiFiSSID The SSID of the WiFi network.
     * @param WiFiPass The password of the WiFi network.
     */
    void setCredentials(const char* WiFiSSID, const char* WiFiPass);

    /**
     * @brief Sets the WiFi credentials.
     */
    void setCredentials();

    /**
     * @brief Stops the WPS connection process.
     */
    static void wpsStop();

    /**
     * @brief Callback function for WPS events.
     * @param event The WiFi event.
     * @param info Additional information about the event.
     */
    void WPSCallback(WiFiEvent_t event, arduino_event_info_t info);

    /**
     * @brief Saves the WiFi credentials to EEPROM.
     * @return True if the credentials were successfully saved, false otherwise.
     */
    bool saveWiFiCredentials();

    /**
     * @brief Loads the WiFi credentials from EEPROM.
     * @return True if the credentials were successfully loaded, false otherwise.
     */
    bool loadWiFiCredentials();

    /**
     * @brief Connects to the WiFi network using WPS.
     */
    void connectWPS();

    /**
     * @brief Connects to the WiFi network.
     * @param timeout_s The connection timeout in seconds.
     */
    void connect(uint16_t timeout_s);

    State state = DISCONNECTED; /**< The current state of the WiFi connection. */
    
private:
    AsyncLED *led; /**< The LED used to indicate the WiFi connection status. */
    char SSID[33]; /**< The SSID of the WiFi network. */
    char password[65]; /**< The password of the WiFi network. */
};

#endif
