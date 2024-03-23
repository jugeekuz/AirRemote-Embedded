#include <Arduino.h>
// #include <WebSocketsClient.h>
#include "WiFiManager.h"
#include <ArduinoJson.h>
#include "ActionHandler.h"
#include "WebsocketHandler.h"

#define WIFI_SSID "COSMOTE-665216"
#define WIFI_PASSWORD "tasos2021"

#define BUTTON_RIGHT_PIN 21
#define BUTTON_LEFT_PIN 35

#define LED_YELLOW_PIN 16
#define LED_BLUE_PIN 18

#define WS_HOST "fofo64r8il.execute-api.eu-central-1.amazonaws.com"
#define WS_PORT 443
#define WS_URL "/dev"

// IRremote irRemote(IR_INPUT, IR_OUTPUT);
WiFiManager wifiManager;
WebSocketAWS webSocket;

bool flag = true;
bool wps_flag = false;
bool ws_flag = true;
void setup() {
  Serial.begin(921600);

  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);

  WiFiCredentials credentials;
  WiFiManager::loadWiFiCredentials(credentials);
  
  if (credentials.flag==EEPROM_VALIDITY_FLAG){
    // TODO: Handle error scenarios 
    Serial.println("Found valid credentials in EEPROM flash memory.");
    wifiManager.setCredentials(credentials.ssid, credentials.password);
    wifiManager.connect(20);
    delay(1000);
  }

  if (WiFi.status() != WL_CONNECTED){
    Serial.println("WiFi Connection Failed. Waiting for WPS Button.");
    //Blocking code
    while (digitalRead(BUTTON_RIGHT_PIN)!=LOW);
    wifiManager.connectWPS();
    //Blocking code
    while (WiFi.status() != WL_CONNECTED);
    wifiManager.saveWiFiCredentials();
  }
  digitalWrite(LED_BLUE_PIN, HIGH);

  webSocket.startConnection(WS_HOST, WS_PORT, WS_URL, "", "wss");
}
void loop() {
  if(webSocket.isConnected()) digitalWrite(LED_YELLOW_PIN, HIGH);
  webSocket.loop();
}