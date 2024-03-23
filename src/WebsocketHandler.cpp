/*
Class that is used as a gateway between AWS and ESP.
*/
#define MSG_SIZE 256 

#include "WebsocketHandler.h"
WebSocketsClient wsClient;
IRremote irRemote(IR_INPUT, IR_OUTPUT);


WebSocketAWS::WebSocketAWS() {}

void WebSocketAWS::loop() { wsClient.loop(); }

bool WebSocketAWS::isConnected() { return wsClient.isConnected(); }

void WebSocketAWS::sendMessage(const char *type, 
                                 const char *error){
    //TODO : Change json format
    char msg[MSG_SIZE];

    sprintf(msg, "{\"action\":\"msg\",\"type\":\"%s\", \"body\":\"%s\"}",type, error);

    wsClient.sendTXT(msg);

}
void WebSocketAWS::handleCommand(JsonDocument doc){
    //TODO: Implement other protocols bit/sizes

    if (strcmp(doc["body"]["type"], "receive")==0){    
        //Convert string to uint16_t
        decode_results remote_code = irRemote.getCode();
        std::string hex_code = std::to_string(remote_code.value);
        sendMessage("code", hex_code.c_str());
    }

    if (strcmp(doc["body"]["type"], "send")==0){
        irRemote.sendCode(doc["body"]["code"]);
        sendMessage("confirmation", "ok");
    }
}

void WebSocketAWS::handleMessage(uint8_t * payload){
    //TODO: handle different types of errors

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error){
        Serial.print(F("deserializeJson failed: "));
        Serial.println(error.f_str());
        sendMessage("error", error.c_str());
        return ;
    }

    if (!doc["type"].is<const char *>()){
        sendMessage("error", "Invalid message type.");
        return;
    }

    if (strcmp(doc["type"], "cmd") == 0){
        if (!doc["body"].is<JsonObject>()){
            sendMessage("error", "Invalid command body.");
            return;
        }
        handleCommand(doc);
    }
}


void WebSocketAWS::onWSEvent(WStype_t type, 
                               uint8_t * payload, 
                               size_t length) {
    switch (type){
        case WStype_CONNECTED:
            Serial.println("WebSocket Succesfully Connected.");
            break;
        case WStype_DISCONNECTED:
            Serial.println("WebSocket Disconnected.");
            break;
        case WStype_TEXT:
            Serial.printf("WebSocket Message: %s\n", payload);
            handleMessage(payload);
            break;
        default:
            Serial.print("Received type: " );
            Serial.println(type);
            break;
        //case WStype_ERROR:
        
    }
}

void WebSocketAWS::startConnection(const char *ws_host, 
                                   uint16_t ws_port, 
                                   const char *ws_url,
                                   const char *ws_fingerprint, 
                                   const char *ws_protocol){

   
    wsClient.beginSSL(ws_host, ws_port, ws_url, "", ws_protocol);
    wsClient.onEvent(onWSEvent);
}
