#include <Arduino.h>
#include <ArduinoJson.h>
#include "wifi_handler/wifi_handler.h"
#include "ir_remote/ir_remote.h"
#include "websocket_handler/websocket_handler.h"
#include "async_led/async_led.h"
#include "utils/utils.h"

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

LED* remoteLED = nullptr;
LED* wifiLED = nullptr;
IRremote* irRemote = nullptr;

WebSocketAWS webSocket(&irRemote);
WiFiHandler WiFiHandler;

String query_parameters = WS_URL;
uint64_t startTime;

bool flag = true;
void setup() {
  Serial.begin(921600);
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);

  remoteLED = new LED(LED_YELLOW_PIN);
  wifiLED = new LED(LED_BLUE_PIN);

  irRemote = new IRremote(IR_INPUT, IR_OUTPUT, &remoteLED);

  WiFiCredentials credentials;
  wifiLED->blink(3, 5);

  if (WiFiHandler.loadWiFiCredentials()) WiFiHandler.connect(10);
  

  if (WiFiHandler.status() != WL_CONNECTED){

    while(digitalRead(BUTTON_RIGHT_PIN)==HIGH) remoteLED->blink(1,1);

    WiFiHandler.connectWPS();

    while (WiFi.status() != WL_CONNECTED);
  }
  
  wifiLED->blink(8,5);
  
  query_parameters += "?deviceType=iot&macAddress=";
  query_parameters += WiFiHandler.macAddress();
  webSocket.startConnection(WS_HOST, WS_PORT, query_parameters.c_str(), "", "wss");
  webSocket.enableHeartbeat(3*1000, 5*1000,1);
  wifiLED->setState(HIGH);

  startTime = millis();
}


void loop() {
  webSocket.loop();
  // Let 10 seconds pass for the webSocket to connect to not flood connection requests
  if((millis() - startTime >= 10000) && !webSocket.isConnected()){
    webSocket.startConnection(WS_HOST, WS_PORT, query_parameters.c_str(), "", "wss");
    startTime = millis();
  }
}