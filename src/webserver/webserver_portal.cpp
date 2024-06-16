#include "webserver/webserver_portal.h"

#define DNS_INTERVAL 30
#define MAX_CLIENTS 4
#define WIFI_CHANNEL 6

#define WIFI_PORTAL_NAME "AirRemote-Portal"
#define WIFI_PORTAL_PASSWORD NULL

DNSServer *dnsServer = nullptr;
AsyncWebServer *server = nullptr;
// DNSServer dnsServer;
// AsyncWebServer server(80);
bool webserverSubmitted = false;

WebServer::WebServer(){
    try{
        dnsServer = new DNSServer();
        server = new AsyncWebServer(80);
    } catch(const std::bad_alloc& e) {
        Serial.printf("[WebServer][ERROR] Memory allocation failed: %s\n",e.what());
    } catch(...){
        Serial.println("[WebServer][ERROR] Failed to initialize WebServer.");
    }
}   

WebServer::~WebServer(){
    try{
        server->end();
        dnsServer->stop();
    } catch (...) {
        Serial.println("[WebServer][ERROR] Failed to stop WebServer.");
    }
    delete server;
    delete dnsServer; 
}
/*
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WiFi Credentials</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f4f4f9;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .container {
            background: white;
            padding: 2em;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
            border-radius: 8px;
            width: 300px;
            text-align: center;
        }
        h1 {
            margin-bottom: 1.5em;
            font-size: 1.5em;
            color: #333;
        }
        form {
            display: flex;
            flex-direction: column;
        }
        label {
            margin-bottom: 0.5em;
            font-weight: bold;
            color: #555;
        }
        input, select {
            padding: 0.5em;
            margin-bottom: 1em;
            border: 1px solid #ccc;
            border-radius: 4px;
            font-size: 1em;
        }
        button {
            padding: 0.7em;
            border: none;
            border-radius: 4px;
            background-color: #007BFF;
            color: white;
            font-size: 1em;
            cursor: pointer;
        }
        button:hover {
            background-color: #0056b3;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>AirRemote Setup</h1>
        <form action="/submit" method="POST">
            <label for="ws_host">WebSocket Host:</label>
            <input type="text" id="ws_host" name="WS_HOST" required>
            
            <label for="ws_port">WebSocket Port:</label>
            <input type="number" id="ws_port" name="WS_PORT" value="443" required>
            
            <label for="ws_url">WebSocket URL:</label>
            <input type="text" id="ws_url" name="WS_URL" required>
            
            <label for="wifi_ssid">WiFi SSID:</label>
            <select id="wifi_ssid" name="WIFI_SSID" required>
*/
char index_html[2000] = R"=====(<!doctypehtml><html lang=en><meta charset=UTF-8><meta content="width=device-width,initial-scale=1"name=viewport><title>WiFi Credentials</title><style>body{font-family:Arial,sans-serif;background-color:#f4f4f9;display:flex;justify-content:center;align-items:center;height:100vh;margin:0}.container{background:#fff;padding:2em;box-shadow:0 0 10px rgba(0,0,0,.1);border-radius:8px;width:300px;text-align:center}h1{margin-bottom:1.5em;font-size:1.5em;color:#333}form{display:flex;flex-direction:column}label{margin-bottom:.5em;font-weight:700;color:#555}input,select{padding:.5em;margin-bottom:1em;border:1px solid #ccc;border-radius:4px;font-size:1em}button{padding:.7em;border:none;border-radius:4px;background-color:#007bff;color:#fff;font-size:1em;cursor:pointer}button:hover{background-color:#0056b3}</style><div class=container><h1>AirRemote Setup</h1><form action=/submit method=POST><label for=ws_host>WebSocket Host:</label> <input id=ws_host name=WS_HOST required> <label for=ws_port>WebSocket Port:</label> <input id=ws_port name=WS_PORT required type=number value=443> <label for=ws_url>WebSocket URL:</label> <input id=ws_url name=WS_URL required> <label for=wifi_ssid>WiFi SSID:</label> <select id=wifi_ssid name=WIFI_SSID required>)=====";

