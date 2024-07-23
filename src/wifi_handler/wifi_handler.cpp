/* 
Class used to connect to WiFi.
Can connect to WiFi through SSID and password.
Can also connect through WPS and the credentials are saved to Flash.
*/
#include "wifi_handler/wifi_handler.h"

WiFiHandler::WiFiHandler() : WiFiClass(){}
WiFiHandler::~WiFiHandler(){}

void WiFiHandler::setCredentials(const char* ssid, const char* pass){
  memset(&this->SSID, '\0', sizeof(this->SSID));
  memset(&this->password, '\0', sizeof(this->password));
  
  strncpy(this->SSID, ssid, strlen(ssid));
  strncpy(this->password, pass, strlen(pass));
}

void WiFiHandler::setCredentials(){
  memset(&this->SSID, '\0', sizeof(this->SSID));
  memset(&this->password, '\0', sizeof(this->password));
  
  strncpy(this->SSID, wifi_credentials.ssid, strlen(wifi_credentials.ssid));
  strncpy(this->password, wifi_credentials.password, strlen(wifi_credentials.password));
}

//Function that saves WiFi credentials to EEPROM flash memory.
bool WiFiHandler::saveWiFiCredentials() {
  memset(&wifi_credentials.ssid, '\0', sizeof(wifi_credentials.ssid));
  memset(&wifi_credentials.password, '\0', sizeof(wifi_credentials.password));
  
  strncpy(wifi_credentials.ssid, WiFi.SSID().c_str(), strlen(WiFi.SSID().c_str()));
  strncpy(wifi_credentials.password, WiFi.psk().c_str(), strlen(WiFi.psk().c_str()));
  
  Serial.printf("Breakpoint2: %s\n", wifi_credentials.ssid);
  return (EEPROMUtils::updateWiFiCredentials());
}

//Function that loads WiFi credentials from EEPROM flash memory and checks if they're valid.
bool WiFiHandler::loadWiFiCredentials() {
  if (EEPROMUtils::loadWiFiCredentials()){
    setCredentials();
    return true;
  }
  return false;
}

//Callback function for WPS related events.
void WiFiHandler::WPSCallback(WiFiEvent_t event, arduino_event_info_t info){
  switch(event){
    case ARDUINO_EVENT_WIFI_STA_START:
      Serial.println("[WiFi][INFO] Station Mode Started");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("[WiFi][INFO] Connected to WiFi: " + String(WiFi.SSID()));
      Serial.print("[WiFi][INFO] Got IP: ");
      Serial.println(WiFi.localIP());
      saveWiFiCredentials();
      state = CONNECTED;
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("[WiFi][INFO] Disconnected from station, attempting reconnection");
      WiFi.reconnect();
      break;
    case ARDUINO_EVENT_WPS_ER_SUCCESS:
      Serial.println("[WiFi][INFO] WPS Successfull, stopping WPS and connecting to: " + String(WiFi.SSID()));
      wpsStop();
      delay(10);
      WiFi.begin();
      break;
    case ARDUINO_EVENT_WPS_ER_FAILED:
      Serial.println("[WiFi][INFO] WPS Failed, retrying");
      wpsStop();
      state = WPS_ERR;
      break;
    case ARDUINO_EVENT_WPS_ER_TIMEOUT:
      Serial.println("[WiFi][INFO] WPS Timed out, retrying");
      wpsStop();
      state = WPS_ERR;
      break;
    case ARDUINO_EVENT_WPS_ER_PIN:
      Serial.println("[WiFi][INFO] WPS_PIN = " + Utils::wpspin2string(info.wps_er_pin.pin_code));
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
  onEvent(cbEvent);
  mode(WIFI_MODE_APSTA);

  state = WPS_CONNECTION;
  Serial.println("[WiFi][INFO] Starting WPS Connection.");

  wps_config.wps_type = ESP_WPS_MODE;
  strcpy(wps_config.factory_info.manufacturer, ESP_MANUFACTURER);
  strcpy(wps_config.factory_info.model_number, ESP_MODEL_NUMBER);
  strcpy(wps_config.factory_info.model_name, ESP_MODEL_NAME);
  strcpy(wps_config.factory_info.device_name, ESP_DEVICE_NAME);

  if(esp_wifi_wps_enable(&wps_config)){
    Serial.println("[WiFi][ERROR] WPS Enable Failed");
  } else if(esp_wifi_wps_start(120000)){
    Serial.println("[WiFi][ERROR] WPS Start Failed");
  }   
}

//Function that attempts to connect to WiFi for until a time out has occured after `timeout_s` seconds.
void WiFiHandler::connect(uint16_t timeout_s) {
    uint32_t attempts = 0;
    const uint16_t time_delay = 300;
    
    WiFi.begin(this->SSID, this->password);

    while (WiFi.status() != WL_CONNECTED && attempts*time_delay < timeout_s*1000){
        attempts++;
        Serial.println("[WiFi][INFO] Attempting to connect to WiFi...");
        delay(time_delay);
    }

    if (attempts*time_delay > timeout_s*1000){
      Serial.println("[WiFi][ERROR] Failed to connect to the WiFi.");
      state = DISCONNECTED;
    }else {
      Serial.print("[WiFi][INFO] Connected to WiFi: ");
      state = CONNECTED;
      Serial.println(WiFi.SSID());
    }
}

void WiFiHandler::wpsStop(){
    if(esp_wifi_wps_disable()){
    	Serial.println("[WiFi][ERROR] WPS Disable Failed");
    }
}