#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFiMulti.h>

class WiFiManager : public WiFiMulti {
public:
    // Constructors
    WiFiManager(); // Default constructor
    WiFiManager(const char* WiFiSSID, const char* WiFiPass); 

    // Member function to connect to WiFi
    uint8_t attemptConnect();
    void connect();
protected:
    const char* SSID; 
    const char* passphrase;
    bool WPS;

};

#endif 
