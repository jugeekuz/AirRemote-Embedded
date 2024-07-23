#include "webserver/webserver_router.h"

namespace WebServerRouter {

	ArRequestHandlerFunction redirectSetupOpt(AsyncWebServerRequest *request) {
		return (
				[](AsyncWebServerRequest *request) {
					Serial.println("[WebServer][INFO] Redirecting to setup option.");
					if (request->hasParam("setup-option", true)) {
						AsyncWebParameter* p = request->getParam("setup-option", true);
						if (p->value() == "wifi-setup") {
							updateWifiClass("idle");
							AsyncWebServerResponse *response = request->beginResponse(200, "text/html", wifi_html);
							request->send(response);
							Serial.println("[WebServer][INFO] WiFi Setup selected.");
							
						} else if (p->value() == "complete-setup") {
							AsyncWebServerResponse *response = request->beginResponse(200, "text/html", api_html);
							request->send(response);
							Serial.println("[WebServer][INFO] Complete Setup selected.");
						}
					} else {            
						request->send(400, "text/plain", "Bad Request");
						Serial.println("[WebServer][ERROR] No setup option selected.");
					}
				});
	}


	ArRequestHandlerFunction wifiSubmit(AsyncWebServerRequest *request, WebServer *web_server) {
		return (
				[web_server](AsyncWebServerRequest *request) {
					if (!(request->hasParam("ssid", true) && request->hasParam("password", true))) {
						//Add validation
						request->send(400, "text/plain", "Missing parameters");
					} else {

						Serial.println("[WebServer][INFO] Received configuration parameters.");
						
						updateWifiClass("wifi");
						String ssid = request->getParam("ssid", true)->value();
						String password = request->getParam("password", true)->value();
						web_server->wifiHandler->setCredentials(ssid.c_str(), password.c_str());
						web_server->wifiHandler->connect(1);
						

						if (WiFi.status() == WL_CONNECTED) {
							AsyncWebServerResponse *response = request->beginResponse(200, "text/html", success_html);
							request->send(response);
							web_server->wifiHandler->saveWiFiCredentials();
							Serial.println("[WebServer][INFO] Received and saved wifi credentials.");
							webserverSubmitted = true;
						} else {
							AsyncWebServerResponse *response = request->beginResponse(200, "text/html", error_html);
							request->send(response);
						}
					}
				});
	}

	ArRequestHandlerFunction wifiConnect(AsyncWebServerRequest *request, bool completeSetup, WebServer *web_server) {
		return (
				[web_server, completeSetup](AsyncWebServerRequest *request) {
				if (!(request->hasParam("ssid", true) && request->hasParam("password", true))) {
					//Add validation
					request->send(400, "text/plain", "Missing parameters");
					
				} else {

					Serial.println("[WebServer][INFO] Received configuration parameters.");
					
					updateWifiClass("wifi");
					String ssid = request->getParam("ssid", true)->value();
					String password = request->getParam("password", true)->value();
					web_server->wifiHandler->setCredentials(ssid.c_str(), password.c_str());
					web_server->wifiHandler->connect(1);
					

					if (WiFi.status() == WL_CONNECTED) {
						if (completeSetup) {
							request->send(request->beginResponse(200, "text/html", api_html));

						} else {
							request->send(request->beginResponse(200, "text/html", success_html));
						}
						web_server->wifiHandler->saveWiFiCredentials();
						Serial.println("[WebServer][INFO] Received and saved wifi credentials.");
						webserverSubmitted = true;
					} else {
						request->send(request->beginResponse(200, "text/html", error_html));
						Serial.println("[WebServer][INFO] Received and saved wifi credentials.");
					}
				}        
				});
	}

	ArRequestHandlerFunction wpsConnect(AsyncWebServerRequest *request, bool completeSetup, WebServer *web_server) {
		return (
				[web_server](AsyncWebServerRequest *request) {
					if (request->hasParam("wps", true)) {
						AsyncWebParameter* p = request->getParam("wps", true);
						if (p->value() == "connect") {
							updateWifiClass("wps");
							web_server->wifiHandler->connectWPS();
							while (WiFi.status() != WL_CONNECTED);
							AsyncWebServerResponse *response = request->beginResponse(200, "text/html", success_html);
							request->send(response);
							web_server->wifiHandler->saveWiFiCredentials();
							Serial.println("[WebServer][INFO] Received and saved wifi credentials.");
							webserverSubmitted = true;
						}
					} else {
						request->send(400, "text/plain", "Bad Request");
						Serial.println("[WebServer][ERROR] No WPS option selected.");
					}
				});
	}
	ArRequestHandlerFunction route(char *path, WebServer *web_server) {
		if (strcmp(path,"/") == 0) {

			return ([](AsyncWebServerRequest *request){ 
						return redirectSetupOpt(request);
			});

		} else if (strcmp(path,"/wifi-submit") == 0){

			return ([web_server](AsyncWebServerRequest *request){ 
						return wifiConnect(request, false, web_server);
			});

		} else if (strcmp(path,"/wifi-complete-submit") == 0) {

			return ([web_server](AsyncWebServerRequest *request){ 
						return wifiConnect(request, true, web_server);
			});

		} 
	}
}