#include "webserver/utils/html_utils.h"
int wifi_index_start = -1;
int wifi_index_end = -1;
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


void setWifiHtmlIndexes(){
	status_index_start = findKthOccurrence(wifi_html, '&', 1);
	status_index_end = findKthOccurrence(wifi_html, '&', 2);

	wifi_index_start = findKthOccurrence(wifi_html, '&', 3);
	wifi_index_end = findKthOccurrence(wifi_html, '&', 4);

	if (wifi_index_start == -1 || wifi_index_end == -1 || \
	    status_index_start == -1 || status_index_end == -1) {
            Serial.println("[WebServer][ERROR] Failed to find HTML indexes.");
        };
    char c[] = {' ', '\0'};
    
    replaceHtml(wifi_html, wifi_index_start, c);
    replaceHtml(wifi_html, wifi_index_start, c);
    replaceHtml(wifi_html, status_index_start, c);
    replaceHtml(wifi_html, status_index_end, c);
}

void updateWifiPage() {
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

    Serial.printf("%d networks found\n", n);
    Serial.printf("wifiOptionsLen: %d\n", wifiOptionsLen);

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
    replaceHtml(wifi_html, status_index_start, "idle");
}

void updateWifiClass(const char* status) {
    if (strlen(status) > 5) return;
    replaceHtml(wifi_html, status_index_start, status);
}