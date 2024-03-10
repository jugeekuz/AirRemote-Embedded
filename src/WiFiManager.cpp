/* 
Class used to connect to WiFi.
Can connect to WiFi through SSID and password.
Can also connect through WPS and the credentials are saved to Flash.
*/

#include "WiFiManager.h"

State state = DISCONNECTED;

void wpsInitConfig(){
  config.wps_type = ESP_WPS_MODE;
  strcpy(config.factory_info.manufacturer, ESP_MANUFACTURER);
  strcpy(config.factory_info.model_number, ESP_MODEL_NUMBER);
  strcpy(config.factory_info.model_name, ESP_MODEL_NAME);
  strcpy(config.factory_info.device_name, ESP_DEVICE_NAME);
}

void wpsStart(){
    esp_err_t err;
    err = esp_wifi_wps_enable(&config); 
    if(err){
    	Serial.println("WPS Enable Failed");
    } else if(esp_wifi_wps_start(120000)){
    	Serial.println("WPS Start Failed");
    }
    Serial.println(err);
    
}

void wpsStop(){
    if(esp_wifi_wps_disable()){
    	Serial.println("WPS Disable Failed");
    }
}

String wpspin2string(uint8_t a[]){
  char wps_pin[9];
  for(int i=0;i<8;i++){
    wps_pin[i] = a[i];
  }
  wps_pin[8] = '\0';
  return (String)wps_pin;
}

void WiFiEvent(WiFiEvent_t event, arduino_event_info_t info){
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

WiFiManager::WiFiManager(): WiFiMulti(), 
                            WPS(true){}

void WiFiManager::setCredentials(const char* WiFiSSID, const char* WiFiPass){
    this->SSID = WiFiSSID;
    this->passphrase= WiFiPass;
    this->WPS = false;
}

//Function that saves WiFi credentials into EEPROM flash memory to use after WPS setup.
void WiFiManager::saveWiFiCredentials(const char *ssid,const char *password) {
    WiFiCredentials credentials;

    //Initialize credentials with '+' so that we know when credentials end.
    memset(credentials.ssid, '+', sizeof(credentials.ssid)-1);
    memset(credentials.password, '+', sizeof(credentials.password)-1);

    //Put a flag to know data being read is valid.
    credentials.flag = EEPROM_VALIDITY_FLAG;
    
    //Null terminate
    credentials.ssid[sizeof(credentials.ssid)-1] = '\0';
    credentials.password[sizeof(credentials.password)-1] = '\0';

    //Copy the remaining ssid / password given into the buffered string.
    strncpy(credentials.ssid, ssid, strlen(ssid));
    strncpy(credentials.password, password, strlen(password));

    EEPROM.begin(sizeof(WiFiCredentials));
    EEPROM.put(0, credentials);
    if (EEPROM.commit()){
        Serial.println("Successfully saved credentials into EEPROM flash memory.");
    }else{
        Serial.println("Couldn't save credentials into EEPROM flash memory.");
    }
}

//Function that loads WiFi credentials from EEPROM flash memory and checks if they're valid.
void WiFiManager::loadWiFiCredentials(WiFiCredentials& credentials) {
    EEPROM.begin(sizeof(WiFiCredentials));
    EEPROM.get(0, credentials);
}

//Connect via WPS to be implemented by the press of a button
void WiFiManager::connectWPS() {
    WiFi.onEvent(WiFiEvent);
    WiFi.mode(WIFI_MODE_STA);
    wpsStart();
    if (WiFi.status()==WL_CONNECTED) saveWiFiCredentials(WiFi.SSID().c_str(), WiFi.psk().c_str());
}

//Function that recurrently calls attemptConnect() until it works
void WiFiManager::connect() {
    addAP(SSID, passphrase);
    run(0);
    uint32_t attempts = 0;

    while (WiFi.status() != WL_CONNECTED && attempts < 1000){
        attempts++;
        Serial.println("Attempting to connect to WiFi.");
        run(0);
        delay(300);
    }
}
