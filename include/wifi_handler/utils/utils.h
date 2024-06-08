#include <EEPROM.h>
#include "utils/utils.h"

struct eepromcredentials_t {
    char ssid[33];
    char password[64];
    uint8_t checksum;
};

struct WiFiCredentials{
    char * ssid;
    char * password;
};

namespace WiFiUtils{
    bool saveEEPROMCredentials(WiFiCredentials &credentials);
    bool loadEEPROMCredentials(WiFiCredentials *wifi_credentials);
}