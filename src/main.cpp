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

#define WIFI_TIMEOUT 10
#define WIFI_BLINK_FREQUENCY 5
#define WEBSOCKET_BLINK_FREQUENCY 10
#define WIFI_NOT_CONNECTED_FREQUENCY 2
#define WPS_CONNECTING_FREQUENCY 3

AsyncLED* remoteLED = nullptr;
AsyncLED* wifiLED = nullptr;
IRremote* irRemote = nullptr;

WebSocketHandler webSocket(&irRemote);

WiFiHandler WiFiHandler;

String query_parameters = WS_URL;
uint64_t startTime;

bool flag = true;

void setup() {
  Serial.begin(921600);
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);

  remoteLED = new AsyncLED(LED_YELLOW_PIN);
  wifiLED = new AsyncLED(LED_BLUE_PIN);

  irRemote = new IRremote(IR_INPUT, IR_OUTPUT, &remoteLED);

  wifiLED->blink(WIFI_BLINK_FREQUENCY);

  //If valid credentials are save to EEPROM, connect to WiFi with those credentials
  if (WiFiHandler.loadWiFiCredentials()) WiFiHandler.connect(WIFI_TIMEOUT);


  if (WiFiHandler.status() != WL_CONNECTED){

    wifiLED->blink(WIFI_NOT_CONNECTED_FREQUENCY);

    //If no valid credentials are saved, wait for button press to connect to WiFi using WPS.
    while(digitalRead(BUTTON_RIGHT_PIN)==HIGH);

    wifiLED->blink(WPS_CONNECTING_FREQUENCY);

    WiFiHandler.connectWPS();
    while (WiFi.status() != WL_CONNECTED);
  }
  
  wifiLED->blink(WEBSOCKET_BLINK_FREQUENCY);
  
  query_parameters += "?deviceType=iot&macAddress=";
  query_parameters += WiFiHandler.macAddress();
  webSocket.startConnection(WS_HOST, WS_PORT, query_parameters.c_str(), "", "wss");
  webSocket.enableHeartbeat(3*1000, 5*1000,1);

  startTime = millis();
}


void loop() {
  webSocket.loop();
  if(flag && webSocket.isConnected()){
    flag = false;
    wifiLED->setState(HIGH);
  }
  // Let 10 seconds pass for the webSocket to connect to not flood connection requests
  if((millis() - startTime >= 10000) && !webSocket.isConnected()){
    webSocket.startConnection(WS_HOST, WS_PORT, query_parameters.c_str(), "", "wss");
    startTime = millis();
  }
}