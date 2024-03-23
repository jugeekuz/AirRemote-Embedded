/* 
Class used to connect to WiFi.
Can connect to WiFi through SSID and password.
Can also connect through WPS and the credentials are saved to Flash.
*/

#include "WiFiManager.h"
State state = DISCONNECTED;

void WiFiManager::wpsInitConfig(){
  config.wps_type = ESP_WPS_MODE;
  strcpy(config.factory_info.manufacturer, ESP_MANUFACTURER);
  strcpy(config.factory_info.model_number, ESP_MODEL_NUMBER);
  strcpy(config.factory_info.model_name, ESP_MODEL_NAME);
  strcpy(config.factory_info.device_name, ESP_DEVICE_NAME);
}

void WiFiManager::wpsStart(){
    if(esp_wifi_wps_enable(&config)){
    	Serial.println("WPS Enable Failed");
    } else if(esp_wifi_wps_start(120000)){
    	Serial.println("WPS Start Failed");
    }    
}

void WiFiManager::wpsStop(){
    if(esp_wifi_wps_disable()){
    	Serial.println("WPS Disable Failed");
    }
}

String WiFiManager::wpspin2string(uint8_t a[]){
  char wps_pin[9];
  for(int i=0;i<8;i++){
    wps_pin[i] = a[i];
  }
  wps_pin[8] = '\0';
  return (String)wps_pin;
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
      Serial.println("WPS_PIN = " + wpspin2string(info.wps_er_pin.pin_code));
      break;
    default:
      break;
  }
}

WiFiManager::WiFiManager(){}

void WiFiManager::setCredentials(const char* WiFiSSID, const char* WiFiPass){
    this->SSID = WiFiSSID;
    this->passphrase= WiFiPass;
}
//Function that saves WiFi credentials into EEPROM flash memory to use after WPS setup.
void WiFiManager::saveWiFiCredentials() {
    if (WiFi.status()!=WL_CONNECTED){
        Serial.println("Not connected. Unable to save credentials.");
        return ;
    }
    EEPROMCredentials credentials;

    //Initialize credentials with EEPROM_BUFFER so that we know when credentials end.
    memset(credentials.ssid, EEPROM_BUFFER, sizeof(credentials.ssid)-1);
    memset(credentials.password, EEPROM_BUFFER, sizeof(credentials.password)-1);

    //Put a flag to know data being read is valid.
    credentials.flag = EEPROM_VALIDITY_FLAG;
    
    //Null terminate
    credentials.ssid[sizeof(credentials.ssid)-1] = '\0';
    credentials.password[sizeof(credentials.password)-1] = '\0';

    //Copy the remaining ssid / password given into the buffered string.
    strncpy(credentials.ssid, WiFi.SSID().c_str(), strlen(WiFi.SSID().c_str()));
    strncpy(credentials.password, WiFi.psk().c_str(), strlen(WiFi.psk().c_str()));

    EEPROM.begin(sizeof(EEPROMCredentials));
    EEPROM.put(0, credentials);
    if (EEPROM.commit()){
        Serial.println("Successfully saved credentials into EEPROM flash memory.");
    }else{
        Serial.println("Couldn't save credentials into EEPROM flash memory.");
    }
}
const char* WiFiManager::convertToString(const char input[]) {
    size_t length = strlen(input);
    size_t actual_length = 0;

    // Copy characters from input to result, excluding EEPROM_BUFFER
    uint8_t i=0;
    while (input[i] != EEPROM_BUFFER && i<(length-1)) {
        actual_length++;
        i++;
    }
    char* result = new char[actual_length + 1];
    i = 0;
    while (input[i] != EEPROM_BUFFER && i<(length-1)) {
        result[i] = input[i];
        i++;
    }
    // Add null terminator to the result
    result[i] = '\0';
    return result;
}

//Function that loads WiFi credentials from EEPROM flash memory and checks if they're valid.
void WiFiManager::loadWiFiCredentials(WiFiCredentials& credentials) {
    EEPROMCredentials eeprom_credentials;
    Serial.println("Loading credentials from EEPROM memory..");
    EEPROM.begin(sizeof(EEPROMCredentials));
    EEPROM.get(0, eeprom_credentials);

    credentials.flag = eeprom_credentials.flag;
    credentials.ssid = convertToString(eeprom_credentials.ssid);
    credentials.password = convertToString(eeprom_credentials.password);

}

//Connect via WPS to be implemented by the press of a button
void WiFiManager::connectWPS() {
    WiFi.onEvent(WiFiEvent);
    WiFi.mode(WIFI_MODE_STA);
    state = WPS_CONNECTION;
    Serial.println("Starting WPS Connection.");
    wpsInitConfig();
    wpsStart();
}

//Function that recurrently calls attemptConnect() until it works
void WiFiManager::connect(uint16_t timeout_s) {
    uint32_t attempts = 0;
    const uint16_t time_delay = 300;
    WiFiMulti multi;
    multi.addAP(SSID, passphrase);
    while (multi.run() != WL_CONNECTED && attempts*time_delay){
        attempts++;
        Serial.println("Attempting to connect to WiFi.");
        delay(time_delay);
    }
    Serial.print("Successfully connected to WiFi: ");
    Serial.println(WiFi.SSID());
}
