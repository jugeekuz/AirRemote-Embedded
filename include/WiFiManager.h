#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#define ESP_WPS_MODE      WPS_TYPE_PBC
#define ESP_MANUFACTURER  "ESPRESSIF"
#define ESP_MODEL_NUMBER  "ESP32"
#define ESP_MODEL_NAME    "ESPRESSIF IOT"
#define ESP_DEVICE_NAME   "ESP STATION"

#define EEPROM_VALIDITY_FLAG 0xDEADBAD9

#include <EEPROM.h>
#include <WiFiMulti.h>
#include "WiFi.h"
#include "esp_wps.h"

static esp_wps_config_t config;

typedef enum{
    CONNECTED, 
    DISCONNECTED, 
    WPS_CONNECTION, 
    WPS_ERR} State;


struct WiFiCredentials {
    char ssid[32];
    char password[64];
    uint32_t flag;
};
void wpsInitConfig();
void wpsStart();
void wpsStop();
String wpspin2string(uint8_t a[]);
void WiFiEvent(WiFiEvent_t event, arduino_event_info_t info);


class WiFiManager : public WiFiMulti {
public:
    // Constructors
    WiFiManager(); 

    void setCredentials(const char* WiFiSSID, const char* WiFiPass);

    static void saveWiFiCredentials(const char *ssid,const char *password);
    static void loadWiFiCredentials(WiFiCredentials& credentials);
    // Member function to connect to WiFi
    void connectWPS();
    void connect();
protected:
    const char* SSID; 
    const char* passphrase;
    bool WPS;

};

#endif 
