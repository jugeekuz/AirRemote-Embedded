#include "webserver/webserver.h"

ArRequestHandlerFunction WebServer::route(const char *path) {

	if (strcmp(path,"/redirect-setup") == 0) {

		return redirectSetupOpt();

	} else if (strcmp(path,"/wifi-connect") == 0) {
		
		return wifiConnect();

	} else if (strcmp(path,"/wps-connect") == 0) {
		
		return wpsConnect();

	} else if (strcmp(path,"/wifi-status") == 0) {
		
		return checkWiFiStatus();

	} else if (strcmp(path,"/websocket-connect") == 0){

		return webSocketConnect();

	} else if (strcmp(path,"/websocket-status") == 0){

		return checkWebSocketStatus();

	} else if (strcmp(path,"/wifi-page") == 0){

		return getWifiHtml();

	} else if (strcmp(path,"/api-page") == 0){

		return getApiHtml();

	} else if (strcmp(path,"/success") == 0) {
		
		return getSuccessHtml();

	} else {
		
		return getErrorHtml();

	}
}

