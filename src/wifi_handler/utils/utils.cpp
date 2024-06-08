#include "wifi_handler/utils/utils.h"
namespace WiFiUtils
{
    //Function that saves WiFi credentials to EEPROM flash memory along with a checksum.
    bool saveEEPROMCredentials(WiFiCredentials &credentials) {

        Serial.println("Saving WiFi Credentials to EEPROM memory...");

        eepromcredentials_t eeprom_credentials;
        
        memset(&eeprom_credentials.ssid, '\0', sizeof(eeprom_credentials.ssid));
        memset(&eeprom_credentials.password, '\0', sizeof(eeprom_credentials.password));

        strncpy(eeprom_credentials.ssid, credentials.ssid, strlen(credentials.ssid));
        strncpy(eeprom_credentials.password, credentials.password, strlen(credentials.password));

        eeprom_credentials.checksum = \
            Utils::calculateChecksum((uint8_t*)eeprom_credentials.ssid, sizeof(eeprom_credentials.ssid)) ^ 
            Utils::calculateChecksum((uint8_t*)eeprom_credentials.password, sizeof(eeprom_credentials.password));
        
        EEPROM.begin(sizeof(eepromcredentials_t));
        EEPROM.put(0, eeprom_credentials);

        bool success = EEPROM.commit();
        if (!success) {
            Serial.println("Failed to save credentials to EEPROM.");
        }
        return success;
    }

    //Function that loads WiFi credentials from EEPROM flash memory and checks if they're valid.
    bool loadEEPROMCredentials(WiFiCredentials *wifi_credentials) {

        Serial.println("Loading credentials from EEPROM memory..");

        try{
            eepromcredentials_t eeprom_credentials;

            EEPROM.begin(sizeof(eepromcredentials_t));
            EEPROM.get(0, eeprom_credentials);

            uint8_t checksum = \
                Utils::calculateChecksum((uint8_t*)eeprom_credentials.ssid, sizeof(eeprom_credentials.ssid)) ^ 
                Utils::calculateChecksum((uint8_t*)eeprom_credentials.password, sizeof(eeprom_credentials.password));
            
            if (checksum == eeprom_credentials.checksum){
                strncpy(wifi_credentials->ssid, eeprom_credentials.ssid, sizeof(eeprom_credentials.ssid));
                strncpy(wifi_credentials->password, eeprom_credentials.password, sizeof(eeprom_credentials.password));
                return true;
            }
        } catch(...) {

            Serial.println("Failed to retrieve data from EEPROM memory.");

        }
            return false;
        }   
    } 
