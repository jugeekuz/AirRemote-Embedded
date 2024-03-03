#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h> 
#include <WiFiMulti.h>
#include <esp_wps.h>

void wpsInitConfig();
void wpsStart();
void wpsStop();
String wpspin2string(uint8_t a[]);
void WiFiEvent(WiFiEvent_t event, arduino_event_info_t info);

class WiFiManager : public WiFiMulti {
public:
    // Constructors
    WiFiManager(); // Default constructor
    WiFiManager(const char* WiFiSSID, const char* WiFiPass); 

    // Member function to connect to WiFi
    wl_status_t attemptConnect();
    void connect();
protected:
    const char* SSID; 
    const char* passphrase;
    bool WPS;

};

#endif 
