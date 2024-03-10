#include <Arduino.h>

#include "WiFiManager.h"
#include "ActionHandler.h"
#include "WebsocketHandler.h"
#include "IRremoteHandler.h"

#define WIFI_SSID "COSMOTE-665216"
#define WIFI_PASSWORD "tasos2021"

#define BUTTON_RIGHT_PIN 21
#define BUTTON_LEFT_PIN 35

#define IR_INPUT 15
#define IR_OUTPUT 4

#define LED_YELLOW_PIN 16
#define LED_BLUE_PIN 18


IRremote irRemote(IR_INPUT, IR_OUTPUT);
WiFiManager wifiManager;

void setup() {
  Serial.begin(921600);

  pinMode(LED_BLUE_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);

  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  
  WiFiCredentials credentials;
  WiFiManager::loadWiFiCredentials(credentials);
  
  if(true){
   if (credentials.flag==EEPROM_VALIDITY_FLAG){
    // TODO: Handle error scenarios + load from Flash
    Serial.println("Found credentials");
    wifiManager.setCredentials(WIFI_SSID, WIFI_PASSWORD);
    wifiManager.connect(); 
    Serial.println("Connected to WiFi Succesfully.");
    }
  }
}

void loop() {
  digitalWrite(LED_BLUE_PIN, WiFi.status() == WL_CONNECTED);
  if(digitalRead(BUTTON_RIGHT_PIN)==LOW) {
    Serial.println("Attempting WPS Connection.");
    delay(1000);
    wifiManager.connectWPS();
  }
}