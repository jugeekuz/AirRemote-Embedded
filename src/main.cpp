#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifi_handler/wifi_handler.h"
#include "webserver/webserver_portal.h"
#include "ir_remote/ir_remote.h"
#include "websocket_handler/websocket_handler.h"
#include "async_led/async_led.h"
#include "async_button/async_button.h"
#include "utils/utils.h"
#include "utils/eeprom_utils.h"

#define BUTTON_RIGHT_PIN 21
#define BUTTON_LEFT_PIN 35

#define LED_YELLOW_PIN 16
#define LED_BLUE_PIN 18

#define IR_INPUT 15
#define IR_OUTPUT 4

#define WS_HOST "fofo64r8il.execute-api.eu-central-1.amazonaws.com"
#define WS_PORT 443
#define WS_URL "/dev"
#define API_GATEWAY_TIMEOUT_MINUTES 10

#define WIFI_TIMEOUT 10
#define WIFI_BLINK_FREQUENCY 5
#define WEBSOCKET_BLINK_FREQUENCY 10
#define WIFI_NOT_CONNECTED_FREQUENCY 2
#define WPS_CONNECTING_FREQUENCY 3

AsyncLED *remoteLED = nullptr;
AsyncLED *connectionLED = nullptr;
AsyncButton *resetButton = nullptr;
// AsyncButton *WPSButton = nullptr;
IRremote *irRemote = nullptr;
WebServer *webServer = nullptr;

WiFiHandler WiFiHandler;
WebSocketHandler webSocket(&irRemote);

String query_parameters = "";
uint64_t startTime;

bool flag = true;

// Function that starts the WebServer to set the configuration parameters
void startWebServer(){
  Serial.print("Free heap before webserver task: ");
  Serial.println(ESP.getFreeHeap());
  webServer = new WebServer;
  
  try{
    webServer->setup();
    Serial.print("Free heap after webserver task: ");
    Serial.println(ESP.getFreeHeap());
    while(!webserverSubmitted) webServer->loop();

  } catch (const std::exception& e){
    Serial.println("[SETUP][ERROR] Unexpected error occured in WebServerPortal setup: " + String(e.what()));
  }
  delete webServer;
  Serial.print("Free heap after delete webserver task: ");
  Serial.println(ESP.getFreeHeap());
  WiFiHandler.loadWiFiCredentials();
}

// Function that starts the WPS connection
void startWPS(){
  if (connectionLED != NULL) connectionLED->blink(WPS_CONNECTING_FREQUENCY);
  WiFiHandler.connectWPS();
  while (WiFi.status() != WL_CONNECTED);
}

// Function that connects to the WiFi network
void connectWiFi(){
  WiFiHandler.connect(WIFI_TIMEOUT);
  if (connectionLED != NULL) connectionLED->blink(WIFI_BLINK_FREQUENCY);
  while (WiFi.status() != WL_CONNECTED);
  if (connectionLED != NULL) connectionLED->setState(HIGH);
}

// Function that connects to the WebSocket server
void connectWebSocket(){
  if (connectionLED != NULL) connectionLED->blink(WEBSOCKET_BLINK_FREQUENCY);
  EEPROMUtils::loadWebSocketConfig();
  query_parameters = websocket_credentials.ws_url;
  query_parameters += "?deviceType=iot&macAddress=";
  query_parameters += WiFiHandler.macAddress();

  webSocket.startConnection(websocket_credentials.ws_host, websocket_credentials.ws_port, query_parameters.c_str(), "", "wss");
  webSocket.enableHeartbeat(3*1000, 5*1000,1);
}

// Function that resets the connection to re-initialize the configuration parameters and re-connect.
void resetConnection(){
  Serial.println("[RESET][INFO] Resetting connection...");
  startWebServer();
  connectWiFi();
  connectWebSocket();
}

void setup() {
  Serial.setTxBufferSize(1024);
  Serial.begin(115200);

  Serial.println();
  Serial.println("*****************************");
  Serial.println("**     AirRemote v1.0      **");
  Serial.println("**  Developed by Jugeekuz  **");
  Serial.println("*****************************");
  Serial.println();

  resetButton = new AsyncButton(BUTTON_RIGHT_PIN);
  resetButton->setButtonListener(&resetConnection);

  // WPSButton = new AsyncButton(BUTTON_LEFT_PIN);
  // WPSButton->setButtonListener(&startWPS);

  remoteLED = new AsyncLED(LED_YELLOW_PIN);
  connectionLED = new AsyncLED(LED_BLUE_PIN);

  irRemote = new IRremote(IR_INPUT, IR_OUTPUT, &remoteLED);

  if(!WiFiHandler.loadWiFiCredentials()) startWebServer();

  connectWiFi();
  connectWebSocket();

  startTime = millis();
  Serial.print("Free stack for loop task: ");
  Serial.println(uxTaskGetStackHighWaterMark(NULL));
}


void loop() {
  // webSocket.loop();
  if(flag && webSocket.isConnected()){
    flag = false;
    if (connectionLED != NULL) connectionLED->setState(HIGH);
  } 

  // Let 10 seconds pass for the webSocket to connect to not flood connection requests
  if((millis() - startTime >= 10000) && !webSocket.isConnected()){
    webSocket.startConnection(websocket_credentials.ws_host, websocket_credentials.ws_port, query_parameters.c_str(), "", "wss");
    startTime = millis();
  }
  
}