#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

#include "WiFiManager.h"
#include "IRremote.h"
#include "PeripheralHandler.h"

#define WIFI_SSID "COSMOTE-665216"
#define WIFI_PASSWORD "tasos2021"

#define BUTTON_RIGHT_PIN 21
#define BUTTON_LEFT_PIN 35

#define IR_INPUT 15
#define IR_OUTPUT 4

#define LED_YELLOW 16
#define LED_BLUE 18

WiFiManager wifiManager(WIFI_SSID,WIFI_PASSWORD);
IRremote irRemote(IR_INPUT, IR_OUTPUT);

void setup() {
  Serial.begin(921600);

  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);

  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);

  wifiManager.connect(); 
  Serial.println("Connected to WiFi Succesfully.");
}

void loop() {
  digitalWrite(LED_BLUE, WiFi.status() == WL_CONNECTED);
}