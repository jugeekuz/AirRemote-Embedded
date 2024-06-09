/*
Class that is used as a gateway between AWS and ESP.
*/

#include "websocket_handler/websocket_handler.h"


WebSocketHandler::WebSocketHandler(IRremote** irRemote): irRemote(irRemote), wsClient() {
    // First code being read is junk, so read a code to initialize the IR receiver.
    decode_results dec_results;
    (*irRemote)->getCode(&dec_results);
}

void WebSocketHandler::loop() { 
    this->wsClient.loop(); 
}

bool WebSocketHandler::isConnected() { 
    return this->wsClient.isConnected(); 
}

void WebSocketHandler::disconnect(){
    this->wsClient.disconnect();
}

void WebSocketHandler::enableHeartbeat(uint32_t pingInterval, uint32_t pongTimeout, uint8_t disconnectTimeoutCount){
    this->wsClient.enableHeartbeat(pingInterval, pongTimeout, disconnectTimeoutCount);
}

void WebSocketHandler::sendTXT(char *payload){
    Serial.printf("[WSc][INFO] Sending message : %s\n", payload);
    this->wsClient.sendTXT(payload);
}
void WebSocketHandler::sendErrorMessage(const char * requestid, const char * body){
    char error_msg[ERROR_RESPONSE_LENGTH];
    sprintf(error_msg, "{\"action\":\"error\",\"requestId\":\"%s\",\"body\":\"%s\"}", requestid, body);
    sendTXT(error_msg);
}

void WebSocketHandler::handleExecuteCommand(const char * requestId, const char * commandSize, const char * buttonCode){
    size_t buffer_size = atoi(commandSize);

    uint16_t* raw_buffer = nullptr;
    try{
        raw_buffer = new uint16_t[buffer_size];
        
        Utils::stringToArray(buttonCode, buffer_size, raw_buffer);
        (*irRemote)->sendCode(raw_buffer, buffer_size);

        delete[] raw_buffer;

        char payload[ACK_RESPONSE_LENGTH];
        sprintf(payload, "{\"action\":\"ack\",\"requestId\":\"%s\"}", requestId);

        sendTXT(payload);

    } catch (const std::bad_alloc& e) {
        Serial.printf("[WSc][ERROR] Memory allocation failed: %s\n",e.what());

        sendErrorMessage(requestId, "[WSc][ERROR] Memory allocation failed");

        return;
    } catch (...) {
        delete[] raw_buffer;

        Serial.println("[WSc][ERROR] An Unknown error occured.");

        sendErrorMessage(requestId, "[WSc][ERROR] An Unknown error occured.");
    }    
}

void WebSocketHandler::handleReadCommand(const char * requestId){
    decode_results dec_results;
    if ((*irRemote)->getCode(&dec_results, TIMEOUT_READ_SECONDS) && !dec_results.repeat && !dec_results.overflow){

        char* response_payload = nullptr;

        try{
            // The size of the response_payload is defined as the size of the key-value pairs in the beggining plus the size of the buttonCode along with the commas and brackets 
            response_payload = new char[FIXED_READ_RESPONSE_LENGTH + dec_results.rawlen*DIGITS_UINT16_T + dec_results.rawlen + 1];

            sprintf(response_payload, "{\"action\":\"ack\",\"requestId\":\"%s\",\"commandSize\":\"%d\",\"buttonCode\":", requestId, dec_results.rawlen);

            Utils::arrayToString(resultToRawArray(&dec_results), dec_results.rawlen, response_payload);

            strcat(response_payload, "}");

            sendTXT(response_payload);

            delete[] response_payload;
        } catch (const std::bad_alloc& e) {

            Serial.printf("[WSc][ERROR] Memory allocation failed: %s\n",e.what());

            sendErrorMessage(requestId, "[WSc][ERROR] Memory allocation failed");

            return;

        } catch (...) {
            delete[] response_payload;

            Serial.println("[WSc][ERROR] An Unknown error occured.");

            sendErrorMessage(requestId, "[WSc][ERROR] An Unknown error occured.");
        }

    } else {
        sendErrorMessage(requestId, "[WSc][ERROR] No code was received.");
    }

}

void WebSocketHandler::handleMessage(uint8_t * request_payload){

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, request_payload);
    try{
        if (CommandValidators::checkExecuteCommand(request_payload)){

            handleExecuteCommand(doc["requestId"], doc["commandSize"], doc["buttonCode"]);

        } else if (CommandValidators::checkReadCommand(request_payload)){

            handleReadCommand(doc["requestId"]);

        } else if (ErrorValidators::checkError(request_payload)){

            Serial.printf("[WSc][ERROR] Received an error message : %s", doc["body"]);

        } else {

            sendErrorMessage(doc["requestId"], "[WSc][ERROR] Provided route key not supported.");
        }
    } catch (...) {
        sendTXT("[WSc][ERROR] Invalid message format.");
    }    
}

void WebSocketHandler::wsEventCallback(WStype_t type, 
                             uint8_t * payload, 
                             size_t length) {
    switch (type){
        case WStype_TEXT:
            Serial.printf("[WSc][INFO] Received Message: %s\n", payload);
            handleMessage(payload);
            break;
        case WStype_CONNECTED:
            Serial.println("[WSc][INFO] Connected.");
            break;
        case WStype_DISCONNECTED:  
            Serial.println("[WSc][INFO] Disconnected.");
            break;       
        case WStype_ERROR:   
            Serial.println("[WSc][INFO] Encountered an error.");
            break;
        case WStype_PONG:
            Serial.println("[WSc][INFO] Received PONG.");
            break;
        default:
            Serial.print("[WSc][INFO] Received type: ");
            Serial.println(type);
            break;        
    }
}

void WebSocketHandler::startConnection(const char *ws_host, 
                                   uint16_t ws_port, 
                                   const char *ws_url,
                                   const char *ws_fingerprint, 
                                   const char *ws_protocol){
   
    this->wsClient.beginSSL(ws_host, ws_port, ws_url, "", ws_protocol);
    
    WebSocketsClient::WebSocketClientEvent cbEvent = [this](WStype_t type, uint8_t * payload, size_t length){
        wsEventCallback(type, payload, length);
    };

    this->wsClient.onEvent(cbEvent);
}
