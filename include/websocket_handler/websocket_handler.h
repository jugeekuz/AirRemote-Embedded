#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#define TIMEOUT_READ_SECONDS 20
#define DIGITS_UINT16_T 5

#define SAMPLE_ERROR_RESPONSE "{\"action\":\"error\",\"requestId\":\"xVB_1716157277.932128\",\"body\":\"Lorem ipsum dolor sit amet, consectetur adipiscing elit.Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\"}"
#define SAMPLE_ACK_RESPONSE "{\"action\":\"ack\",\"requestId\":\"xVB_1716157277.932128\"}"
#define SAMPLE_FIXED_READ_RESPONSE "{\"action\":\"ack\",\"requestId\":\"xVB_1716157277.932128\",\"bufferSize\":\"255\",\"rawBuffer\":\"\"}"

#define ERROR_RESPONSE_LENGTH sizeof(SAMPLE_ERROR_RESPONSE)
#define ACK_RESPONSE_LENGTH sizeof(SAMPLE_ACK_RESPONSE)
#define FIXED_READ_RESPONSE_LENGTH sizeof(SAMPLE_FIXED_READ_RESPONSE)

#include <string>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <WebSocketsClient.h>
#include "utils/utils.h"
#include "ir_remote/ir_remote.h"

/**
 * @brief A class to handle WebSocket Connections.
 * 
 * This class provides functionality to handle WebSocket connections
 * and routes and executes incoming requests appropriately, sending response back.
 */
class WebSocketAWS {
public:
    WebSocketAWS(IRremote** irRemote);
    void handleExecuteCommand(const char * requestId, const char * commandSize, const char * buttonCode);
    void handleReadCommand(const char * requestId);
    void sendErrorMessage(const char * requestId, const char * error);
    void wsEventCallback(WStype_t type, uint8_t * payload, size_t length);
    void handleMessage(uint8_t * payload);
    void startConnection(const char *ws_host, uint16_t ws_port, const char *ws_url,const char *ws_fingerprint, const char *ws_protocol);
    bool isConnected();
    void loop();
    void disconnect();
    void sendTXT(char *payload);
    void enableHeartbeat(uint32_t pingInterval, uint32_t pongTimeout, uint8_t disconnectTimeoutCount);
private:
    WebSocketsClient wsClient;
    IRremote** irRemote;
};

#endif