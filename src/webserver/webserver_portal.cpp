#include "webserver/webserver_portal.h"
#include "webserver/webserver_router.h"

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
String ssid;
String password;
WebServer::WebServer(WiFiHandler *wifiHandler): wifiHandler(wifiHandler) {
    try{
        dnsServer = new DNSServer();
        server = new AsyncWebServer(80);
        setWifiHtmlIndexes(); //find indexes of placeholders in html
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

    // scanAndUpdateHTML();
    updateWifiPage();
	WiFi.mode(WIFI_MODE_APSTA);

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

    
    server->on("/redirect-setup", HTTP_POST, WebServerRouter::route("/redirect-setup", this));

    server->on("/wifi-submit", HTTP_POST, WebServerRouter::route("/wifi-submit", this));
    
    server->on("/wifi-complete-submit", HTTP_POST, WebServerRouter::route("/wifi-complete-submit", this));

    server->on("/wps-submit", HTTP_POST, WebServerRouter::route("/wps-submit", this));

    server->on("/wps-complete-submit", HTTP_POST, WebServerRouter::route("/wps-complete-submit", this));

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