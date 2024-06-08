/* 
Class used to connect to WiFi.
Can connect to WiFi through SSID and password.
Can also connect through WPS and the credentials are saved to Flash.
*/
#include "wifi_handler/wifi_handler.h"

WiFiHandler::WiFiHandler(){
  try{
    WiFiCredentials *wifi_credentials = new WiFiCredentials;
    wifi_credentials->ssid = new char[33];
    wifi_credentials->password = new char[64];
    this->wifi_credentials = wifi_credentials;
  }catch(const std::bad_alloc& e){
    Serial.println("[WiFi][ERROR] Failed to allocate memory for WiFi credentials.");
  }
}
WiFiHandler::~WiFiHandler(){
  try{
    delete[] this->wifi_credentials->ssid;
    delete[] this->wifi_credentials->password;
    delete[] this->wifi_credentials;
  }catch(...){
    Serial.println("[WiFi][ERROR] Failed to deallocate memory for WiFi credentials.");
  }
}

void WiFiHandler::setCredentials(const char* SSID, const char* password){
  this->SSID = SSID;
  this->password = password;
}

void WiFiHandler::setCredentials(WiFiCredentials credentials){
  this->SSID = credentials.ssid;
  this->password = credentials.password;
}

//Function that saves WiFi credentials to EEPROM flash memory.
bool WiFiHandler::saveWiFiCredentials() {
  WiFiCredentials credentials;
  strncpy(credentials.ssid, WiFi.SSID().c_str(), strlen(WiFi.SSID().c_str()));
  strncpy(credentials.password, WiFi.psk().c_str(), strlen(WiFi.psk().c_str()));
  return (WiFiUtils::saveEEPROMCredentials(credentials));
}

//Function that loads WiFi credentials from EEPROM flash memory and checks if they're valid.
bool WiFiHandler::loadWiFiCredentials() {
  if (WiFiUtils::loadEEPROMCredentials(this->wifi_credentials)){
    setCredentials(*wifi_credentials);
    return true;
  }
  return false;
}

//Callback function for WPS related events.
void WiFiHandler::WPSCallback(WiFiEvent_t event, arduino_event_info_t info){
  switch(event){
    case ARDUINO_EVENT_WIFI_STA_START:
      Serial.println("[WiFi] Station Mode Started");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("[WiFi] Connected to WiFi: " + String(WiFi.SSID()));
      Serial.print("[WiFi] Got IP: ");
      Serial.println(WiFi.localIP());
      saveWiFiCredentials();
      state = CONNECTED;
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("[WiFi] Disconnected from station, attempting reconnection");
      WiFi.reconnect();
      break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
      Serial.println("[WiFi] WPS Successfull, stopping WPS and connecting to: " + String(WiFi.SSID()));
      wpsStop();
      delay(10);
      WiFi.begin();
      break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
      Serial.println("[WiFi] WPS Failed, retrying");
      wpsStop();
      state = WPS_ERR;
      break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
      Serial.println("[WiFi] WPS Timed out, retrying");
      wpsStop();
      state = WPS_ERR;
      break;
    case ARDUINO_EVENT_WPS_ER_PIN:
      Serial.println("[WiFi] WPS_PIN = " + Utils::wpspin2string(info.wps_er_pin.pin_code));
      break;
    default:
      break;
  }
}

//Function that connects to WiFi using WPS.
void WiFiHandler::connectWPS() {
  WiFiEventFuncCb cbEvent = [this](WiFiEvent_t event, arduino_event_info_t info){
    WPSCallback(event, info);
  };
  WiFi.onEvent(cbEvent);
  WiFi.mode(WIFI_MODE_STA);

  state = WPS_CONNECTION;
  Serial.println("[WiFi] Starting WPS Connection.");

  config.wps_type = ESP_WPS_MODE;
  strcpy(config.factory_info.manufacturer, ESP_MANUFACTURER);
  strcpy(config.factory_info.model_number, ESP_MODEL_NUMBER);
  strcpy(config.factory_info.model_name, ESP_MODEL_NAME);
  strcpy(config.factory_info.device_name, ESP_DEVICE_NAME);

  if(esp_wifi_wps_enable(&config)){
    Serial.println("[WiFi][ERROR] WPS Enable Failed");
  } else if(esp_wifi_wps_start(120000)){
    Serial.println("[WiFi][ERROR] WPS Start Failed");
  }   
}

//Function that attempts to connect to WiFi for until a time out has occured after `timeout_s` seconds.
void WiFiHandler::connect(uint16_t timeout_s) {
    uint32_t attempts = 0;
    const uint16_t time_delay = 300;
    WiFiMulti multi;
    multi.addAP(this->SSID, this->password);

    while (multi.run() != WL_CONNECTED && attempts*time_delay<timeout_s*1000){
        attempts++;
        Serial.println("[WiFi] Attempting to connect to WiFi.");
        delay(time_delay);
    }

    if (attempts*time_delay > timeout_s){
      Serial.println("[WiFi][ERROR] Failed to connect to the WiFi.");
      state = DISCONNECTED;
    }else {
      Serial.print("[WiFi] Connected to WiFi: ");
      state = CONNECTED;
      Serial.println(WiFi.SSID());
    }
}

String WiFiHandler::macAddress(){
  return WiFi.macAddress();
}

wl_status_t WiFiHandler::status(){
  return WiFi.status();
}

void WiFiHandler::wpsStop(){
    if(esp_wifi_wps_disable()){
    	Serial.println("[WiFi][ERROR] WPS Disable Failed");
    }
}