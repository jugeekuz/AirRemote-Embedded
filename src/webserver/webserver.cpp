#include "webserver/webserver.h"

#define DNS_INTERVAL 30
#define MAX_CLIENTS 4
#define WIFI_CHANNEL 6

#define WIFI_PORTAL_NAME "AirRemote-Portal"
#define WIFI_PORTAL_PASSWORD NULL
#define WEBSERVER_BLINK_FREQUENCY 2

DNSServer *dnsServer = nullptr;
AsyncWebServer *server = nullptr;

String ssid;
String password;

WebServer::WebServer(WiFiHandler *wifiHandler, WebSocketHandler *webSocket, AsyncLED *led): wifiHandler(wifiHandler), webSocket(webSocket), led(led) {
    try{
        dnsServer = new DNSServer();
        server = new AsyncWebServer(80);
        setWifiHtmlIndexes();
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


void WebServer::setup() {

	WiFi.mode(WIFI_MODE_APSTA);

	const IPAddress subnetMask(255, 255, 255, 0);

	WiFi.softAPConfig(localIP, gatewayIP, subnetMask);
	WiFi.softAP(WIFI_PORTAL_NAME, WIFI_PORTAL_PASSWORD, WIFI_CHANNEL, 0, MAX_CLIENTS);

    EEPROMUtils::saveConfig("", "", "", "", 0, "");
	esp_wifi_stop();
	esp_wifi_deinit();
	wifi_init_config_t my_config = WIFI_INIT_CONFIG_DEFAULT();
	my_config.ampdu_rx_enable = false;
	esp_wifi_init(&my_config);
	esp_wifi_start();
	vTaskDelay(100 / portTICK_PERIOD_MS); 

	dnsServer->setTTL(3600);
	dnsServer->start(53, "*", localIP);
    dnsServer->setErrorReplyCode(DNSReplyCode::NonExistentDomain);

    // Handle Windows, Android, Apple, and other common captive portal checks [https://github.com/CDFER/Captive-Portal-ESP32/tree/main]
	server->on("/connecttest.txt", [](AsyncWebServerRequest *request) { request->redirect("http://logout.net"); });	
	server->on("/wpad.dat", [](AsyncWebServerRequest *request) { request->send(404); });								
	server->on("/generate_204", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });		   
	server->on("/redirect", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   
	server->on("/canonical.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });	   
	server->on("/success.txt", [](AsyncWebServerRequest *request) { request->send(200); });					   
	server->on("/ncsi.txt", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   
    
    // In iOS 14+ Apple implemented draft [https://www.ietf.org/archive/id/draft-ietf-dnsop-svcb-https-07.txt], performing a Type 65 query instead of a standard A request.
    // DNSServer doesn't know what to do with the type 65 request and replies with a type A request, so the request fails.
    // If DNSServer is modified to return a not found response for type 65 queries, the iOS device will revert back to listening to the A record and process things normally.
    // For now, this hasn't been implemented yet and thus not working for iOS 14+ so just go to `captive.apple.com` and you'll be redirected.
	server->on("/hotspot-detect.html", [](AsyncWebServerRequest *request) { request->send(404); });  
	server->on("/library/test/success.html", [](AsyncWebServerRequest *request) { request->send(404); });  


	server->on("/favicon.ico", [](AsyncWebServerRequest *request) { request->send(404); });	

	server->on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {

		request->send(request->beginResponse(200, "text/html", index_html));

		Serial.println("[WebServer][INFO] Served HTML page to client.");
	});


    server->on("/redirect-setup", HTTP_ANY, route("/redirect-setup"));

    server->on("/wifi-connect", HTTP_ANY, route("/wifi-connect"));
    server->on("/wps-connect", HTTP_ANY, route("/wps-connect"));
    server->on("/wifi-page", HTTP_ANY, route("/wifi-page"));
    server->on("/api-page", HTTP_ANY, route("/api-page"));

    server->on("/websocket-connect", HTTP_ANY, route("/websocket-connect"));
    server->on("/websocket-status", HTTP_ANY, route("/websocket-status"));
    server->on("/wifi-status", HTTP_ANY, route("/wifi-status"));

    server->on("/success", HTTP_ANY, route("/success"));
    server->on("/error", HTTP_ANY, route("/error"));

	server->onNotFound([](AsyncWebServerRequest *request) { request->redirect(localIPURL); });

    server->begin();
    Serial.println("[WebServer][INFO] WebServer started.");
    this->led->blink(WEBSERVER_BLINK_FREQUENCY);
}

void WebServer::loop() {
    dnsServer->processNextRequest();
    delay(DNS_INTERVAL);
}