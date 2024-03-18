#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#define IR_INPUT 15
#define IR_OUTPUT 4

#include <string>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>

#include "IRremoteHandler.h"


class WebSocketAWS{
public:
    WebSocketAWS();
    // WebSocketAWS(IRremote &ir_remote, const char *ws_host, uint16_t ws_port, const char *ws_url,const char *ws_fingerprint, const char *ws_protocol);
    static void handleCommand(JsonDocument doc);
    static void sendMessage(const char *type, const char *error);
    static void onWSEvent(WStype_t type, uint8_t * payload, size_t length);
    static void handleMessage(uint8_t * payload);
    static void startConnection(const char *ws_host, uint16_t ws_port, const char *ws_url,const char *ws_fingerprint, const char *ws_protocol);
};
#endif