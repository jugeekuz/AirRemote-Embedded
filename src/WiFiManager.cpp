//Used to connect to the WiFi


#include "WiFiManager.h"

WiFiManager::WiFiManager(): WiFiMulti(), WPS(true){}

WiFiManager::WiFiManager(const char* WiFiSSID, const char* WiFiPass): WiFiMulti(), SSID(WiFiSSID), passphrase(WiFiPass), WPS(false) {}

//Connect via WPS to be implemented by the press of a button
uint8_t WiFiManager::attemptConnect() {
    if (WPS){
        Serial.println("Not Implemented yet error.");
    }else{
        addAP(SSID, passphrase);
    }
    return run();
}

void WiFiManager::connect() {
    while (attemptConnect() != WL_CONNECTED){
        delay(100);
    }
}