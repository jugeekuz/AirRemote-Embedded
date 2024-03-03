#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

#include "WiFiManager.h"

#define WIFI_SSID "COSMOTE-665216"
#define WIFI_PASSWORD "tasos2021"

WiFiManager wifiManager(WIFI_SSID,WIFI_PASSWORD);

void setup() {
  Serial.begin(921600);
  pinMode(LED_BUILTIN, OUTPUT);
  wifiManager.connect(); 
  Serial.println("Connected");
}

void loop() {
  digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED);
}