#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#define ESP_WPS_MODE      WPS_TYPE_PBC
#define ESP_MANUFACTURER  "ESPRESSIF"
#define ESP_MODEL_NUMBER  "ESP32"
#define ESP_MODEL_NAME    "ESPRESSIF IOT"
#define ESP_DEVICE_NAME   "ESP STATION"

#include <EEPROM.h>
#include <WiFiMulti.h>
#include "WiFi.h"
#include "esp_wps.h"
#include "./utils/utils.h"

static esp_wps_config_t config;

typedef enum{
    CONNECTED, 
    DISCONNECTED, 
    WPS_CONNECTION, 
    WPS_ERR} State;


class WiFiHandler {
public:
    WiFiHandler(); 
    ~WiFiHandler();
    void setCredentials(const char* WiFiSSID, const char* WiFiPass);
    void setCredentials(WiFiCredentials credentials);
    static void wpsStop();
    void WPSCallback(WiFiEvent_t event, arduino_event_info_t info);
    bool saveWiFiCredentials();
    bool loadWiFiCredentials();
    void connectWPS();
    void connect(uint16_t timeout_s);
    String macAddress();
    wl_status_t status();
    State state = DISCONNECTED;
    
private:
    WiFiCredentials *wifi_credentials;
    const char* SSID; 
    const char* password;
};

#endif 
