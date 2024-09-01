#ifndef WEBSERVER_PORTAL_H
#define WEBSERVER_PORTAL_H

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <esp_wifi.h>
#include "wifi_handler/wifi_handler.h"
#include "./pages/index_html.h"
#include "webserver/html_utils.h"
#include "utils/eeprom_utils.h"
#include "websocket_handler/websocket_handler.h"
#include "async_led/async_led.h"

#define MAX_CLIENTS 4	
#define WIFI_CHANNEL 6	

const IPAddress localIP(4, 3, 2, 1);		   
const IPAddress gatewayIP(4, 3, 2, 1);		   
const IPAddress subnetMask(255, 255, 255, 0);  

const String localIPURL = "http://4.3.2.1";	 

extern bool webserverSubmitted;
extern String macAddress;
void wifiConnectTask(void *parameter);
void webSocketConnectTask(void *parameter);
void websocketSaveCredentialsTask(void *parameter);

class WebServer {
public:
    WebServer(WiFiHandler *wiFiHandler, WebSocketHandler *webSocket, AsyncLED *led);
    ~WebServer();
    void setup();
    void loop();
    ArRequestHandlerFunction redirectSetupOpt();
    ArRequestHandlerFunction wifiConnect();
    ArRequestHandlerFunction wpsConnect();
    ArRequestHandlerFunction apiSubmit();
    ArRequestHandlerFunction checkWiFiStatus();
    ArRequestHandlerFunction webSocketConnect();
    ArRequestHandlerFunction checkWebSocketStatus();

    ArRequestHandlerFunction getSuccessHtml(); 
    ArRequestHandlerFunction getErrorHtml();
    ArRequestHandlerFunction getWifiHtml();
    ArRequestHandlerFunction getApiHtml();
    ArRequestHandlerFunction route(const char *path); 
    
    ArRequestHandlerFunction getWifiList();
    ArRequestHandlerFunction wifiScan();
private:
    AsyncLED *led;
    WiFiHandler *wifiHandler;
    WebSocketHandler *webSocket;
};
#endif