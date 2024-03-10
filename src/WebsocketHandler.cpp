/*
Class that is used as a gateway between Azure and ESP.
*/
#define MSG_SIZE 256 

#include "WebsocketHandler.h"


WebSocketAzure::WebSocketAzure(IRremote &ir_remote,
                               char *ws_host,
                               uint8_t ws_port,
                               char *ws_url,
                               char *ws_fingerprint, 
                               char *ws_protocol) 
                               : WebSocketsClient(),
                                 ir_remote(ir_remote){
    this->ws_host = ws_host;
    this->ws_port = ws_port;
    this->ws_url = ws_url;
    this->ws_fingerprint = ws_fingerprint;
    this->ws_protocol = ws_protocol;
}

void WebSocketAzure::sendMessage(const char *type, 
                                 const char *error){
    //TODO : Change json format
    char msg[MSG_SIZE];

    sprintf(msg, "{\"action\":\"msg\",\"type\":\"%s\", \"body\":\"%s\"}",type, error);

    sendTXT(msg);
}

void WebSocketAzure::handleCommand(JsonDocument doc){
    //TODO: Implement other protocols bit/sizes

    if (strcmp(doc["body"]["type"], "receive")==0){    
        //Convert string to uint16_t
        decode_results remote_code = ir_remote.getCode();
        std::string hex_code = std::to_string(remote_code.value);
        sendMessage("code", hex_code.c_str());
    }

    if (strcmp(doc["body"]["type"], "send")==0){
        ir_remote.sendCode(doc["body"]["code"]);
        sendMessage("confirmation", "ok");
    }
}

void WebSocketAzure::handleMessage(uint8_t * payload){
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


void WebSocketAzure::onWSEvent(WStype_t type, 
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
        //case WStype_ERROR:
        
    }
}
void WebSocketAzure::startConnection(char *ws_host, 
                                     uint8_t ws_port, 
                                     char *ws_url,
                                     char *ws_fingerprint, 
                                     char *ws_protocol){
    beginSSL(ws_host, ws_port, ws_url, ws_fingerprint, ws_protocol);
    onEvent([this](WStype_t type, uint8_t *payload, size_t length) {
        this->onWSEvent(type, payload, length);
    });
}
