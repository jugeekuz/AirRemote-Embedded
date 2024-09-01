#include "webserver/webserver.h"

/*---------------------------------------------------------------------------------
--------------------------------Task Callbacks-------------------------------------
----------------------------------------------------------------------------------*/

void wifiConnectTask(void *parameter) {

    auto *params = static_cast<std::tuple<WiFiHandler*, String, String>*>(parameter);
    WiFiHandler* wifiHandler = std::get<0>(*params);
    String ssid = std::get<1>(*params);
    String password = std::get<2>(*params);
    
    wifiHandler->setCredentials(ssid.c_str(), password.c_str());
    wifiHandler->connect(10);

	if (WiFi.status() == WL_CONNECTED) wifiHandler->saveWiFiCredentials();
	EEPROMUtils::loadConfig();

	delete params;
    vTaskDelete(NULL);
}

void wpsConnectTask(void *parameter) {
	auto *params = static_cast<std::tuple<WiFiHandler*>*>(parameter);
    WiFiHandler* wifiHandler = std::get<0>(*params);

	wifiHandler->connectWPS();

	unsigned long start_time = millis();
	while ((WiFi.status() != WL_CONNECTED) && (millis() - start_time) < 30000) {
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
	
	if (WiFi.status() == WL_CONNECTED) wifiHandler->saveWiFiCredentials();

	delete params;
    vTaskDelete(NULL);
}

void webSocketConnectTask(void *parameter) {
    auto *params = static_cast<std::tuple<WebSocketHandler*, String, String, uint16_t, String>*>(parameter);
    WebSocketHandler* webSocket = std::get<0>(*params);
    String ws_host = std::get<1>(*params);
    String ws_url = std::get<2>(*params);
	uint16_t ws_port = std::get<3>(*params);
    String macAddress = std::get<4>(*params);

	String query_parameters = ws_url;
	query_parameters += "?deviceType=iot&macAddress=";
	query_parameters += macAddress;
	webSocket->startConnection(ws_host.c_str(), ws_port, query_parameters.c_str(), "", "wss");
	webSocket->enableHeartbeat(3*1000, 5*1000,1);

	unsigned long start_time = millis();
	while ((millis() - start_time) < 10000) {
		webSocket->loop();
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}

	delete params;
    vTaskDelete(NULL);
}
void websocketSaveCredentialsTask(void *parameter) {
	auto *params = static_cast<std::tuple<WebSocketHandler*>*>(parameter);
	WebSocketHandler* webSocket = std::get<0>(*params);
	webSocket->saveWebSocketCredentials();
	
	delete params;
	vTaskDelete(NULL);
}

/*---------------------------------------------------------------------------------
---------------------------------Controllers---------------------------------------
----------------------------------------------------------------------------------*/

ArRequestHandlerFunction WebServer::redirectSetupOpt() {

	return [this](AsyncWebServerRequest *request) {
			if (request->hasParam("setup-option", true)) {
				AsyncWebParameter* p = request->getParam("setup-option", true);
				String redirectUrl = String(localIPURL) + "/wifi-page?setup=";

				redirectUrl += p->value();
    			request->redirect(redirectUrl);
				
			} else {            
				request->send(400, "text/plain", "Bad Request");
				Serial.println("[WebServer][ERROR] No setup option selected.");
			}
	};
}

ArRequestHandlerFunction WebServer::checkWiFiStatus() {
	return [this](AsyncWebServerRequest *request) {
		Serial.println("[WebServer][INFO] WiFi Status Requested");
		if (WiFi.status() == WL_CONNECTED) {			
			request->send(200, "application/json", "{\"status\": \"connected\"}");
		} else {
			request->send(200, "application/json", "{\"status\": \"not_connected\"}");
		}
	};
}

ArRequestHandlerFunction WebServer::wifiConnect() {

	return [this](AsyncWebServerRequest *request) {
			if (!(request->hasParam("ssid", true) && request->hasParam("password", true))) {
				//Add validation
				request->send(400, "text/plain", "Missing parameters");
				
			} else {
				Serial.println("[WebServer][INFO] Starting connection...");
				
				auto *params = new std::tuple<WiFiHandler*, String, String>(
					wifiHandler, 
					request->getParam("ssid", true)->value(), 
					request->getParam("password", true)->value()
				);

				if (xTaskCreate(
						wifiConnectTask, 
						"WiFiConnectTask", 
						4096, 
						params, 
						1, 
						NULL) != pdPASS) {
							delete params;
							request->send(500, "text/plain", "Failed to create task");
				} else {
					request->send(200, "text/plain", "WiFi started connection");
            	}
				
			}        
	};
}

ArRequestHandlerFunction WebServer::checkWebSocketStatus() {
	return [this](AsyncWebServerRequest *request) {
		Serial.println("[WebServer][INFO] WebSocket Status Requested");
		if (webSocket->isConnected()) {
			auto *params = new std::tuple<WebSocketHandler*>(
					webSocket
				);
			if (xTaskCreate(
						websocketSaveCredentialsTask, 
						"websocketSaveCredentialsTask", 
						4096, 
						params, 
						1, 
						NULL) != pdPASS) {
							delete params;
						request->send(500, "application/json", "{\"status\": \"not_connected\"}");
				} else {
					request->send(200, "application/json", "{\"status\": \"connected\"}");
            	}

			request->send(200, "application/json", "{\"status\": \"connected\"}");
		} else {
			request->send(200, "application/json", "{\"status\": \"not_connected\"}");
		}
	};
}

ArRequestHandlerFunction WebServer::webSocketConnect() {

	return [this](AsyncWebServerRequest *request) {
			if (!(request->hasParam("WS_HOST", true) && request->hasParam("WS_PORT", true) && request->hasParam("WS_URL", true))) {
				//Add validation
				request->send(400, "text/plain", "Missing parameters");
				
			} else {
				Serial.println("[WebServer][INFO] Starting connection...");
				
				auto *params = new std::tuple<WebSocketHandler*, String, String, uint16_t, String>(
					webSocket, 
					request->getParam("WS_HOST", true)->value(), 
					request->getParam("WS_URL", true)->value(), 
					request->getParam("WS_PORT", true)->value().toInt(), 
					wifiHandler->macAddress()
				);

				if (xTaskCreate(
						webSocketConnectTask, 
						"webSocketConnectTask", 
						4096, 
						params, 
						1, // Bigger priority than main loop
						NULL) != pdPASS) {
							delete params;
							request->send(500, "text/plain", "Failed to create task");
				} else {
					request->send(200, "text/plain", "Websocket started connection");
				}
			}        
	};
}

ArRequestHandlerFunction WebServer::wpsConnect() {

	return [this](AsyncWebServerRequest *request) {
			Serial.println("[WebServer][INFO] Starting WPS connection...");
			
			auto *params = new std::tuple<WiFiHandler*>(
				wifiHandler
			);

			if (xTaskCreate(
					wpsConnectTask, 
					"wpsConnectTask", 
					4096, 
					params, 
					1, 
					NULL) != pdPASS) {
						delete params;
						request->send(500, "text/plain", "Failed to create task");
			} else {
				request->send(200, "text/plain", "WiFi started connection");
			}    
	};
}
ArRequestHandlerFunction WebServer::getSuccessHtml() {
	return [](AsyncWebServerRequest *request) {
			AsyncWebServerResponse *response = request->beginResponse(200, "text/html", success_html);
			request->send(response);
			esp_restart();    
	};
}

ArRequestHandlerFunction WebServer::getErrorHtml() {

	return [](AsyncWebServerRequest *request) {
			AsyncWebServerResponse *response = request->beginResponse(200, "text/html", error_html);
			request->send(response);    
	};
}

ArRequestHandlerFunction WebServer::getWifiHtml() {
	updateWifiPage();

	return [](AsyncWebServerRequest *request) {
			AsyncWebServerResponse *response = request->beginResponse(200, "text/html", wifi_html);
			request->send(response);    
	};
}

ArRequestHandlerFunction WebServer::getApiHtml() {
	return [](AsyncWebServerRequest *request) {
			AsyncWebServerResponse *response = request->beginResponse(200, "text/html", api_html);
			request->send(response);    
	};
}