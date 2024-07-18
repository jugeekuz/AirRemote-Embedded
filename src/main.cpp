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
#include <esp_task_wdt.h>

// #define BUTTON_RIGHT_PIN 21
// #define BUTTON_LEFT_PIN 35
// #define LED_YELLOW_PIN 16
// #define LED_BLUE_PIN 18

// #define IR_INPUT 15
// #define IR_OUTPUT 4
#define RESET_BUTTON 9
#define IR_INPUT 2
#define IR_OUTPUT 13
#define LED_YELLOW_PIN 25
#define LED_BLUE_PIN 26


#define WS_HOST "fofo64r8il.execute-api.eu-central-1.amazonaws.com"
#define WS_PORT 443
#define WS_URL "/dev"
#define API_GATEWAY_TIMEOUT_MINUTES 10

#define WIFI_TIMEOUT 10
#define WIFI_BLINK_FREQUENCY 5
#define WEBSOCKET_BLINK_FREQUENCY 10
#define WIFI_NOT_CONNECTED_FREQUENCY 2
#define WPS_CONNECTING_FREQUENCY 3

SET_LOOP_TASK_STACK_SIZE( 16*1024 );

AsyncLED *remoteLED = nullptr;
AsyncLED *connectionLED = nullptr;
AsyncButton *resetButton = nullptr;

IRremote *irRemote = nullptr;


WiFiHandler WiFiHandler;
WebServer *webServer;
WebSocketHandler *webSocket;
String query_parameters = "";
uint64_t startTime;

bool flag = true;

// Function that starts the WebServer to set the configuration parameters
void runWebServer(){  
  try{
    webServer = new WebServer();
    webServer->setup();
    while(!webserverSubmitted) webServer->loop();
    
  } catch (const std::bad_alloc& e) {

    Serial.printf("[WebServer][ERROR] Memory allocation failed: %s\n",e.what());
    return;

  }catch (const std::exception& e){
    Serial.println("[SETUP][ERROR] Unexpected error occured in WebServerPortal setup: " + String(e.what()));
    delete webServer;
    return;
  }
  delete webServer;
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
  try{
    if (webSocket == NULL) webSocket = new WebSocketHandler(&irRemote);

  } catch (const std::bad_alloc& e) {
    Serial.printf("[WSc][ERROR] Memory allocation failed: %s\n",e.what());
    return;
  }

  if (connectionLED != NULL) connectionLED->blink(WEBSOCKET_BLINK_FREQUENCY);
  EEPROMUtils::loadWebSocketConfig();
  query_parameters = websocket_credentials.ws_url;
  query_parameters += "?deviceType=iot&macAddress=";
  query_parameters += WiFiHandler.macAddress();

  webSocket->startConnection(websocket_credentials.ws_host, websocket_credentials.ws_port, query_parameters.c_str(), "", "wss");
  webSocket->enableHeartbeat(3*1000, 5*1000,1);
}


// Function that resets the connection to re-initialize the configuration parameters and re-connect.
void resetConnection(){
  Serial.println("[RESET][INFO] Resetting connection...");
  runWebServer();
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

  resetButton = new AsyncButton(RESET_BUTTON);
  resetButton->setButtonListener(&resetConnection);


  remoteLED = new AsyncLED(LED_YELLOW_PIN);
  connectionLED = new AsyncLED(LED_BLUE_PIN);

  irRemote = new IRremote(IR_INPUT, IR_OUTPUT, &remoteLED);

  if(!WiFiHandler.loadWiFiCredentials()) runWebServer();

  connectWiFi();
  connectWebSocket();

  startTime = millis();
}


void loop() {
  if (webSocket != NULL) {
    webSocket->loop();
    if(flag && webSocket->isConnected()){
      flag = false;
      if (connectionLED != NULL) connectionLED->setState(HIGH);
    } 

    // Let 10 seconds pass for the webSocket to connect to not flood connection requests
    if((millis() - startTime >= 10000) && !webSocket->isConnected()){
      webSocket->startConnection(websocket_credentials.ws_host, websocket_credentials.ws_port, query_parameters.c_str(), "", "wss");
      startTime = millis();
    }
  }
  
}