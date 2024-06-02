/*
Class that is used as a gateway between AWS and ESP.
*/

#include <string>
#include "WebsocketHandler.h"
#include "utils.h"

WebSocketAWS::WebSocketAWS(IRremote** irRemote): irRemote(irRemote), wsClient() {}

void WebSocketAWS::loop() { 
    this->wsClient.loop(); 
}

bool WebSocketAWS::isConnected() { 
    return this->wsClient.isConnected(); 
}

void WebSocketAWS::disconnect(){
    this->wsClient.disconnect();
}

void WebSocketAWS::sendTXT(char *payload){
    Serial.printf("[WSc] Sending message : %s\n", payload);
    this->wsClient.sendTXT(payload);
}
void WebSocketAWS::enableHeartbeat(uint32_t pingInterval, uint32_t pongTimeout, uint8_t disconnectTimeoutCount){
    this->wsClient.enableHeartbeat(pingInterval, pongTimeout, disconnectTimeoutCount);
}
void WebSocketAWS::sendErrorMessage(const char * requestid, const char * body){
    char error_msg[ERROR_RESPONSE_LENGTH];
    sprintf(error_msg, "{\"action\":\"error\",\"requestId\":\"%s\",\"body\":\"%s\"}", requestid, body);
    Serial.println(error_msg);
    sendTXT(error_msg);
}

void WebSocketAWS::handleExecuteCommand(const char * requestId, const char * commandSize, const char * buttonCode){
    size_t buffer_size = atoi(commandSize);

    uint16_t* raw_buffer = nullptr;
    try{
        raw_buffer = new uint16_t[buffer_size];
        
        Utils::stringToArray(buttonCode, buffer_size, raw_buffer);
        (*irRemote)->sendCode(raw_buffer, buffer_size);

        delete[] raw_buffer;

        char payload[ACK_RESPONSE_LENGTH];
        sprintf(payload, "{\"action\":\"ack\",\"requestId\":\"%s\"}", requestId);
        Serial.println("Sending response : ");
        Serial.println(payload);
        sendTXT(payload);

    } catch (const std::bad_alloc& e) {
        Serial.printf("[ERROR] Memory allocation failed: %s\n",e.what());

        sendErrorMessage(requestId, "[ERROR] Memory allocation failed");

        return;
    } catch (...) {
        delete[] raw_buffer;

        Serial.println("[ERROR] An Unknown error occured.");

        sendErrorMessage(requestId, "[ERROR] An Unknown error occured.");
    }
    
    
}

void WebSocketAWS::handleReadCommand(const char * requestId){
    decode_results results;
    if ((*irRemote)->getCode(&results, TIMEOUT_READ_SECONDS) && !results.repeat && !results.overflow){

        char* response_payload = nullptr;

        try{
            // The size of the response_payload is defined as the size of the key-value pairs in the beggining plus the size of the buttonCode along with the commas and brackets 
            response_payload = new char[FIXED_READ_RESPONSE_LENGTH + results.rawlen*DIGITS_UINT16_T + results.rawlen + 1];

            sprintf(response_payload, "{\"action\":\"ack\",\"requestId\":\"%s\",\"commandSize\":\"%d\",\"buttonCode\":", requestId, results.rawlen);

            Utils::arrayToString(resultToRawArray(&results), results.rawlen, response_payload);
            strcat(response_payload, "}");

            Serial.println("Sending response : ");
            Serial.println(response_payload);
            sendTXT(response_payload);

            delete[] response_payload;
        } catch (const std::bad_alloc& e) {

            Serial.printf("[ERROR] Memory allocation failed: %s\n",e.what());

            sendErrorMessage(requestId, "[ERROR] Memory allocation failed");

            return;

        } catch (...) {
            delete[] response_payload;

            Serial.println("[ERROR] An Unknown error occured.");

            sendErrorMessage(requestId, "[ERROR] An Unknown error occured.");

        }

    } else {
        sendErrorMessage(requestId, "[ERROR] No code was received.");
    }

}

void WebSocketAWS::handleMessage(uint8_t * request_payload){

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, request_payload);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }
    
    if (!doc.containsKey("action") || !doc.containsKey("requestId") || !doc.containsKey("cmd")) {
        Serial.println("[WSc][ERROR] Received a request that was invalid.");
        return;
    }
    if (!doc["action"].is<const char*>() || !doc["requestId"].is<const char*>() || !doc["cmd"].is<const char*>()) {
        Serial.println("[WSc][ERROR] Received a request that was invalid.");
        return;
    }

    if (strcmp(doc["action"], "cmd") != 0){ 
        sendErrorMessage(doc["requestId"], "[WSc][ERROR] Provided route key not supported.");
        return;
    }

    if (strcmp(doc["cmd"], "read") == 0){
        handleReadCommand(doc["requestId"]);

    } else if (strcmp(doc["cmd"], "execute") == 0){

        if(!doc.containsKey("commandSize") || !doc.containsKey("buttonCode")){
            sendErrorMessage(doc["requestId"], "[WSc][ERROR] Invalid message format.");
            return;
        }
        if (!doc["commandSize"].is<const char*>() || !doc["buttonCode"].is<const char*>()) {
            sendErrorMessage(doc["requestId"], "[WSc][ERROR] Invalid message format.");
            return;
        }
        handleExecuteCommand(doc["requestId"], doc["commandSize"], doc["buttonCode"]);

    } else {
        sendErrorMessage(doc["requestId"], "[WSc][ERROR] Provided `cmd` is invalid.");
    }
    
}

void WebSocketAWS::onWSEvent(WStype_t type, 
                             uint8_t * payload, 
                             size_t length) {
    switch (type){
        case WStype_TEXT:
            Serial.printf("[WSc] Received Message: %s\n", payload);
            handleMessage(payload);
            break;
        case WStype_CONNECTED:
            Serial.println("[WSc] Connected.");
            break;
        case WStype_DISCONNECTED:  
            Serial.println("[WSc] Disconnected.");
            break;       
        case WStype_ERROR:   
            break;
        default:
            // Serial.print("Received type: " );
            Serial.println(type);
            break;        
    }
}

void WebSocketAWS::startConnection(const char *ws_host, 
                                   uint16_t ws_port, 
                                   const char *ws_url,
                                   const char *ws_fingerprint, 
                                   const char *ws_protocol){
   
    this->wsClient.beginSSL(ws_host, ws_port, ws_url, "", ws_protocol);
    
    WebSocketsClient::WebSocketClientEvent cbEvent = [this](WStype_t type, uint8_t * payload, size_t length){
        onWSEvent(type, payload, length);
    };

    this->wsClient.onEvent(cbEvent);
}
