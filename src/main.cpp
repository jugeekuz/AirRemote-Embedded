#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifi_handler/wifi_handler.h"
#include "webserver/webserver.h"
#include "ir_remote/ir_remote.h"
#include "websocket_handler/websocket_handler.h"
#include "async_led/async_led.h"
#include "async_button/async_button.h"
#include "utils/utils.h"
#include "utils/eeprom_utils.h"
#include <esp_task_wdt.h>

#define LED_YELLOW_PIN 25
#define LED_BLUE_PIN 26

#define IR_INPUT 2
#define IR_OUTPUT 13
#define RESET_BUTTON 9

WebServer *webServer = nullptr;
WiFiHandler *wifiHandler = nullptr;
WebSocketHandler *webSocketHandler = nullptr;
IRremote *irRemote = nullptr;

AsyncLED *remoteLED = nullptr;
AsyncLED *setupLED = nullptr;

AsyncButton *resetButton = nullptr;
uint64_t startTime;
String query_parameters;
void resetConfig() {
  if(EEPROMUtils::resetConfig()) esp_restart();
}
bool bootWebServer = false;

void setup() {
  Serial.setTxBufferSize(1024);
  Serial.begin(115200);

  while (!Serial)
    ;;

  remoteLED = new AsyncLED(LED_YELLOW_PIN);
  setupLED = new AsyncLED(LED_BLUE_PIN);
  remoteLED->setState(LOW);
  setupLED->setState(LOW);
  
  wifiHandler = new WiFiHandler(setupLED);
  irRemote = new IRremote(IR_INPUT, IR_OUTPUT, &remoteLED);
  webSocketHandler = new WebSocketHandler(&irRemote, setupLED);
  
  bootWebServer = EEPROMUtils::bootWebserver();
  if (bootWebServer) {
    webServer = new WebServer(wifiHandler, webSocketHandler, setupLED);
    webServer->setup();
  } else {
    resetButton = new AsyncButton(RESET_BUTTON);
    resetButton->setButtonListener(&resetConfig, 1);

    if(!EEPROMUtils::loadWiFiCredentials()) resetConfig();

    wifiHandler->setCredentials();
    wifiHandler->connect(60);

    while(WiFi.status() != WL_CONNECTED)
      ;;

    if(!EEPROMUtils::loadWebSocketConfig()) resetConfig();

    query_parameters = websocket_credentials.ws_url;
    query_parameters += "?deviceType=iot&macAddress=";
    query_parameters += wifiHandler->macAddress();

    webSocketHandler->startConnection(websocket_credentials.ws_host, websocket_credentials.ws_port, query_parameters.c_str(), "", "wss");
    webSocketHandler->enableHeartbeat(3*1000, 5*1000,1);

    startTime = millis();
  }
}

void loop() {
  if (bootWebServer) {
    webServer->loop();
  } else {
    webSocketHandler->loop();

    // Let 10 seconds pass for the webSocket to connect to not flood connection requests in case webSocket disconnects
    if((millis() - startTime >= 10000) && !webSocketHandler->isConnected()){
      webSocketHandler->startConnection(websocket_credentials.ws_host, websocket_credentials.ws_port, query_parameters.c_str(), "", "wss");
      startTime = millis();
    }
  }
}