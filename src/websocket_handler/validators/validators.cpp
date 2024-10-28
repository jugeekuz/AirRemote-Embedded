#include "websocket_handler/validators/validators.h"

namespace MessageValidators{
    void checkMessage(uint8_t * request_payload){
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, request_payload);

        if (error) {
            Serial.print(F("[WSc][ERROR] deserializeJson() failed: "));
            Serial.println(error.f_str());
            Serial.println(error.c_str());
            return;
        }

        if (!doc.containsKey("action") || !(doc.containsKey("requestId") || doc.containsKey("automationId"))) {
            Serial.println("[WSc][ERROR] Received a request that was invalid.");
            throw std::runtime_error("[WSc][ERROR] Invalid message format.");
            return;
        }

        if (!doc["action"].is<const char*>() || !(doc["requestId"].is<const char*>() || doc["automationId"].is<const char*>())) {
            Serial.println("[WSc][ERROR] Received a request that was invalid.");
            throw std::runtime_error("[WSc][ERROR] Invalid message format.");
            return;
        }
    }
}

namespace CommandValidators{
    bool checkCommand(uint8_t * request_payload){
        
        MessageValidators::checkMessage(request_payload);
        
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, request_payload);

        if (strcmp(doc["action"], "cmd") != 0){ 
            return false;
        }

        if (!doc.containsKey("cmd") || !doc["cmd"].is<const char*>()) {
            throw std::runtime_error("[WSc][ERROR] Invalid message format.");
            return false;
        }

        return true;
    }

    bool checkReadCommand(uint8_t * request_payload){
        
        if (!checkCommand(request_payload)){
            return false;
        }

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, request_payload);

        if (strcmp(doc["cmd"], "read") != 0){
            return false;
        }
        return true;
    }

    bool checkExecuteCommand(uint8_t * request_payload){
        
        if (!checkCommand(request_payload)){
            return false;
        }

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, request_payload);

        if (strcmp(doc["cmd"], "execute") != 0){
            return false;
        }

        if(!doc.containsKey("commandSize") || !doc.containsKey("buttonCode") || !doc.containsKey("requestId")){
            return false;
        }
        if (!doc["commandSize"].is<const char*>() || !doc["buttonCode"].is<const char*>()) {
            Serial.println("[WSc][ERROR] Invalid message format.");
            return false;
        }
        return true;
    }

    bool checkAutomationCommand(uint8_t * request_payload){
        
        if (!checkCommand(request_payload)){
            return false;
        }

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, request_payload);

        if (strcmp(doc["cmd"], "execute") != 0){
            return false;
        }

        if(!doc.containsKey("commandSize") || !doc.containsKey("buttonCode") || !doc.containsKey("automationId")){
            return false;
        }
        if (!doc["commandSize"].is<const char*>() || !doc["buttonCode"].is<const char*>()) {
            Serial.println("[WSc][ERROR] Invalid message format.");
            return false;
        }
        return true;
    }
}

namespace ErrorValidators{
    bool checkError(uint8_t * request_payload){

        MessageValidators::checkMessage(request_payload);

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, request_payload);

        if (strcmp(doc["action"], "error") != 0){
            return false;
        }

        if (!doc.containsKey("body") || !doc["error"].is<const char*>()) {
            Serial.println("[WSc][ERROR] Invalid message format.");
            return false;
        }
        return true;
    }
}