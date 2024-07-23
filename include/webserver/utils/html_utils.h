#ifndef HTML_UTILS_H
#define HTML_UTILS_H

#define MAXIMUM_WIFI_NETWORKS 15
#define WIFI_SSID_SIZE 32
#include <Arduino.h>

#include "wifi_handler/wifi_handler.h"
#include "webserver/pages/index_html.h"

extern int wifi_index_start;
extern int wifi_index_end;
extern int status_index_start;
extern int status_index_end;

void replaceHtml(char* html, int i, const char* replacement);
int findKthOccurrence(char* html, char placeholder, int k);
void setWifiHtmlIndexes();
void updateWifiPage();
void updateWifiClass(const char* status);

#endif