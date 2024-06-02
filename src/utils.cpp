#include "utils.h"
namespace Utils{
    void arrayToString(volatile uint16_t* input, size_t len, char* buffer){
        strcat(buffer, "\"[");

        char uint16string[5];
        for (size_t i = 0; i < len-1; i++) {
            sprintf(uint16string, "%hu", input[i]);
            strcat(buffer, uint16string);
            strcat(buffer, ",");
        }
        sprintf(uint16string, "%u", input[len-1]);
        strcat(buffer, uint16string);
        strcat(buffer, "]\"");
    }

    void stringToArray(const char * input, size_t len, uint16_t* buffer){
        if(!input || !buffer) return;

        size_t index = 0;

        char * tempInput = nullptr;

        try{
            tempInput = new char[strlen(input) + 1];
            strcpy(tempInput, input);

            char *token = strtok(tempInput, "[,]");
            while (token != NULL && index < len) {
                buffer[index++] = (uint16_t)atoi(token);
                token = strtok(NULL, "[,]");
            }

            delete[] tempInput;
        }catch(const std::bad_alloc& e){
            Serial.printf("[ERROR] Memory allocation failed: %s\n",e.what()); 
            throw;
        }catch(...){
            delete[] tempInput;
            throw;
        }
        
    }

    uint8_t calculateChecksum(uint8_t* data, size_t len) {
        uint8_t checksum = 0;
        for (size_t i = 0; i < len; i++) {
            checksum ^= data[i];
        }
        return checksum;
    }

    String wpspin2string(uint8_t a[]){
        char wps_pin[9];
        for(int i=0;i<8;i++){
            wps_pin[i] = a[i];
        }
        wps_pin[8] = '\0';
        return (String)wps_pin;
    }

}

TaskHandle_t LED::blinkTaskHandle = NULL;  // Initialize the static task handle

LED::LED(const uint8_t pin): pin(pin) {
    pinMode(pin, OUTPUT);
}

void LED::blink(uint8_t frequency, uint8_t times) {

    BlinkParameters* params = new BlinkParameters{this->pin, frequency, times};

    xTaskCreate(
        blinkTask,     // Task function
        "BlinkTask",   // Name of the task (for debugging)
        1024,          // Stack size
        params,        // Parameters to pass to the task
        1,             // Task priority
        &blinkTaskHandle // Task handle
    );
}

void LED::setState(uint8_t val) {

    SetStateParameters* params = new SetStateParameters{this->pin, val};

    xTaskCreate(
        setStateTask, // Task function
        "SetStateTask", // Name of the task (for debugging)
        1024,           // Stack size
        params,         // Parameters to pass to the task
        1,              // Task priority
        NULL            // Task handle
    );
}

void LED::blinkTask(void* parameter) {

    BlinkParameters* params = static_cast<BlinkParameters*>(parameter);
    int delayTime = 1000 / params->frequency;

    for(int i = 0; i < params->times; i++) {
        digitalWrite(params->pin, HIGH);
        vTaskDelay(delayTime / portTICK_PERIOD_MS);
        digitalWrite(params->pin, LOW);
        vTaskDelay(delayTime / portTICK_PERIOD_MS);
    }

    delete params;
    xTaskNotifyGive(blinkTaskHandle);  // Notify that the blink task is complete
    vTaskDelete(NULL);
}

void LED::setStateTask(void* parameter) {

    SetStateParameters* params = static_cast<SetStateParameters*>(parameter);
    
    // Wait for the blink task to complete if it is running
    if (blinkTaskHandle != NULL) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    }

    digitalWrite(params->pin, params->val);

    delete params;
    vTaskDelete(NULL);
}
