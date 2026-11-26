#pragma once

#include "freertos/idf_additions.h"
#include "uEvents.h"
#include "esp_wifi_types_generic.h"
#include "esp_netif_types.h"
#include "esp_netif.h"

#define SSID_BASE "uTinyOS_NETWORK"
#define PASS_BASE "uTinyOS_PASSWORD"

typedef enum{
	
	WIFI_INIT,
	WIFI_CONNECT,
	WIFI_DISCONNECT,
	WIFI_STA,
	WIFI_AP,
	WIFI_NON_STATUS
	
} wifiStatus;



typedef struct{
	
	esp_event_base_t eventBase;
	int32_t eventId;
	union{
		wifi_event_sta_disconnected_t sta_disconnected;
		wifi_event_ap_staconnected_t ap_connected;
		ip_event_got_ip_t got_ip;
	} data;
	
} wifiQueue;

extern QueueHandle_t uT2uWM;

extern wifiStatus wifi_status_flag;

void uWifi_init(void *pvParameters);
void uWifi_eventTask(void *pvParameters);
void uWifi_managerTask(void *pvParameters);

