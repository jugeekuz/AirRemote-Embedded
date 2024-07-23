#ifndef WEBSERVER_ROUTER_H
#define WEBSERVER_ROUTER_H

#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <esp_wifi.h>
#include "wifi_handler/wifi_handler.h"
#include "./pages/index_html.h"
#include "webserver/utils/html_utils.h"
#include "utils/eeprom_utils.h"
#include "webserver/webserver_portal.h"

namespace WebServerRouter {

    ArRequestHandlerFunction redirectSetupOpt(AsyncWebServerRequest *request);

    ArRequestHandlerFunction wifiSubmit(AsyncWebServerRequest *request, WebServer *web_server);

    ArRequestHandlerFunction wifiConnect(AsyncWebServerRequest *request, bool completeSetup, WebServer *web_server);

    ArRequestHandlerFunction wpsConnect(AsyncWebServerRequest *request, bool completeSetup, WebServer *web_server);

    ArRequestHandlerFunction route(const char *path, WebServer *web_server);

} 