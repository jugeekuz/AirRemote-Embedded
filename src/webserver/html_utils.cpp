#include "webserver/html_utils.h"
int wifi_index_start = -1;
int wifi_index_end = -1;
int complete_index_start = -1;
int complete_index_end = -1;
int status_index_start = -1;
int status_index_end = -1;

void replaceHtml(char* html, int i, const char* replacement) {
    int htmlLen = strlen(html);
    int replacementLen = strlen(replacement);

    if (i < 0 || i >= htmlLen) return;
    
    for (int j = 0; j < replacementLen && (i + j) < htmlLen; j++) {
        html[i + j] = replacement[j];
    }
}

int findKthOccurrence(char* html, char placeholder, int k) {
    int len = strlen(html);
    int count = 0;

    for (int i = 0; i < len; ++i) {
        if (html[i] == placeholder) {
            count++;
            if (count == k) return i;
        }
    }
    return -1;
}

// Function that finds the indexes of placeholders `&` within the HTML page (to use to toggle classes) and replaces them with spaces.
void setWifiHtmlIndexes(){

	wifi_index_start = findKthOccurrence(wifi_html, '&', 1);
	wifi_index_end = findKthOccurrence(wifi_html, '&', 2);

	if (wifi_index_start == -1 || wifi_index_end == -1 ) {
            Serial.println("[WebServer][ERROR] Failed to find HTML indexes.");
        };
    char c[] = {' ', '\0'};
    
    replaceHtml(wifi_html, wifi_index_start, c);
    replaceHtml(wifi_html, wifi_index_start, c);
}

void updateWifiPage() {

    for (int i = wifi_index_start; i <= wifi_index_end; i++) {
        wifi_html[i] = ' ';
    }

	const char* left = "<option value=\"";
	const char* middle = "\">";
	const char* right = "</option>";

    // Calculate lengths of the string literals
    size_t leftLen = strlen(left);
    size_t middleLen = strlen(middle);
    size_t rightLen = strlen(right);

	size_t wifiOptionsLen = (leftLen + middleLen + rightLen + WIFI_SSID_SIZE * 2) * MAXIMUM_WIFI_NETWORKS + 1;
    char wifiOptions[wifiOptionsLen];

	int n = WiFi.scanNetworks();
	memset(wifiOptions, 0, wifiOptionsLen);

    if (n == 0) {
        strcat(wifiOptions, "<option value=\"\">No networks found</option>");
    } else {
        for (int i = 0; i < n; ++i) {
            strcat(wifiOptions, left);
            strcat(wifiOptions, WiFi.SSID(i).c_str());
            strcat(wifiOptions, middle);
            strcat(wifiOptions, WiFi.SSID(i).c_str());
            strcat(wifiOptions, right);
        }
	}
    if (wifi_index_start == -1 || wifi_index_end == -1 ) return;
    
	replaceHtml(wifi_html, wifi_index_start, wifiOptions);
}

