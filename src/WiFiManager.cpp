/* 
Class used to connect to WiFi.
Can connect to WiFi through SSID and password.
Can also connect through WPS and the credentials are saved to Flash.
*/

#include "WiFiManager.h"
void WiFiManager::wpsStop(){
    if(esp_wifi_wps_disable()){
    	Serial.println("WPS Disable Failed");
    }
}

void WiFiManager::WiFiEvent(WiFiEvent_t event, arduino_event_info_t info){
  switch(event){
    case ARDUINO_EVENT_WIFI_STA_START:
      Serial.println("Station Mode Started");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("Connected to WiFi: " + String(WiFi.SSID()));
      Serial.print("Got IP: ");
      Serial.println(WiFi.localIP());
      saveWiFiCredentials();
      state = CONNECTED;
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("Disconnected from station, attempting reconnection");
      WiFi.reconnect();
      break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
      Serial.println("WPS Successfull, stopping WPS and connecting to: " + String(WiFi.SSID()));
      wpsStop();
      delay(10);
      WiFi.begin();
      break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
      Serial.println("WPS Failed, retrying");
      wpsStop();
      state = WPS_ERR;
      break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
      Serial.println("WPS Timedout, retrying");
      wpsStop();
      state = WPS_ERR;
      break;
    case ARDUINO_EVENT_WPS_ER_PIN:
      Serial.println("WPS_PIN = " + Utils::wpspin2string(info.wps_er_pin.pin_code));
      break;
    default:
      break;
  }
}

WiFiManager::WiFiManager(){}

void WiFiManager::setCredentials(const char* SSID, const char* password){
  this->SSID = SSID;
  this->password = password;
}
void WiFiManager::setCredentials(WiFiCredentials credentials){
  this->SSID = credentials.ssid;
  this->password = credentials.password;
}
//Function that saves WiFi credentials into EEPROM flash memory to use after WPS setup.
bool WiFiManager::saveWiFiCredentials() {

  Serial.println("Saving WiFi Credentials to EEPROM memory...");

  if (WiFi.status()!=WL_CONNECTED){
      Serial.println("Not connected. Unable to save credentials.");
      return false;
  }
  eepromcredentials_t credentials;
  
  memset(&credentials.ssid, '\0', sizeof(credentials.ssid));
  memset(&credentials.password, '\0', sizeof(credentials.password));

  strncpy(credentials.ssid, WiFi.SSID().c_str(), strlen(WiFi.SSID().c_str()));
  strncpy(credentials.password, WiFi.psk().c_str(), strlen(WiFi.psk().c_str()));

  credentials.checksum = \
    Utils::calculateChecksum((uint8_t*)credentials.ssid, sizeof(credentials.ssid)) ^ 
    Utils::calculateChecksum((uint8_t*)credentials.password, sizeof(credentials.password));
  
  EEPROM.begin(sizeof(eepromcredentials_t));
  EEPROM.put(0, credentials);

  bool success = EEPROM.commit();
  if (!success) {
    Serial.println("Failed to save credentials to EEPROM.");
  }
  return success;
}

//Function that loads WiFi credentials from EEPROM flash memory and checks if they're valid.
bool WiFiManager::loadWiFiCredentials() {

  Serial.println("Loading credentials from EEPROM memory..");

  try{
    EEPROM.begin(sizeof(eepromcredentials_t));
    EEPROM.get(0, eeprom_credentials);

    uint8_t checksum = \
      Utils::calculateChecksum((uint8_t*)eeprom_credentials.ssid, sizeof(eeprom_credentials.ssid)) ^ 
      Utils::calculateChecksum((uint8_t*)eeprom_credentials.password, sizeof(eeprom_credentials.password));
    
    if (checksum == eeprom_credentials.checksum){
      this->SSID = eeprom_credentials.ssid;
      this->password = eeprom_credentials.password;

      return true;
    }
  } catch(...) {

    Serial.println("Failed to retrieve data from EEPROM memory.");

  }
  return false;
}

void WiFiManager::connectWPS() {

  WiFiEventFuncCb cbEvent = [this](WiFiEvent_t event, arduino_event_info_t info){
    WiFiEvent(event, info);
  };

  WiFi.onEvent(cbEvent);

  WiFi.mode(WIFI_MODE_STA);

  state = WPS_CONNECTION;
  Serial.println("Starting WPS Connection.");

  config.wps_type = ESP_WPS_MODE;
  strcpy(config.factory_info.manufacturer, ESP_MANUFACTURER);
  strcpy(config.factory_info.model_number, ESP_MODEL_NUMBER);
  strcpy(config.factory_info.model_name, ESP_MODEL_NAME);
  strcpy(config.factory_info.device_name, ESP_DEVICE_NAME);

  if(esp_wifi_wps_enable(&config)){
    Serial.println("WPS Enable Failed");
  } else if(esp_wifi_wps_start(120000)){
    Serial.println("WPS Start Failed");
  }   
}

void WiFiManager::connect(uint16_t timeout_s) {
    uint32_t attempts = 0;
    const uint16_t time_delay = 300;
    WiFiMulti multi;
    multi.addAP(this->SSID, this->password);

    while (multi.run() != WL_CONNECTED && attempts*time_delay<timeout_s*1000){
        attempts++;
        Serial.println("Attempting to connect to WiFi.");
        delay(time_delay);
    }

    if (attempts*time_delay > timeout_s){
      Serial.println("Failed to connect to the WiFi.");
      state = DISCONNECTED;
    }else {
      Serial.print("Successfully connected to WiFi: ");
      state = CONNECTED;
      Serial.println(WiFi.SSID());
    }
}

String WiFiManager::macAddress(){
  return WiFi.macAddress();
}

wl_status_t WiFiManager::status(){
  return WiFi.status();
}