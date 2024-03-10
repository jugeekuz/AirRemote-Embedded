#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#include <string>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

#include "IRremoteHandler.h"

class WebSocketAzure: public WebSocketsClient{
    WebSocketAzure(IRremote &ir_remote, char *ws_host, uint8_t ws_port, char *ws_url,char *ws_fingerprint, char *ws_protocol);
    void handleCommand(JsonDocument doc);
    void sendMessage(const char *type, const char *error);
    void onWSEvent(WStype_t type, uint8_t * payload, size_t length);
    void handleMessage(uint8_t * payload);
    void startConnection(char *ws_host, uint8_t ws_port, char *ws_url,char *ws_fingerprint, char *ws_protocol);
protected:
    IRremote ir_remote;
    char *ws_host; 
    uint8_t ws_port; 
    char *ws_url;
    char *ws_fingerprint; 
    char *ws_protocol;
};
#endif