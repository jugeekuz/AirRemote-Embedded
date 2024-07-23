#include "utils/eeprom_utils.h"

struct eepromconfig_t eeprom_config;
struct wificredentials_t wifi_credentials;
struct websocketcredentials_t websocket_credentials;



namespace EEPROMUtils{
    //Function that saves WiFi & WebSocket configuration parameters to EEPROM flash memory.
    bool saveConfig(const char * ssid, const char * password, const char * ws_host, const char * ws_url, uint16_t ws_port) {

        Serial.println("[EEPROM][INFO] Saving WiFi Credentials to EEPROM memory...");
        
        memset(&eeprom_config.ssid, '\0', sizeof(eeprom_config.ssid));
        memset(&eeprom_config.password, '\0', sizeof(eeprom_config.password));
        memset(&eeprom_config.ws_host, '\0', sizeof(eeprom_config.ws_host));
        memset(&eeprom_config.ws_url, '\0', sizeof(eeprom_config.ws_url));
        
        strncpy(eeprom_config.ssid, ssid, strlen(ssid));
        strncpy(eeprom_config.password, password, strlen(password));
        strncpy(eeprom_config.ws_host, ws_host, strlen(ws_host));
        strncpy(eeprom_config.ws_url, ws_url, strlen(ws_url));
        eeprom_config.ws_port = ws_port;
        
        
        eeprom_config.checksum = \
            Utils::calculateChecksum((uint8_t*)eeprom_config.ssid, sizeof(eeprom_config.ssid)) ^ 
            Utils::calculateChecksum((uint8_t*)eeprom_config.password, sizeof(eeprom_config.password)) ^
            Utils::calculateChecksum((uint8_t*)eeprom_config.ws_host, sizeof(eeprom_config.ws_host)) ^
            Utils::calculateChecksum((uint8_t*)eeprom_config.ws_url, sizeof(eeprom_config.ws_url)) ^
            Utils::calculateChecksum((uint8_t*)&eeprom_config.ws_port, sizeof(eeprom_config.ws_port));

        EEPROM.begin(sizeof(eepromconfig_t));
        EEPROM.put(0, eeprom_config);
        
        bool success = EEPROM.commit();
        if (!success) {
            Serial.println("[EEPROM][ERROR] Failed to save credentials to EEPROM.");
        }
        return success;
    }

    //Function that loads WiFi & WebSocket configuration parameters from EEPROM flash memory.
    bool loadConfig() {
        try{

            //This is necessary to ensure the checksum is the same as the one calculated from the data.
            memset(&eeprom_config.ssid, '\0', sizeof(eeprom_config.ssid));
            memset(&eeprom_config.password, '\0', sizeof(eeprom_config.password));
            memset(&eeprom_config.ws_host, '\0', sizeof(eeprom_config.ws_host));
            memset(&eeprom_config.ws_url, '\0', sizeof(eeprom_config.ws_url));

            EEPROM.begin(sizeof(eepromconfig_t));
            EEPROM.get(0, eeprom_config);

            Serial.println("[EEPROM][INFO] Credentials loaded from EEPROM memory.");

            uint8_t checksum = \
                Utils::calculateChecksum((uint8_t*)eeprom_config.ssid, sizeof(eeprom_config.ssid)) ^ 
                Utils::calculateChecksum((uint8_t*)eeprom_config.password, sizeof(eeprom_config.password)) ^
                Utils::calculateChecksum((uint8_t*)eeprom_config.ws_host, sizeof(eeprom_config.ws_host)) ^
                Utils::calculateChecksum((uint8_t*)eeprom_config.ws_url, sizeof(eeprom_config.ws_url)) ^
                Utils::calculateChecksum((uint8_t*)&eeprom_config.ws_port, sizeof(eeprom_config.ws_port));

            return (checksum == eeprom_config.checksum);
        } catch(...) {
            Serial.println("[EEPROM][ERROR] Failed to retrieve data from EEPROM memory.");
        }
        return false;
    }   

    //Function that loads WiFi credentials from EEPROM flash memory and stores them in the provided WiFiCredentials structure.
    bool loadWiFiCredentials() {
        if(loadConfig()) {
            memset(&wifi_credentials.ssid, '\0', sizeof(wifi_credentials.ssid));
            memset(&wifi_credentials.password, '\0', sizeof(wifi_credentials.password));
            
            strncpy(wifi_credentials.ssid, eeprom_config.ssid, strlen(eeprom_config.ssid));
            strncpy(wifi_credentials.password, eeprom_config.password, strlen(eeprom_config.password));
            return true;
        }
        return false;
    }

    //Function that updates WiFi credentials in EEPROM flash memory.
    bool updateWiFiCredentials() {
        Serial.printf("Ssid3 is : %s", wifi_credentials.ssid);
        Serial.printf("Password3 is : %s", wifi_credentials.password);
        if(loadConfig()) {
            Serial.printf("Ssid4 is : %s", wifi_credentials.ssid);
            Serial.printf("Password4 is : %s", wifi_credentials.password);
            return saveConfig(wifi_credentials.ssid, wifi_credentials.password, eeprom_config.ws_host, eeprom_config.ws_url, eeprom_config.ws_port);
        }
        return false;
    }

    //Function that loads WebSocket credentials from EEPROM flash memory and stores them in the provided WebSocketCredentials structure.
    bool updateWebSocketConfig() {
        if(loadConfig()) {
            return saveConfig(eeprom_config.ssid, eeprom_config.password, websocket_credentials.ws_host, websocket_credentials.ws_url, websocket_credentials.ws_port);
        }
        return false;
    }

    //Function that loads WebSocket credentials from EEPROM flash memory and stores them in the provided WebSocketCredentials structure.
    bool loadWebSocketConfig() {
        if(loadConfig()) {
            websocket_credentials.ws_host = eeprom_config.ws_host;
            websocket_credentials.ws_url = eeprom_config.ws_url;
            websocket_credentials.ws_port = eeprom_config.ws_port;
            return true;
        }
        return false;
    }
} 