// New method to scan networks and update HTML
void WebServer::scanAndUpdateHTML() {
    int n = WiFi.scanNetworks();
    if (n == 0) {
        strcat(index_html, "<option value=\"\">No networks found</option>");
    } else {
        for (int i = 0; i < n; ++i) {
            strcat(index_html, "<option value=\"");
            strcat(index_html, WiFi.SSID(i).c_str());
            strcat(index_html, "\">");
            strcat(index_html, WiFi.SSID(i).c_str());
            strcat(index_html, "</option>");
        }
    }
    strcat(index_html, R"=====(<label for=wifi_password>WiFi Password:</label> <input id=wifi_password name=WIFI_PASSWORD required type=password> <button type=submit>Submit</button>)=====");
}


void WebServer::setup() {

    scanAndUpdateHTML();

	WiFi.mode(WIFI_MODE_AP);

	const IPAddress subnetMask(255, 255, 255, 0);

	WiFi.softAPConfig(localIP, gatewayIP, subnetMask);
	WiFi.softAP(WIFI_PORTAL_NAME, WIFI_PORTAL_PASSWORD, WIFI_CHANNEL, 0, MAX_CLIENTS);

	esp_wifi_stop();
	esp_wifi_deinit();
	wifi_init_config_t my_config = WIFI_INIT_CONFIG_DEFAULT();
	my_config.ampdu_rx_enable = false;
	esp_wifi_init(&my_config);
	esp_wifi_start();
	vTaskDelay(100 / portTICK_PERIOD_MS); 

	dnsServer->setTTL(3600);
	dnsServer->start(53, "*", localIP);

	server->on("/connecttest.txt", [](AsyncWebServerRequest *request) { request->redirect("http://logout.net"); });	// windows 11 captive portal workaround
	server->on("/wpad.dat", [](AsyncWebServerRequest *request) { request->send(404); });								// Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

	server->on("/generate_204", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });		   // android captive portal redirect
	server->on("/redirect", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // microsoft redirect
	server->on("/hotspot-detect.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });  // apple call home
	server->on("/canonical.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });	   // firefox captive portal call home
	server->on("/success.txt", [](AsyncWebServerRequest *request) { request->send(200); });					   // firefox captive portal call home
	server->on("/ncsi.txt", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // windows call home


	server->on("/favicon.ico", [](AsyncWebServerRequest *request) { request->send(404); });	// webpage icon

	server->on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse(200, "text/html", index_html);
		request->send(response);

		Serial.println("[WebServer][INFO] Served HTML page to client.");
	});

    server->on("/submit", HTTP_POST, [](AsyncWebServerRequest *request) {
        if (!(request->hasParam("WS_HOST", true) && request->hasParam("WS_PORT", true) &&
            request->hasParam("WS_URL", true) && request->hasParam("WIFI_SSID", true) &&
            request->hasParam("WIFI_PASSWORD", true))) {
            //Add validation
            request->send(400, "text/plain", "Missing parameters");
            
        } else {
            request->send(200, "text/plain", "Form submitted successfully");

            Serial.println("[WebServer][INFO] Received configuration parameters.");

            EEPROMUtils::saveConfig(request->getParam("WIFI_SSID", true)->value().c_str(), 
                                    request->getParam("WIFI_PASSWORD", true)->value().c_str(),
                                    request->getParam("WS_HOST", true)->value().c_str(),  
                                    request->getParam("WS_URL", true)->value().c_str(),
                                    request->getParam("WS_PORT", true)->value().toInt());

            webserverSubmitted = true;
        }
        
    });

	server->onNotFound([](AsyncWebServerRequest *request) {
		request->redirect(localIPURL);
	});

    server->begin();
    Serial.println("[WebServer][INFO] WebServer started.");
}

void WebServer::loop() {
    dnsServer->processNextRequest();
    delay(DNS_INTERVAL);
}