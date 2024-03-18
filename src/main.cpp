#include <Arduino.h>

#include "WiFiManager.h"
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


//deletefromhere
// #define MSG_SIZE 256

// WebSocketsClient wsClient;
// void sendErrorMessage(const char *error) {
//   char msg[MSG_SIZE];

//   sprintf(msg, "{\"action\":\"msg\",\"type\":\"error\",\"body\":\"%s\"}",
//           error);

//   wsClient.sendTXT(msg);
// }

// void sendOkMessage() {
//   wsClient.sendTXT("{\"action\":\"msg\",\"type\":\"status\",\"body\":\"ok\"}");
// }

// uint8_t toMode(const char *val) {
//   if (strcmp(val, "output") == 0) {
//     return OUTPUT;
//   }

//   if (strcmp(val, "input_pullup") == 0) {
//     return INPUT_PULLUP;
//   }

//   return INPUT;
// }

// void handleMessage(uint8_t *payload) {
//   JsonDocument doc;

//   DeserializationError error = deserializeJson(doc, payload);

//   // Test if parsing succeeds.
//   if (error) {
//     Serial.print(F("deserializeJson() failed: "));
//     Serial.println(error.f_str());
//     sendErrorMessage(error.c_str());
//     return;
//   }

//   if (!doc["type"].is<const char *>()) {
//     sendErrorMessage("invalid message type format");
//     return;
//   }

//   if (strcmp(doc["type"], "cmd") == 0) {
//     if (!doc["body"].is<JsonObject>()) {
//       sendErrorMessage("invalid command body");
//       return;
//     }

//     if (strcmp(doc["body"]["type"], "pinMode") == 0) {
//       /*
//       Uncomment this code for better validation of pinMode command

//       if (!doc["body"]["mode"].is<const char *>()) {
//         sendErrorMessage("invalid pinMode mode type");
//         return;
//       }

//       if (strcmp(doc["body"]["mode"], "input") != 0 &&
//           strcmp(doc["body"]["mode"], "input_pullup") != 0 &&
//           strcmp(doc["body"]["mode"], "output") != 0) {
//         sendErrorMessage("invalid pinMode mode value");
//         return;
//       }
//       */

//       pinMode(doc["body"]["pin"], toMode(doc["body"]["mode"]));
//       sendOkMessage();
//       return;
//     }

//     if (strcmp(doc["body"]["type"], "digitalWrite") == 0) {
//       digitalWrite(doc["body"]["pin"], doc["body"]["value"]);
//       sendOkMessage();
//       return;
//     }

//     if (strcmp(doc["body"]["type"], "digitalRead") == 0) {
//       auto value = digitalRead(doc["body"]["pin"]);

//       char msg[MSG_SIZE];

//       sprintf(msg, "{\"action\":\"msg\",\"type\":\"output\",\"body\":%d}",
//               value);

//       wsClient.sendTXT(msg);
//       return;
//     }

//     sendErrorMessage("unsupported command type");
//     return;
//   }

//   sendErrorMessage("unsupported message type");
//   return;
// }

// void onWSEvent(WStype_t type, uint8_t *payload, size_t length) {
//   switch (type) {
//   case WStype_CONNECTED:
//     Serial.println("WS Connected");
//     break;
//   case WStype_DISCONNECTED:
//     Serial.println("WS Disconnected");
//     break;
//   case WStype_TEXT:
//     Serial.printf("WS Message: %s\n", payload);

//     handleMessage(payload);

//     break;
//   }
// }
//tohere

// IRremote irRemote(IR_INPUT, IR_OUTPUT);
WiFiManager wifiManager;
bool flag = true;
bool wps_flag = false;

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
  WebSocketAWS wsSocket;
  wsSocket.startConnection(WS_HOST, WS_PORT, WS_URL, "", "wss");
  Serial.println("Successfully connected to AWS Websocket.");
}

void loop() {
  if(flag && WiFi.status() == WL_CONNECTED){
    digitalWrite(LED_BLUE_PIN, HIGH);
    if (wps_flag) wifiManager.saveWiFiCredentials();
    flag = false;
  }
  if(digitalRead(BUTTON_RIGHT_PIN)==LOW) {
    Serial.println("Attempting WPS Connection.");
    wifiManager.connectWPS();
    wps_flag = true;
    delay(1000);
  }
}