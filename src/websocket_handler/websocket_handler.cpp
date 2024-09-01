/*
Class that is used as a gateway between AWS and ESP.
*/
#define WEBSOCKET_BLINK_FREQUENCY 10

#include "websocket_handler/websocket_handler.h"

WebSocketHandler::WebSocketHandler(IRremote** irRemote, AsyncLED *led): irRemote(irRemote), WebSocketsClient(), led(led) {
    decode_results dec_results;
}

void WebSocketHandler::sendMSG(char *payload){
    Serial.printf("[WSc][INFO] Sending message : %s\n", payload);
    this->sendTXT(payload);
}
void WebSocketHandler::sendErrorMessage(const char * requestid, const char * body){
    char *error_msg;
    try{
        error_msg = new char[ERROR_RESPONSE_LENGTH];
    } catch (const std::bad_alloc& e) {
        Serial.printf("[WSc][ERROR] Memory allocation failed: %s\n",e.what());
        return;
    }
    sprintf(error_msg, "{\"action\":\"error\",\"requestId\":\"%s\",\"body\":\"%s\"}", requestid, body);
    sendMSG(error_msg);
    delete[] error_msg;
}

void WebSocketHandler::handleExecuteCommand(const char * requestId, 
                                            const char * commandSize, 
                                            const char * buttonCode){
    size_t buffer_size = atoi(commandSize);

    uint16_t* raw_buffer = nullptr;
    try{
        raw_buffer = new uint16_t[buffer_size];
        
        Utils::stringToArray(buttonCode, buffer_size, raw_buffer);
        (*irRemote)->sendCode(raw_buffer, buffer_size);

        delete[] raw_buffer;

    } catch (const std::bad_alloc& e) {
        Serial.printf("[WSc][ERROR] Memory allocation failed: %s\n",e.what());

        sendErrorMessage(requestId, "[WSc][ERROR] Memory allocation failed");

        return;
    } catch (...) {
        delete[] raw_buffer;

        Serial.println("[WSc][ERROR] An Unknown error occured.");

        sendErrorMessage(requestId, "[WSc][ERROR] An Unknown error occured.");
        return;
    }    
    
    char *payload;
    try{
        payload = new char[ACK_RESPONSE_LENGTH];
        sprintf(payload, "{\"action\":\"ack\",\"requestId\":\"%s\"}", requestId);
        sendMSG(payload);
        delete[] payload;
    } catch (const std::bad_alloc& e) {
        Serial.printf("[WSc][ERROR] Memory allocation failed: %s\n",e.what());
        sendErrorMessage(requestId, "[WSc][ERROR] Memory allocation failed");
        return;
    } catch (...) {
        delete[] payload;
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

            sendMSG(response_payload);

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
        sendMSG("[WSc][ERROR] Invalid message format.");
    }    
}

void WebSocketHandler::wsEventCallback( WStype_t type, 
                                        uint8_t * payload, 
                                        size_t length) {
    switch (type){
        case WStype_TEXT:
            Serial.printf("[WSc][INFO] Received Message: %s\n", payload);
            handleMessage(payload);
            break;
        case WStype_CONNECTED:
            Serial.println("[WSc][INFO] Connected.");
            this->led->setState(HIGH);
            break;
        case WStype_DISCONNECTED:  
            Serial.println("[WSc][INFO] Disconnected.");
            break;       
        case WStype_ERROR:   
            Serial.println("[WSc][ERROR] Encountered an error.");
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

void WebSocketHandler::startConnection( const char *ws_host, 
                                        uint16_t ws_port, 
                                        const char *ws_url,
                                        const char *ws_fingerprint, 
                                        const char *ws_protocol){
   
    this->beginSSL(ws_host, ws_port, ws_url, "", ws_protocol);
    
    memset(&this->ws_host, '\0', sizeof(this->ws_host));
    memset(&this->ws_url, '\0', sizeof(this->ws_url));

    strncpy(this->ws_host, ws_host, strlen(ws_host));
    strncpy(this->ws_url, ws_url, strlen(ws_url));
    this->ws_port = ws_port;
   
    WebSocketsClient::WebSocketClientEvent cbEvent = [this](WStype_t type, uint8_t * payload, size_t length){
        wsEventCallback(type, payload, length);
    };

    this->onEvent(cbEvent);
    this->led->blink(WEBSOCKET_BLINK_FREQUENCY);
}

void WebSocketHandler::saveWebSocketCredentials(){
    
    memset(&websocket_credentials.ws_host, '\0', sizeof(websocket_credentials.ws_host));
    memset(&websocket_credentials.ws_url, '\0', sizeof(websocket_credentials.ws_url));

    strncpy(websocket_credentials.ws_host, this->ws_host, strlen(this->ws_host));

    //Copy without the query parameters
    const char* queryPos = strchr(this->ws_url, '?');
    size_t copyLength = queryPos ? static_cast<size_t>(queryPos - this->ws_url) : strlen(this->ws_url);
    strncpy(websocket_credentials.ws_url, this->ws_url, copyLength);
    websocket_credentials.ws_url[copyLength] ='\0';
    websocket_credentials.ws_port = this->ws_port;

    EEPROMUtils::updateWebSocketConfig();
}